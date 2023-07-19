#ifndef __DBHANDLER_H__
#define __DBHANDLER_H__

#include "util.h"
#include <sqlite3.h>

#define SQL_CREATE_TABLE                                                       \
  "CREATE TABLE IF NOT EXISTS employee (name TEXT, password TEXT,"              \
  "age INTEGER, department TEXT, sex INTEGER, phone TEXT, type INTEGER);"

#define SQL_CREATE_LOG_TABLE                                                   \
  "CREATE TABLE IF NOT EXISTS log (name TEXT, time TEXT, action TEXT);"

#define SQL_INSERT_TABLE                                                       \
  "INSERT INTO employee VALUES ('%s', '%s', %d, '%s', %d, '%s', %d);"

void init_db(sqlite3 *db);
int insert_db(sqlite3 *db, struct info *user);

#endif // !__DBHANDLER_H__
