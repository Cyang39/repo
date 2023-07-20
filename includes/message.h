#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include "util.h"

// 消息结构体
struct message {
  char name[20];        // 发送方姓名
  enum cmd_type ctype;  // 命令类型
  char buf[1024];       // 消息内容
  struct info st;       // 员工信息
};

// 打印职员信息
void print_info(struct info *user);
// 生成登录消息结构体
void gen_login_msg(struct message *msg, char *username, char *password);
// 从消息结构体中获取用户名
void get_username(struct message *msg, char *username);
// 从消息结构体中获取密码
void get_password(struct message *msg, char *password);
// 生成查询消息结构体
void gen_query_by_name_req_msg(struct message *msg, char *target_name);
// 生成查询结果消息结构体
void gen_query_by_name_res_msg(struct message *msg, struct info *user);

#endif // !__MESSAGE_H__
