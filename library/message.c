#include "message.h"

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
  sprintf(msg->buf, "%-20s%-20s", username, password);
}

// 构建一个“通过用户名称查询”消息结构体
void gen_query_by_name_req_msg(struct message *msg, char *name) {
  memset(msg, 0, sizeof(struct message));
  msg->ctype = MSG_QUERY;
  sprintf(msg->buf, "%-20s%-20s", "name", name);
}

// 回复“通过用户名称查询”消息结构体
void gen_query_by_name_res_msg(struct message *msg, struct info *user) {
  memset(msg, 0, sizeof(struct message));
  msg->ctype = MSG_QUERY_RES;
  memcpy(&msg->st, user, sizeof(struct info));
}
