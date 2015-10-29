// Serge Voilokov, 2015
// Dumb http server

#include "struct.h"

struct httpreq
{
	int fd;
	const char* method;
	const char* path;
	const char* sec_websocket_key;
	int connection_upgrade;
	int ownership_taken;
};

struct httpd
{
	int listen_socket;
	void (*handler)(struct httpreq *req, struct buf *resp, void *arg);
	void *nahdler_arg;
};

int httpd_start(struct httpd *d, const char *address, int port);
int httpd_accept(struct httpd *d);
