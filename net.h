#include "struct.h"

struct message
{
	const char *to;        /* recipient */
	const char *from;      /* sender */
	const char *subject;   /* message subject */
	const char *body;      /* message body */
	const char *content_type;
};

struct httpreq_opts
{
	const char *method;
	const char *authorization;
	const char *cookie_file;        /* use a file as a cookie jar */
	const char *post_data;          /* do POST request with post_data */
	const char *resp_fname;         /* write response to file */
	int debug;                      /* curl verbose output */
};

int send_email(const struct message *m, const char *password_file);
int httpreq(const char *url, struct buf *b, struct httpreq_opts *opts);
