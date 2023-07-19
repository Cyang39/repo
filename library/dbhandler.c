#include "dbhandler.h"
#include <stdio.h>

void init_db(sqlite3 *db) {
  char *sql;
  // 如果数据表不存在创建它
  sql = SQL_CREATE_TABLE;
  if (sqlite3_exec(db, sql, NULL, NULL, NULL) != SQLITE_OK) {
    printf("error on sqlite_exec(): %s\n", sqlite3_errmsg(db));
    exit(-1);
  }
  // 如果查询日志表不存在创建它
  sql = SQL_CREATE_LOG_TABLE;
  if (sqlite3_exec(db, sql, NULL, NULL, NULL) != SQLITE_OK) {
    printf("error on sqlite_exec(): %s\n", sqlite3_errmsg(db));
    exit(-1);
  }
}

int insert_db(sqlite3 *db, struct info *user) {
  // TBD: 先用 query_db 检查是否存在
  char *sql;
  char query[1024] = {0};
  // 插入数据
  sprintf(query, SQL_INSERT_TABLE, user->name, user->password, user->age,
          user->department, user->sex, user->phone, user->type);
  sql = query;
  if (sqlite3_exec(db, sql, NULL, NULL, NULL) != SQLITE_OK) {
    printf("error on sqlite_exec(): %s\n", sqlite3_errmsg(db));
    exit(-1);
  }
  return 0;
}

