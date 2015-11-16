// Serge Voilokov, 2015
// Convert text file into C string.
// Creates pair of .h and .c files.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

static void
usage()
{
	printf(
	       "usage: mkresource FILE\n"
	       "Converts text file to C string.\n"
	       "Produces .h and .c files pair.\n");
}

static const char *fname;

#define NSIZE 200

static char name[NSIZE];
static char h_fname[NSIZE+3];
static char c_fname[NSIZE+3];

static void
create_names()
{
	const char *slash = strrchr(fname, '/');
	if (slash != NULL)
		slash++;
	else
		slash = fname;

	strncpy(name, slash, NSIZE);

	char *dot = strrchr(name, '.');
	if (dot != NULL)
		*dot = '_';

	strncpy(h_fname, slash, NSIZE);
	strcat(h_fname, ".h");

	strncpy(c_fname, slash, NSIZE);
	strcat(c_fname, ".c");
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		usage();
		exit(1);
	}

	FILE *fin, *fh, *fc;
	fname = argv[1];
	create_names();

	fin = fopen(fname, "rt");
	if (fin == NULL)
		err(1, "cannot open input file: %s", fname);

	fc = fopen(c_fname, "wt");
	if (fc == NULL)
		err(1, "cannot open output C file %s", c_fname);

	fh = fopen(h_fname, "wt");
	if (fh == NULL)
		err(1, "cannot open output H file %s", h_fname);

	fprintf(fh,
		"extern const char %s[];\n"
		"extern const size_t %s_size;\n",
		name, name);

	fprintf(fc,
		"#include <unistd.h>\n"
		"\n"
		"const char %s[] = \"\"\n\"",
		name);

	int c = 0;
	size_t size = 0;

	while ((c = fgetc(fin)) != EOF) {
		size++;
		switch (c) {
			case '\\':
				fputs("\\\\", fc);
				break;
			case '\"':
				fputs("\\\"", fc);
				break;
			case '\n':
				fputs("\\n\"\n\"", fc);
				break;
			default:
				fputc(c, fc);
		}
	}

	fprintf(fc,
		"\";\n"
		"\n"
		"const size_t %s_size = %zu;\n",
		name,
		size);

	fclose(fc);
	fclose(fh);
	fclose(fin);
}
