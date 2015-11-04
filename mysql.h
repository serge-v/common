#include <mysql/mysql.h>

MYSQL *db_open(const char *dbhost, const char *dbname, const char *dbuser, const char *dbpassword);
void db_close(MYSQL *mysql);
