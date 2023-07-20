#ifndef __DBHANDLER_H__
#define __DBHANDLER_H__

#include "util.h"
#include <sqlite3.h>

#define SQL_CREATE_TABLE                                                       \
  "CREATE TABLE IF NOT EXISTS employee (name TEXT, password TEXT,"             \
  "age INTEGER, department TEXT, sex INTEGER, phone TEXT, type INTEGER);"

#define SQL_CREATE_LOG_TABLE                                                   \
  "CREATE TABLE IF NOT EXISTS log (name TEXT, time TEXT, action TEXT);"

#define SQL_INSERT_TABLE                                                       \
  "INSERT INTO employee VALUES ('%s', '%s', %d, '%s', %d, '%s', %d);"

#define SQL_CHECK_TABLE_BY_USERNAME                                            \
  "SELECT COUNT(*) FROM employee WHERE name = \"%s\";"

#define SQL_CHECK_TABLE_BY_USERNAME_AND_PASSWORD                              \
  "SELECT COUNT(*) FROM employee WHERE name = \"%s\" AND password = \"%s\";"

#define SQL_QUERY_TABLE_BY_USERNAME                                            \
  "SELECT * FROM employee WHERE name = \"%s\";"

void init_db(sqlite3 *db);
// 插入员工数据到数据库
int insert_db(sqlite3 *db, struct info *user);
// 检查用户名是否存在
int check_db_by_username(sqlite3 *db, char *name);
// 检查用户名和密码是否匹配
int check_db_by_username_and_password(sqlite3 *db, char *name, char *password);
// 从数据库获取员工结构体
int query_info_db_by_username(sqlite3 *db, char *name, struct info *st);

#endif // !__DBHANDLER_H__
