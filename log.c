#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <err.h>

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
	struct tm utctm;

	gmtime_r(&now, &utctm);
	strftime(ts, 50, "%F %T INFO  ", &utctm);

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
