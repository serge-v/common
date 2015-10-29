// Serge Voilokov, 2015
// Dumb http server

#include <stdio.h>
#include <errno.h>
#include <err.h>
#include <memory.h>
#include "socket.h"
#include "http.h"

int
httpreq_parse(char* s, int len, struct httpreq* req)
{
	char* p = s;
	memset(req, 0, sizeof(struct httpreq));

	req->method = p;
	p = strchr(p, ' ');
	if (p < s || p >= s + len)
		return -1;
	*p = 0;
	p++;
	req->path = p;

	p = strchr(p, ' ');
	if (p < s || p >= s + len)
		return -1;

	*p = 0;
	p++;

	const char http_sig[] = "HTTP/1.1\r\n";
	const size_t sig_size = sizeof(http_sig) - 1;

	if (strncmp(p, http_sig, sig_size) != 0) {
		warn("invalid first line");
		return -1;
	}

	p += sig_size;

	const char conn_upgrade[] = "Connection: Upgrade\r\n";
	const char hdr_sec_websocket_key[] = "Sec-WebSocket-Key: ";

	while (p != NULL && *p != '\r') {
		if (strncmp(p, conn_upgrade, sizeof(conn_upgrade)-1) == 0) {
			req->connection_upgrade = 1;
		} else if (strncmp(p, hdr_sec_websocket_key, sizeof(hdr_sec_websocket_key)-1) == 0) {
			req->sec_websocket_key = p + sizeof(hdr_sec_websocket_key)-1;
		}

		p = strchr(p, '\r');
		if (p == NULL) {
			warn("invalid request");
			return -1;
		}
		*p = 0;
		p++;
		if (*p != '\n') {
			warn("invalid request");
			return -1;
		}
		*p = 0;
		p++;
	}

	return 0;
}

int
httpd_start(struct httpd *d, const char *address, int port)
{
	if ((d->listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
		perror("cannot create socket");
		return -1;
	}

	if (set_reuse_addr(d->listen_socket) == -1) {
		perror("cannot reuse socket");
		goto err;
	}

	if (bind_address(d->listen_socket, address, port) == -1) {
		perror("cannot bind socket");
		goto err;
	}

	printf("Started on %s:%d\n", address, port);

	if (listen(d->listen_socket, 100) == -1) {
		perror("cannot listen socket");
		goto err;
	}

	return 0;
err:
	shutdown(d->listen_socket, SHUT_RDWR);
	return -1;
}

int
httpd_accept(struct httpd *d)
{
	int asocket;
	ssize_t size;
	char rbuf[20000];
	struct httpreq req;
	struct buf resp;
	struct sockaddr si_from;
	socklen_t addrlen = sizeof(si_from);

	buf_init(&resp);

	asocket = accept(d->listen_socket, &si_from, &addrlen);
	if (asocket == -1) {
		perror("cannot accept socket");
		goto err;
	}

	size = recv(asocket, rbuf, sizeof(rbuf), 0);
	if (size == -1) {
		perror("cannot recv");
		goto err;
	}

	if (size == 0)
		goto err;

	printf("len: %lu, buf:\n%s\n", size, rbuf);

	httpreq_parse(rbuf, size, &req);
	printf("method: %s %s %zu\n", req.method, req.path, size);

	req.fd = asocket;
	d->handler(&req, &resp, d->nahdler_arg);

	if (!req.ownership_taken) {
		send(asocket, resp.s, resp.len, 0);
		close(asocket);
	}

	resp.len = 0;
	rbuf[size] = 0;

	return 0;
err:
	return -1;
}
