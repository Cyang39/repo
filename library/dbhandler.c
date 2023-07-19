#include "dbhandler.h"
#include <stdio.h>

int callback_count(void *count, int argc, char **argv, char **azColName) {
  *(int *)count = atoi(argv[0]);
  return 0;
}

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
  if (check_db_by_username(db, user->name) == 0) { // 用户名已存在
    return -1;
  }
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

int check_db_by_username(sqlite3 *db, char *name) {
  int count;
  char *sql;
  char query[1024] = {0};
  sprintf(query, SQL_CHECK_TABLE_BY_USERNAME, name);
  sql = query;
  if (sqlite3_exec(db, sql, callback_count, &count, NULL) != SQLITE_OK) {
    printf("error on sqlite_exec(): %s\n", sqlite3_errmsg(db));
    exit(-1);
  }
  if (0 == count) {
    return -1;
  }
  return 0;
}

int check_db_by_username_and_password(sqlite3 *db, char *name, char *password) {
  int count;
  char *sql;
  char query[1024] = {0};
  sprintf(query, SQL_CHECK_TABLE_BY_USERNAME_AND_PASSWORD, name, password);
  sql = query;
  if (sqlite3_exec(db, sql, callback_count, &count, NULL) != SQLITE_OK) {
    printf("error on sqlite_exec(): %s\n", sqlite3_errmsg(db));
    exit(-1);
  }
  if (0 == count) {
    return -1;
  }
  return 0;
}
