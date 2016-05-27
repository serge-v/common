#include <stdbool.h>
#include "struct.h"

bool exists(const char *path);
int expired(const char *fname, int age);
int read_text(const char *fname, struct buf *buf);
