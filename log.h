#include <stdio.h>

void log_open(const char *fname);
void log_set(FILE* log);
void log_close();
void logi(const char *fmt, ...);
void logwarn(const char *fmt, ...);
void logfatal(const char *fmt, ...);
