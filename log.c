#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
#include <err.h>
#include <string.h>

static FILE *logger;

void
log_open(const char *fname)
{
	logger = fopen(fname, "at");
	if (logger == NULL)
		err(1, "cannot open log file %s", fname);
}

void
log_close(const char *fname)
{
	if (logger != NULL)
		fclose(logger);
}

void
logi(const char *fmt, ...)
{
	if (logger == NULL)
		errx(1, "logger is not open");

	time_t now = time(NULL);
	char ts[50];
	struct tm *utctm = gmtime(&now);
	strftime(ts, 50, "%F %T INFO  ", utctm);

	va_list args, cp;

	va_start(args, fmt);
	va_copy(cp, args);
	fputs(ts, logger);
	vfprintf(logger, fmt, args);
	fputs("\n", logger);
	va_end (args);
	fflush(logger);

	va_start(args, fmt);
	va_copy(cp, args);
	printf("%s", ts);
	vprintf(fmt, args);
	puts("");
	va_end (args);
}

void
logwarn(const char *fmt, ...)
{
	if (logger == NULL)
		errx(1, "logger is not open");

	time_t now = time(NULL);
	char ts[50];
	struct tm *utctm = gmtime(&now);
	strftime(ts, 50, "%F %T WARN  ", utctm);

	va_list args, cp;

	va_start(args, fmt);
	va_copy(cp, args);
	fputs(ts, logger);
	vfprintf(logger, fmt, args);
	fputs("\n", logger);
	va_end (args);
	fflush(logger);

	va_start(args, fmt);
	va_copy(cp, args);
	fprintf(stderr, "\x1b[32m%s", ts);
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\x1b[0m\n");
	va_end (args);
}

void
logfatal(const char *fmt, ...)
{
	if (logger == NULL)
		errx(1, "logger is not open");

	int err = errno;

	time_t now = time(NULL);
	char ts[50];
	struct tm *utctm = gmtime(&now);
	strftime(ts, 50, "%F %T FATAL ", utctm);

	va_list args, cp;

	va_start(args, fmt);
	va_copy(cp, args);
	fputs(ts, logger);
	vfprintf(logger, fmt, args);
	if (err > 0)
		fprintf(logger, ": %d: %s", err, strerror(err));
	fputs("\n", logger);
	va_end (args);
	fflush(logger);

	va_start(args, fmt);
	va_copy(cp, args);
	fprintf(stderr, "\x1b[31m%s", ts);
	vfprintf(stderr, fmt, args);
	if (err > 0)
		fprintf(stderr, ": %d: %s", err, strerror(err));
	fprintf(stderr, "\x1b[0m\n");
	va_end (args);
	exit(1);
}
