#include "fs.h"
#include <sys/stat.h>

bool
exists(const char *path)
{
	struct stat st;
	return stat(path, &st) == 0;
}
