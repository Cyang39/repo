#include "client.h"

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
  printf("1.注册\n");
  printf("2.登录\n");
  printf("3.退出\n");
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

  char buf[1024];
  ssize_t read_size = 0;
  while (1) {
    struct login_form form;
    display_login_menu(&form);
    sprintf(buf, "%s %s\n", form.username, form.password);
    // 如果登录成功
    while (1) {
      int choice;
      display_main_menu_for_user(&choice);
      sprintf(buf, "%d\n", choice);
      buf[strlen(buf) - 1] = '\0';
      if (send(sfd, buf, strlen(buf), 0) < 0) {
        ERR_MSG("send");
        return -1;
      }
      bzero(buf, sizeof(buf));
      if ((read_size = recv(sfd, buf, sizeof(buf), 0)) < 0) {
        ERR_MSG("recv");
        return -1;
      } else if (read_size == 0) {
        printf("server closed\n");
        break;
      }
    }
  }
  close(sfd);
  return 0;
}
