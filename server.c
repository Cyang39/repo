#include "server.h"

#define ADMIN_NAME "root"
#define ADMIN_PASSWD "1234"

sqlite3 *db; // 数据库

void on_query_request(struct message *msg) {
#ifdef DEBUG
  // buf 为要查找的员工 name
  char dbug_msg[2048];
  sprintf(dbug_msg, "query>username: '%s'", msg->buf);
  DEBUG_MSG(dbug_msg);
#endif
  if (check_user_type(db, msg->name) == USER_NORMAL &&
      strcmp(msg->buf, msg->name) != 0) {
    msg->ctype = MSG_ERROR;
    strcpy(msg->buf, "普通用户只能查询自己的信息");
    return;
  }
  struct info user;
  int ret = query_info_db_by_username(db, msg->buf, &user);
  if (ret < 0) {
    msg->ctype = MSG_ERROR;
    strcpy(msg->buf, "该员工不存在");
  } else {
    gen_query_by_name_res_msg(msg, &user);
  }
}

// 处理添加员工请求
void on_insert_requset(struct message *msg) {
  int ret;
  if (check_user_type(db, msg->name) != USER_ADMIN) {
    msg->ctype = MSG_ERROR;
    strcpy(msg->buf, "权限不足");
    return;
  }
  // 注意：此时客户端传来的 msg.st 是一个完整的员工信息
  ret = insert_db(db, &msg->st);
  if (ret < 0) {
    msg->ctype = MSG_ERROR;
    strcpy(msg->buf, "该员工已存在");
  } else {
    msg->ctype = MSG_OK;
    strcpy(msg->buf, "添加成功");
  }
}

// 处理删除员工请求
void on_delete_request(struct message *msg) {
  if (check_user_type(db, msg->name) != USER_ADMIN) {
    msg->ctype = MSG_ERROR;
    strcpy(msg->buf, "权限不足");
    return;
  }
  // 注意：此时 msg->buf 中存放的是要删除的员工的用户名
  int ret = delete_db_by_username(db, msg->buf);
  if (ret < 0) {
    msg->ctype = MSG_ERROR;
    strcpy(msg->buf, "该员工不存在");
  } else {
    msg->ctype = MSG_OK;
    strcpy(msg->buf, "删除成功");
  }
}

// 更新员工信息
void on_update_request(struct message *msg) {
  // 注意：此时 msg->st 中存放的是要更新的员工的完整信息
  if (check_user_type(db, msg->name) == USER_NORMAL &&
      strcmp(msg->st.name, msg->name) != 0) {
    msg->ctype = MSG_ERROR;
    strcpy(msg->buf, "普通用户只能修改自己的信息");
    return;
  }
  if (check_user_type(db, msg->name) == USER_NORMAL) {
    // 普通用户不能修改用户类型
    msg->st.type = USER_NORMAL;
  }
  int ret = update_db_by_username(db, msg->st.name, &msg->st);
  if (ret < 0) {
    msg->ctype = MSG_ERROR;
    strcpy(msg->buf, "该员工不存在");
  } else {
    msg->ctype = MSG_OK;
    strcpy(msg->buf, "修改成功");
  }
}

int main(int argc, const char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s file.db\n", argv[0]);
    return -1;
  }
  // 打开数据库
  if (sqlite3_open(argv[1], &db) != SQLITE_OK) {
    printf("error on sqlite_open(): %s\n", sqlite3_errmsg(db));
    exit(-1);
  }
  // 初始化数据库
  init_db(db);
  // 创建套接字
  int sfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sfd < 0) {
    ERR_MSG("socket");
    return -1;
  }
  // 允许端口复用
  int opt = 1;
  if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    ERR_MSG("setsockopt");
    return -1;
  }
  // 绑定地址
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr.s_addr = INADDR_ANY;
  if (bind(sfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    ERR_MSG("bind");
    return -1;
  }
  // 监听
  if (listen(sfd, 10) < 0) {
    ERR_MSG("listen");
    return -1;
  }

  // 接收连接
  struct sockaddr_in client_store[MAX_CLIENT];
  struct sockaddr_in client_addr;
  socklen_t len = sizeof(client_addr);
  int cfd;

  struct epoll_event event;
  struct epoll_event events[MAX_CLIENT];
  char buf[sizeof(struct message)] = {0};

  int epfd = epoll_create(MAX_CLIENT);
  if (epfd < 0) {
    ERR_MSG("epoll_create");
    return -1;
  }
  event.events = EPOLLIN;
  event.data.fd = sfd;
  if (epoll_ctl(epfd, EPOLL_CTL_ADD, sfd, &event) < 0) {
    ERR_MSG("epoll_ctl");
    return -1;
  }

  int ret; // 处理错误用的返回值临时保存变量
  while (1) {
    int n = epoll_wait(epfd, events, MAX_CLIENT, -1);
    if (n < 0) {
      ERR_MSG("epoll_wait");
      return -1;
    }
    for (int i = 0; i < n; i++) {
      if (events[i].events & EPOLLIN) {
        // 新连接
        if (events[i].data.fd == sfd) {
          cfd = accept(sfd, (struct sockaddr *)&client_addr, &len);
          if (cfd < 0) {
            ERR_MSG("accept");
            return -1;
          }
          printf("client %s:%d connected\n", inet_ntoa(client_addr.sin_addr),
                 ntohs(client_addr.sin_port));
          // 添加到epoll
          event.events = EPOLLIN;
          event.data.fd = cfd;
          if (epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &event) < 0) {
            ERR_MSG("epoll_ctl");
            return -1;
          }
        } else {
          // 读取数据
          memset(buf, 0, sizeof(buf));
          int n = read(events[i].data.fd, buf, sizeof(buf));
          if (n < 0) {
            ERR_MSG("read");
            return -1;
          }
          if (n == 0) {
            printf("client closed\n");
            close(events[i].data.fd);
          } else {
            struct message msg; // 接受和发送使用同一个结构体
            char username[20];
            char password[20];
            memcpy(&msg, &buf, sizeof(msg));
            switch (msg.ctype) {
            case MSG_LOGIN:
              printf("user request login\n");
              get_username(&msg, (char *)&username);
              get_password(&msg, (char *)&password);
              if (strcmp(username, ADMIN_NAME) == 0) {
                if (strcmp(password, ADMIN_PASSWD) == 0) {
                  printf("admin login success\n");
                  msg.ctype = MSG_OK;
                  strcpy(msg.buf, "登录成功");
                } else {
                  printf("admin login failed\n");
                  msg.ctype = MSG_ERROR;
                  strcpy(msg.buf, "用户名或密码错误");
                }
              } else {
#ifdef DEBUG
                char dbug_msg[100];
                sprintf(dbug_msg, "login>username: '%s'", username);
                DEBUG_MSG(dbug_msg);
                sprintf(dbug_msg, "login>password: '%s'", password);
                DEBUG_MSG(dbug_msg);
#endif
                ret = check_db_by_username_and_password(db, (char *)&username,
                                                        (char *)&password);
                if (ret < 0) {
                  msg.ctype = MSG_ERROR;
                  strcpy(msg.buf, "用户名或密码错误");
                } else {
                  msg.ctype = MSG_OK;
                  strcpy(msg.buf, "登录成功");
                }
              }
              break;
            case MSG_QUERY: // 查询员工信息
              on_query_request(&msg);
              break;
            case MSG_INSERT: // 添加员工
              on_insert_requset(&msg);
              break;
            case MSG_DELETE: // 删除员工
              on_delete_request(&msg);
              break;
            case MSG_UPDATE: // 更新员工信息
              on_update_request(&msg);
              break;
            }
            write(events[i].data.fd, &msg, n);
          }
        }
      }
    }
  }
  return 0;
}
