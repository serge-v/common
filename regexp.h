#include "struct.h"
#include <regex.h>

void regex_compile(regex_t* rex, const char* pattern);
void buf_replace(struct buf *buf, const char *rex_what, const char *replacement);
