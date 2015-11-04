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
