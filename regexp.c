#include "regexp.h"
#include <regexp.h>

static void
print_rex_error(int errcode, const regex_t *preg)
{
	char buf[1000];
	size_t rc = regerror(errcode, preg, buf, sizeof(buf));
	if (rc <= 0)
		err(1, "cannot get error string for code: %d", errcode);
	err(1, "%s", buf);
}

static void
regex_compile(regex_t* rex, const char* pattern)
{
	int rc = regcomp(rex, pattern, REG_EXTENDED);
	if (rc != 0) {
		print_rex_error(rc, rex);
	}
}

void
regex_replace(const char *rex_what, const char *replacement, char **text)
{
	regex_t rex;
	regex_compile(&rex, rex_what);

	regmatch_t match[1];

	int rc = regexec(&rex, *text, 1, match, 0);
	if (rc == REG_NOMATCH)
		err(1, "no match. regex: %p", rex_what);

	const char *orig = *text;
	size_t len = strlen(orig) - match[0].rm_eo + match[0].rm_so + 1;
	if (replacement != NULL)
		len += strlen(replacement);
	char *s = calloc(1, len);

	strncpy(s, orig, match[0].rm_so);  /* copy chunk before regexp */
	if (replacement != NULL)
		strcat(s, replacement);    /* copy replacement */
	strcat(s, &orig[match[0].rm_eo]);  /* copy chunk after replacement */
	s[len-1] = 0;

	free(*text);
	regfree(&rex);

	*text = s;
}
