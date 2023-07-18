#ifndef __DBHANDLER_H__
#define __DBHANDLER_H__

#include "util.h"
#include <sqlite3.h>

#define SQL_CREATE_TABLE                                                       \
  "CREATE TABLE IF NOT EXISTS employee (name TEXT, "                           \
  "age INTEGER, department TEXT, sex INTEGER, phone TEXT);"

#define SQL_CREATE_LOG_TABLE \
  "CREATE TABLE IF NOT EXISTS log (name TEXT, time TEXT, action TEXT);"

void init_db(sqlite3 *db);

#endif // !__DBHANDLER_H__
