#include "message.h"

// 构建一个“登录”消息结构体
void gen_login_msg(struct message *msg, char *username, char *password) {
  memset(msg, 0, sizeof(struct message));
  msg->ctype = MSG_LOGIN;
  sprintf(msg->buf, "%-20s%-20s", username, password);
}
