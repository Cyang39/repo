#include "message.h"
#include <ctype.h>

void trim(char *str) {
  int start = 0;
  int end = strlen(str) - 1;
  // 寻找字符串开头的空白字符
  while (isspace(str[start])) {
    start++;
  }
  // 寻找字符串末尾的空白字符
  while (end >= start && isspace(str[end])) {
    end--;
  }
  // 移动字符串内容，去除首尾空白字符
  int i, j;
  for (i = start, j = 0; i <= end; i++, j++) {
    str[j] = str[i];
  }
  str[j] = '\0'; // 添加字符串结束符
}

struct username_pair {
  char username[20];
  char password[20];
};

// 打印用户信息
void print_info(struct info *user) {
  printf("Name:\t%s\n", user->name);
  printf("Age:\t\t%d\n", user->age);
  printf("Dept:\t\t%s\n", user->department);
  printf("Sex:\t\t%s\n", user->sex == FEMALE ? "female" : "male");
  printf("Phone:\t\t%s\n", user->phone);
}

// 构建一个“登录”消息结构体
void gen_login_msg(struct message *msg, char *username, char *password) {
  memset(msg, 0, sizeof(struct message));
  msg->ctype = MSG_LOGIN;
  struct username_pair pair;
  strcpy(pair.username, username);
  strcpy(pair.password, password);
  memcpy(msg->buf, &pair, sizeof(struct username_pair));
}

// 从消息结构体中获取密码
void get_password(struct message *msg, char *password) {
  struct username_pair pair;
  memcpy(&pair, msg->buf, sizeof(struct username_pair));
  strcpy(password, pair.password);
}

// 从消息结构体获取用户名
void get_username(struct message *msg, char *username) {
  struct username_pair pair;
  memcpy(&pair, msg->buf, sizeof(struct username_pair));
  strcpy(username, pair.username);
}

// 构建一个“通过用户名称查询”消息结构体
void gen_query_by_name_req_msg(struct message *msg, char *name) {
  memset(msg, 0, sizeof(struct message));
  msg->ctype = MSG_QUERY; // 消息类型为查询
  sprintf(msg->buf, "%s", name); // 消息内容为用户名
}

// 回复“通过用户名称查询”消息结构体
void gen_query_by_name_res_msg(struct message *msg, struct info *user) {
  memset(msg, 0, sizeof(struct message));
  msg->ctype = MSG_QUERY_RES;
  memcpy(&msg->st, user, sizeof(struct info));
}
