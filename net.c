#include <err.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>

#include "net.h"

/* ========= send_email using curl ========= */

static int
message_compose(const struct message *m, struct buf *b)
{
	if (m == NULL) {
		fprintf(stdout, "Message is null\n");
		return 1;
	}

	if (m->to == NULL) {
		fprintf(stdout, "Recipient is not specified\n");
		return 1;
	}

	if (m->from == NULL) {
		fprintf(stdout, "Sender is not specified\n");
		return 1;
	}

	if (m->body == NULL) {
		fprintf(stdout, "Message body is empty\n");
		return 1;
	}

	const char *content_type = "text/plain";

	if (m->content_type != NULL)
		content_type = m->content_type;

	char s[1024];
	const size_t sz = sizeof(s);

	/* "Date: Mon, 29 Nov 2010 21:54:29 +1100\r\n" */

	time_t now = time(NULL);

	int n = strftime(s, sz, "Date: %a, %d %b %Y %T %z\r\n", gmtime(&now));
	buf_append(b, s, n);

	n = snprintf(s, sz, "To: %s\r\n", m->to);
	buf_append(b, s, n);

	n = snprintf(s, sz, "From: %s\r\n", m->from);
	buf_append(b, s, n);

	buf_appendf(b, "Content-Type: multipart/mixed; boundary=frontier\r\n");
	buf_appendf(b, "MIME-Version: 1.0\r\n");

	if (m->subject != NULL) {
		n = snprintf(s, sz, "Subject: %s\r\n", m->subject);
		buf_append(b, s, n);
	}

	buf_append(b, "\r\n", 2);
	buf_appendf(b, "--frontier\r\n");
	buf_appendf(b, "Content-Type: %s; charset=\"us-ascii\"\r\n", m->content_type);
	buf_appendf(b, "MIME-Version: 1.0\r\n");
	buf_appendf(b, "Content-Transfer-Encoding: 7bit\r\n");
	buf_append(b, "\r\n", 2);
	buf_append(b, m->body, strlen(m->body));
	buf_appendf(b, "--frontier--\r\n");
	buf_append(b, "\r\n", 2);

	return 0;
}

struct upload_status
{
	const char *text; /* composed message */
	size_t pos;       /* current sending position */
	size_t len;       /* message length */
};

static size_t
read_data(void *ptr, size_t size, size_t nmemb, void *userp)
{
	struct upload_status *status = (struct upload_status *)userp;
	const char *data;

	if ((size == 0) || (nmemb == 0) || ((size * nmemb) < 1))
		return 0;

	data = &status->text[status->pos];

	size_t write_len = size * nmemb;
	size_t remain_len = status->len - status->pos;

	if (remain_len < write_len)
		write_len = remain_len;

	memcpy(ptr, data, write_len);
	status->pos += write_len;

	return write_len;
}

struct smtp_info
{
	const char *server;            /* smtp server */
	const char *user;              /* smtp user */
	const char *password_file;     /* file with password */
	int debug;                     /* using local server without credentials */
};

static int
curl_smtp_send(const struct smtp_info *s, const struct message *m)
{
	CURL *curl;
	CURLcode res = CURLE_OK;
	struct curl_slist *recipients = NULL;
	struct upload_status status;
	struct buf b;

	memset(&b, 0, sizeof(struct buf));

	int rc = message_compose(m, &b);
	if (rc != 0) {
		fprintf(stderr, "Cannot compose message\n");
		return 1;
	}

	status.pos = 0;
	status.len = b.len;
	status.text = b.s;

	curl = curl_easy_init();
	if (!curl) {
		fprintf(stderr, "Cannot init curl\n");
		return 1;
	}

	FILE *f = fopen(s->password_file, "rt");
	if (f == NULL) {
		fprintf(stderr, "Cannot read password from %s\n", s->password_file);
		return 1;
	}

	char password[100];
	const size_t pwdlen = 16;

	memset(password, 0, sizeof(password));
	size_t n = fread(password, 1, sizeof(password), f);
	fclose(f);

	if (n == pwdlen+1 && password[pwdlen] == '\n') {
		password[pwdlen] = 0;
		n = pwdlen;
	}

	if (n != 16) {
		fprintf(stderr, "Invalid gmail app password for pin %s\n", s->password_file);
		return 1;
	}

	if (s->debug) {
		curl_easy_setopt(curl, CURLOPT_URL, "smtp://127.0.0.1:8025");
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
	} else {
		curl_easy_setopt(curl, CURLOPT_URL, s->server);
		curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
		curl_easy_setopt(curl, CURLOPT_USERNAME, s->user);
		curl_easy_setopt(curl, CURLOPT_PASSWORD, password);
	}

	curl_easy_setopt(curl, CURLOPT_MAIL_FROM, m->from);
	recipients = curl_slist_append(recipients, m->to);
	curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_data);
	curl_easy_setopt(curl, CURLOPT_READDATA, &status);
	curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

	res = curl_easy_perform(curl);

	if (res != CURLE_OK)
		fprintf(stderr, "Cannot send email. Error: %s\n",
			curl_easy_strerror(res));

	curl_slist_free_all(recipients);
	curl_easy_cleanup(curl);

	return (int)res;
}

int
send_email(const struct message *m, const char *password_file)
{
	struct smtp_info gmail = {
		.server = "smtp://smtp.gmail.com:587",
		.user = "serge0x76@gmail.com",
		.password_file = password_file,
		.debug = 0
	};

	return curl_smtp_send(&gmail, m);
}

/* ========= http request using curl ========= */

#define HTTP_FOUND 302

struct response
{
	int error;                  /* curl error */
	int code;                   /* http error code */
	size_t len;                 /* http content length */
	struct buf* buf;            /* received data */
	struct httpreq_opts *opts;  /* request options */
};

/* curl related */

static size_t
header_callback(char *buffer, size_t size, size_t nitems, void *userdata)
{
	struct response *resp = (struct response *)userdata;
	size_t len = size * nitems;

	if (strncmp("HTTP/1.", buffer, 7) == 0)
		resp->code = atoi(&buffer[9]);

	if (strncmp("Content-Length: ", buffer, 16) == 0)
		resp->len = atoi(&buffer[16]);

	return len;
}

static size_t
write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	struct response *resp = (struct response *)userdata;
	size_t len = size * nmemb;

	if (len > 0) {
		if (resp->opts != NULL && resp->opts->debug) {
			fwrite(ptr, size, nmemb, stderr);
		}
		buf_append(resp->buf, ptr, len);
	}

	return len;
}

static size_t
empty_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	struct response *resp = (struct response *)userdata;
	size_t len = size * nmemb;

	if (resp->opts != NULL && resp->opts->debug) {
		fwrite(ptr, size, nmemb, stderr);
	}

	return len;
}

int
httpreq(const char *url, struct buf *b, struct httpreq_opts *opts)
{
	struct response resp;
	memset(&resp, 0, sizeof(struct response));
	struct buf local_buf;

	resp.buf = b;
	resp.opts = opts;

	if (b == NULL) {
		buf_init(&local_buf);
		resp.buf = &local_buf;
	}

	CURL* curl;
	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);

	if (opts != NULL && opts->debug)
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

	if (opts != NULL && opts->cookie_file != NULL) {
		curl_easy_setopt(curl, CURLOPT_COOKIEFILE, opts->cookie_file);
		curl_easy_setopt(curl, CURLOPT_COOKIEJAR, opts->cookie_file);
	}

	if (b == NULL && (opts == NULL || opts->resp_fname == NULL)) {
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, empty_callback);
	} else {
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);
	}

	if (opts != NULL && opts->post_data != NULL)
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, opts->post_data);

	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &resp);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);

	struct curl_slist *headers = NULL;

	if (opts->authorization != NULL)
		headers = curl_slist_append(headers, opts->authorization);

	if (headers != NULL)
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	if (opts->method != NULL && strcmp(opts->method, "PUT") == 0)
		curl_easy_setopt(curl, CURLOPT_PUT, 1L);

	const char *useragent =
		"Mozilla/5.0 (Macintosh; U; Linux i686; en-US; rv:1.8.0.10) "
		"Gecko/20070223 CentOS/1.5.0.10-0.1.el4.centos Firefox/2.0b2";

	curl_easy_setopt(curl, CURLOPT_USERAGENT, useragent);

	CURLcode curl_err = curl_easy_perform(curl);

	if (curl_err != CURLE_OK)
		fprintf(stderr, "fetch error: %s\n", curl_easy_strerror(curl_err));

	curl_easy_cleanup(curl);

	if (curl_err == CURLE_OK && opts != NULL && opts->resp_fname != NULL && resp.buf != NULL) {
		FILE *f = fopen(opts->resp_fname, "wb");
		if (f == NULL)
			err(1, "cannot open file: %s", opts->resp_fname);
		fwrite(resp.buf->s, 1, resp.buf->len, f);
		fclose(f);
	}

	if (curl_err == CURLE_OK && resp.code == HTTP_FOUND)
		curl_err = HTTP_FOUND;

	if (b == NULL)
		buf_clean(&local_buf);

	return curl_err;
}
