#include "fs.h"
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <inttypes.h>

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

int
read_text(const char *fname, struct buf *buf)
{
	struct stat st;
	FILE *f = NULL;
	char *buff = NULL;
	int ret = -1;

	if (stat(fname, &st) != 0) {
		printf("Cannot stat file %s. Error: %d\n", fname, errno);
		goto out;
	}

	f = fopen(fname, "rt");
	if (f == NULL) {
		printf("Cannot open file %s. Error: %d\n", fname, errno);
		goto out;
	}

	buff = malloc(st.st_size + 1);
	if (buff == NULL) {
		printf("Cannot allocate %" PRId64 " bytes. Error: %d\n", st.st_size + 1, errno);
		goto out;
	}

	fread(buff, 1, st.st_size, f);
	fclose(f);
	f = NULL;
	buff[st.st_size] = 0;
	buf->s = buff;
	buf->len = st.st_size;
	buf->cap = st.st_size;
	ret = 0;
out:
	if (f != NULL)
		fclose(f);

	return ret;
}

