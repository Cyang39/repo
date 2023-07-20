#include "dbhandler.h"

int callback_count(void *count, int argc, char **argv, char **azColName) {
  *(int *)count = atoi(argv[0]);
  return 0;
}

int check_admin_db(sqlite3 *db) {
  int count;
  char *sql;
  sql = SQL_COUNT_ADMIN;
  if (sqlite3_exec(db, sql, callback_count, &count, NULL) != SQLITE_OK) {
    printf("error on sqlite_exec(): %s\n", sqlite3_errmsg(db));
    exit(-1);
  }
  if (0 == count) {
    return -1;
  }
  return 1;
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
  // 如果管理员不存在创建它
  if (check_admin_db(db) == -1) {
    struct info admin;
    strcpy(admin.name, "admin");
    strcpy(admin.password, "admin");
    admin.age = 0;
    admin.sex = FEMALE;
    admin.type = USER_ADMIN;
    strcpy(admin.department, "admin");
    strcpy(admin.phone, "admin");
    insert_db(db, &admin);
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

int query_info_db_by_username(sqlite3 *db, char *name, struct info *st) {
  char query[1024] = {0};
  char **result;
  int nrow, ncolumn;
  char *errmsg;
  sprintf(query, SQL_QUERY_TABLE_BY_USERNAME, name);
  if (sqlite3_get_table(db, query, &result, &nrow, &ncolumn, &errmsg) !=
      SQLITE_OK) {
    printf("error on sqlite3_get_table(): %s\n", errmsg);
    exit(-1);
  }
  if (nrow == 0) {
    return -1;
  }
  memset(st, 0, sizeof(struct info));
  memcpy(st->name, result[ncolumn], strlen(result[ncolumn]));
  memcpy(st->password, result[ncolumn + 1], strlen(result[ncolumn + 1]));
  st->age = atoi(result[ncolumn + 2]);
  memcpy(st->department, result[ncolumn + 3], strlen(result[ncolumn + 3]));
  st->sex = (enum user_sex)atoi(result[ncolumn + 4]);
  memcpy(st->phone, result[ncolumn + 5], strlen(result[ncolumn + 5]));
  st->type = (enum user_type)atoi(result[ncolumn + 6]);
  sqlite3_free_table(result);
  return 0;
}

enum user_type check_user_type(sqlite3 *db, char *name) {
  struct info st;
  query_info_db_by_username(db, name, &st);
  return st.type;
}

int delete_db_by_username(sqlite3 *db, char *name) {
  if (check_db_by_username(db, name) == -1) {
    return -1;
  }
  char *sql;
  char query[1024] = {0};
  sprintf(query, SQL_REMOVE_TABLE_BY_USERNAME, name);
  sql = query;
  if (sqlite3_exec(db, sql, NULL, NULL, NULL) != SQLITE_OK) {
    printf("error on sqlite_exec(): %s\n", sqlite3_errmsg(db));
    exit(-1);
  }
  return 0;
}

int update_db_by_username(sqlite3 *db, char *name, struct info *user) {
  if (check_db_by_username(db, name) == -1) {
    return -1;
  }
  char *sql;
  char query[1024] = {0};
  sprintf(query, SQL_UPDATE_TABLE_BY_USERNAME, user->name, user->password,
          user->age, user->department, user->sex, user->phone, user->type,
          name);
  sql = query;
  if (sqlite3_exec(db, sql, NULL, NULL, NULL) != SQLITE_OK) {
    printf("error on sqlite_exec(): %s\n", sqlite3_errmsg(db));
    exit(-1);
  }
  return 0;
}
