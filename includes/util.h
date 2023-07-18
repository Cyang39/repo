#ifndef __UTIL_H__
#define __UTIL_H__

#include <string.h>
#include <stdio.h>

#define ERR_MSG(msg)                                                           \
  do {                                                                         \
    fprintf(stderr, "line: %d\n", __LINE__);                                   \
    perror(msg);                                                               \
  } while (0)

enum cmd_type {
  MSG_LOGIN = 1,
  MSG_LOGOUT,
  MSG_SEND,
  MSG_RECV,
  MSG_LIST,
  MSG_ERROR,
  MSG_OK,
  MSG_EXIT,
  MSG_QUERY,
  MSG_UPDATE,
  MSG_DELETE,
};

enum user_type { USER_NORMAL = 1, USER_ADMIN };

// 职员信息结构体
struct info {
  char name[20]; // 职员姓名
  int age;       // 职员年龄
};

// 消息结构体
struct message {
  enum user_type utype; // 用户类型
  enum cmd_type ctype;  // 命令类型
  char buf[1024];       // 消息内容
  struct info st;
};

#endif // !__UTIL_H__
