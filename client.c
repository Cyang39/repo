#include "client.h"
#include <stdio.h>

struct login_form {
  char username[20];
  char password[20];
};

void display_login_menu(struct login_form *form) {
  printf("请输入用户名>>> ");
  scanf("%s", form->username);
  printf("请输入密码>>> ");
  scanf("%s", form->password);
}

void display_main_menu_for_user(int *choice) {
  printf("1.查询\n");
  printf("2.修改\n");
  printf("3.删除\n");
  printf("请输入>>> ");
  scanf("%d", choice);
}

void display_main_menu_for_admin(int *choice) {}

int main() {
  int sfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sfd < 0) {
    ERR_MSG("socket");
    return -1;
  }
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(SERVER_PORT);
  addr.sin_addr.s_addr = inet_addr(SERVER_IP);

  int ret = connect(sfd, (struct sockaddr *)&addr, sizeof(addr));
  if (ret < 0) {
    ERR_MSG("connect");
    return -1;
  }

  char buf[sizeof(struct message)];
  ssize_t read_size = 0;
  while (1) {
    struct login_form form;
    struct message msg;
    display_login_menu(&form);
    gen_login_msg(&msg, form.username, form.password);
    memcpy(buf, &msg, sizeof(msg));
    // 发送登录信息
    if (send(sfd, buf, strlen(buf), 0) < 0) {
      ERR_MSG("send");
      return -1;
    }
    // 如果登录成功
    while (1) {
      int choice;
      display_main_menu_for_user(&choice);
      switch (choice) {
      case 1:
        msg.ctype = MSG_QUERY;
        break;
      case 2:
        msg.ctype = MSG_UPDATE;
        break;
      case 3:
        msg.ctype = MSG_DELETE;
        break;
      }
      if (send(sfd, &msg, sizeof(msg), 0) < 0) {
        ERR_MSG("send");
        return -1;
      }
      bzero(&msg, sizeof(msg));
      if ((read_size = recv(sfd, &msg, sizeof(msg), 0)) < 0) {
        ERR_MSG("recv");
        return -1;
      } else if (read_size == 0) {
        printf("server closed\n");
        break;
      } else {
        printf("recv: %s\n", (char *)&msg);
      }
    }
  }
  close(sfd);
  return 0;
}
