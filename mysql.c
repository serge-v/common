#include <stdio.h>
#include <string.h>
#include <err.h>
#include <mysql/mysql.h>

static MYSQL *mysql;
static MYSQL *my;

MYSQL *
db_open(const char *dbhost, const char *dbname, const char *dbuser, const char *dbpassword)
{
	mysql = mysql_init(NULL);
	if (mysql == NULL)
		err(1, "cannot init mysql library: %s", mysql_error(mysql));

	my = mysql_real_connect(mysql, dbhost, dbuser, dbpassword, dbname, 0, NULL, 0);
	if (my == NULL) {
		mysql_close(mysql);
		err(1, "cannot connect to database. Error: %s", mysql_error(mysql));
	}

	return my;
}


static void
dump_sql(const char *query, const char *fname)
{
	int i;
	MYSQL_RES *res = NULL;
	FILE *f;

	int rc = mysql_real_query(mysql, query, strlen(query));
	if (rc != 0)
		err(1, "cannot exec query. %s", mysql_error(mysql));

	res = mysql_use_result(mysql);
	if (res == NULL)
		err(1, "cannot get results. %s", mysql_error(mysql));

	f = fopen(fname, "wt");
	if (f == NULL)
		err(1, "cannot open file %s", fname);

	for (i = 0; i < res->field_count; i++) {
		fprintf(f, "%s", res->fields[i].name);
		if (i < res->field_count - 1)
			fprintf(f, "\t");
	}
	fprintf(f, "\n");

	while (1) {
		MYSQL_ROW row = mysql_fetch_row(res);
		if (row == NULL)
			break;

		for (i = 0; i < res->field_count; i++) {
			fprintf(f, "%s", row[i]);
			if (i < res->field_count - 1)
				fprintf(f, "\t");
		}
		fprintf(f, "\n");
	}

	if (res != NULL)
		mysql_free_result(res);

	fclose(f);
	printf("generated: %s\n", fname);
}
