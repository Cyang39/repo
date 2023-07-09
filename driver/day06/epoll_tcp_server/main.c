#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>

#define MAX_EVENTS 10
#define BUFFER_SIZE 1024

int main() {
  int listen_fd, conn_fd, epoll_fd, num_events;
  struct epoll_event events[MAX_EVENTS];
  struct sockaddr_in server_addr, client_addr;
  socklen_t client_addr_len;
  char buffer[BUFFER_SIZE];

  // 创建 TCP 套接字
  if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  // 设置服务器地址
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(8080);
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  // 将套接字绑定到服务器地址
  if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) ==
      -1) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  // 监听套接字，等待连接
  if (listen(listen_fd, SOMAXCONN) == -1) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  // 创建 epoll 实例
  if ((epoll_fd = epoll_create1(0)) == -1) {
    perror("epoll_create1");
    exit(EXIT_FAILURE);
  }

  struct epoll_event event;
  event.events = EPOLLIN;
  event.data.fd = listen_fd;

  // 将监听套接字加入 epoll 监听
  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &event) == -1) {
    perror("epoll_ctl");
    exit(EXIT_FAILURE);
  }

  while (1) {
    // 等待事件发生
    if ((num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1)) == -1) {
      perror("epoll_wait");
      exit(EXIT_FAILURE);
    }

    for (int i = 0; i < num_events; i++) {
      if (events[i].data.fd == listen_fd) {
        // 接受新连接
        client_addr_len = sizeof(client_addr);
        conn_fd = accept(listen_fd, (struct sockaddr *)&client_addr,
                         &client_addr_len);

        // 将新连接的套接字加入 epoll 监听
        event.events = EPOLLIN;
        event.data.fd = conn_fd;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, conn_fd, &event) == -1) {
          perror("epoll_ctl");
          exit(EXIT_FAILURE);
        }

        printf("Accepted new connection from %s:%d\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
      } else {
        // 处理已连接套接字的数据
        conn_fd = events[i].data.fd;
        int bytes_recvd = recv(conn_fd, buffer, BUFFER_SIZE, 0);
        if (bytes_recvd <= 0) {
          // 连接已关闭或发生错误
          if (bytes_recvd == 0) {
            printf("Connection closed by client\n");
          } else {
            perror("recv");
          }

          // 关闭连接并从 epoll 监听中移除
          close(conn_fd);
          epoll_ctl(epoll_fd, EPOLL_CTL_DEL, conn_fd, NULL);
        } else {
          // 处理接收到的数据
          printf("Received data from client: %.*s\n", bytes_recvd, buffer);

          // 回显数据给客户端
          send(conn_fd, buffer, bytes_recvd, 0);
        }
      }
    }
  }

  // 关闭监听套接字和 epoll 实例
  close(listen_fd);
  close(epoll_fd);

  return 0;
}
