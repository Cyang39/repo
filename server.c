#include "includes/server.h"

int main() {
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
  char buf[1024] = {0};

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
          int n = read(events[i].data.fd, buf, sizeof(buf));
          if (n < 0) {
            ERR_MSG("read");
            return -1;
          }
          if (n == 0) {
            printf("client closed\n");
            close(events[i].data.fd);
          } else {
            printf("recv: %s\n", buf);
            write(events[i].data.fd, buf, n);
          }
        }
      }
    }
  }
  return 0;
}
