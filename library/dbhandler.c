#include "dbhandler.h"
#include <stdio.h>

void init_db(sqlite3 *db) {
  char *sql;
  char query[1024] = {0};
  // 如果数据表不存在创建它
  sql = SQL_CREATE_TABLE;
  if(sqlite3_exec(db, sql, NULL, NULL, NULL) != SQLITE_OK) {
    printf("error on sqlite_exec(): %s\n", sqlite3_errmsg(db));
    exit(-1);
  }
  // 如果查询日志表不存在创建它
  sql = SQL_CREATE_LOG_TABLE;
  if(sqlite3_exec(db, sql, NULL, NULL, NULL) != SQLITE_OK) {
    printf("error on sqlite_exec(): %s\n", sqlite3_errmsg(db));
    exit(-1);
  }
}
