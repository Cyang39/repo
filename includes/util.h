#ifndef __UTIL_H__
#define __UTIL_H__

#define DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERR_MSG(msg)                                                           \
  do {                                                                         \
    fprintf(stderr, "line: %d\n", __LINE__);                                   \
    perror(msg);                                                               \
  } while (0)

#define DEBUG_MSG(msg) \
  do {                 \
    printf("[%s, %d]%s\n", __FILE_NAME__, __LINE__, msg); \
  } while (0)

enum user_type { USER_NORMAL = 1, USER_ADMIN };
enum user_sex { FEMALE = 1, MALE };

// 职员信息结构体
struct info {
  char name[20];       // 职员姓名
  char password[20];   // 职员密码
  enum user_sex sex;   // 职员性别
  enum user_type type; // 职员类型
  int age;             // 职员年龄
  char department[20]; // 职员部门
  char phone[20];      // 职员电话
};

#endif // !__UTIL_H__
