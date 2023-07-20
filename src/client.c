#include "client.h"

char my_name[20] = {0};

struct login_form {
  char username[20];
  char password[20];
};

void tui_welcome() {
  printf("=================================\n");
  printf("=\t欢迎使用员工管理系统\t=\n");
  printf("=\t\t请登录\t\t=\n");
  printf("=================================\n");
}

void tui_login_repl(struct login_form *form) {
  printf("请输入用户名>>> ");
  scanf("%s", form->username);
  printf("请输入密码>>> ");
  scanf("%s", form->password);
}

void tui_main_menu(int *choice) {
  while (1) {
    printf("1. 查询\n");
    printf("2. 修改\n");
    printf("3. 删除\n");
    printf("4. 添加\n");
    printf("5. 日志\n");
    printf("请输入>>> ");
    if (scanf("%d", choice) != 1 || *choice < 1 || *choice > 5) {
      printf("输入错误，请重新输入\n");
      while (getchar() != '\n')
        ;
      continue;
    } else {
      break;
    }
  }
  printf("=================================\n");
}

int on_login(int sfd, struct message *msg) {
  int read_size;
  while (1) { // loop for login
    struct login_form form;
    tui_login_repl(&form);
    gen_login_msg(msg, form.username, form.password);
    // 发送登录信息
    if (send(sfd, msg, sizeof(struct message), 0) < 0) {
      ERR_MSG("send");
      return -1;
    }
    // 接收登录结果
    bzero(msg, sizeof(struct message));
    if ((read_size = recv(sfd, msg, sizeof(struct message), 0)) < 0) {
      ERR_MSG("recv");
      return -1;
    } else if (read_size == 0) {
      printf("server closed\n");
      break;
    } else {
      if (msg->ctype == MSG_OK) {
        printf("登录成功\n");
        printf("=================================\n");
        strcpy(my_name, form.username);
        break;
      } else { // MSG_ERROR
        printf("%s\n", msg->buf);
      }
    }
  }
  return 0;
}

int main() {
  int ret;
  int sfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sfd < 0) {
    ERR_MSG("socket");
    return -1;
  }
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(SERVER_PORT);
  addr.sin_addr.s_addr = inet_addr(SERVER_IP);

  if (connect(sfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    ERR_MSG("connect");
    return -1;
  }

  ssize_t read_size = 0;
  tui_welcome(); // 成功连接服务器后，显示欢迎界面
  while (1) {
    struct message msg;
    if (on_login(sfd, &msg) < 0) {
      return -1;
    }
    bzero(&msg, sizeof(msg));
    // 如果登录成功
    int choice;
    while (1) {
      choice = 0;
      char name[20] = {0};
      char temp[20] = {0};
      tui_main_menu(&choice);
      switch (choice) {
      case 1:
        printf("请输入要查询的用户名>>> ");
        scanf("%s", name);
        gen_query_by_name_req_msg(&msg, name);
        break;
      case 2:
        msg.ctype = MSG_UPDATE;
        printf("请指定要修改的用户名>>> ");
        scanf("%s", msg.buf);
        printf("请输入要修改的用户名称>>> ");
        scanf("%s", msg.st.name);
        printf("请输入要修改的用户年龄>>> ");
        scanf("%s", (char *)&temp);
        msg.st.age = atoi(temp);
        printf("请输入要修改的用户性别(1:Female/2:Male)>>> ");
        scanf("%s", (char *)&temp);
        msg.st.sex = atoi(temp);
        printf("请输入要修改的用户电话>>> ");
        scanf("%s", msg.st.phone);
        printf("请输入要修改的用户部门>>> ");
        scanf("%s", msg.st.department);
        printf("请输入要修改的用户密码>>> ");
        scanf("%s", msg.st.password);
        printf("请输入要修改的用户权限(1:User/2:Admin)>>> ");
        scanf("%s", (char *)&temp);
        msg.st.type = atoi(temp);
        break;
      case 3:
        msg.ctype = MSG_DELETE;
        printf("请输入要删除的用户名>>> ");
        memset(msg.buf, 0, sizeof(msg.buf));
        scanf("%s", msg.buf);
        break;
      case 4:
        msg.ctype = MSG_INSERT;
        printf("请输入要添加的用户名>>> ");
        scanf("%s", msg.st.name);
        printf("请输入要添加的用户年龄>>> ");
        scanf("%s", (char *)&temp);
        msg.st.age = atoi(temp);
        printf("请输入要添加的用户性别(1:Female/2:Male)>>> ");
        scanf("%s", (char *)&temp);
        msg.st.sex = atoi(temp);
        printf("请输入要添加的用户电话>>> ");
        scanf("%s", msg.st.phone);
        printf("请输入要添加的用户部门>>> ");
        scanf("%s", msg.st.department);
        printf("请输入要添加的用户密码>>> ");
        scanf("%s", msg.st.password);
        printf("请输入要添加的用户权限(1:User/2:Admin)>>> ");
        scanf("%s", (char *)&temp);
        msg.st.type = atoi(temp);
        break;
      case 5:
        msg.ctype = MSG_LOG;
        break;
      }
      strcpy(msg.name, my_name);
      if (send(sfd, &msg, sizeof(msg), 0) < 0) {
        ERR_MSG("send");
        return -1;
      }
      bzero(&msg, sizeof(msg));
      // 接收部分====================================================
      if ((read_size = recv(sfd, &msg, sizeof(msg), 0)) < 0) {
        ERR_MSG("recv");
        return -1;
      } else if (read_size == 0) {
        printf("server closed\n");
        break; // choice loop
      } else {
        switch (msg.ctype) {
        case MSG_OK: // 成功回复（删除，修改，添加）
          printf("ok: %s\n", msg.buf);
          break;
        case MSG_ERROR: // 失败回复（删除，修改，添加）
          printf("error: %s\n", msg.buf);
          break;            // switch res type
        case MSG_QUERY_RES: // 查询回复 (st 有数据)
          printf("您查询的用户信息如下：\n");
          printf("=================================\n");
          print_info(&msg.st);
          break; // switch res type
        case MSG_LOG_RES:
          printf("您查询的日志信息如下：\n");
          printf("=================================\n");
          printf("%s\n", msg.buf);
        }
        printf("=================================\n");
      }
    }
  }
  close(sfd);
  return 0;
}
