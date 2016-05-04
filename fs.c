#include "fs.h"
#include <sys/stat.h>
#include <time.h>

bool
exists(const char *path)
{
	struct stat st;
	return stat(path, &st) == 0;
}

int
expired(const char *fname, int age)
{
	struct stat st;

	int rc = stat(fname, &st);
	if (rc != 0) {
		return 1;
	}

	if (st.st_mtime + age < time(NULL)) {
		return 2;
	}

	return 0;
}
