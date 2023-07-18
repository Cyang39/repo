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
  struct sockaddr_in client_addr;
  socklen_t len = sizeof(client_addr);
  int cfd = accept(sfd, (struct sockaddr *)&client_addr, &len);
  if (cfd < 0) {
    ERR_MSG("accept");
    return -1;
  }
  char buf[1024] = {0};
  while (1) {
    // 接收数据
    int ret = recv(cfd, buf, sizeof(buf), 0);
    if (ret < 0) {
      ERR_MSG("recv");
      return -1;
    } else if (ret == 0) {
      printf("client close\n");
      break;
    }
    printf("client: %s\n", buf);
    // 发送数据
    memset(buf, 0x00, sizeof(buf));
    scanf("%s", buf);
    ret = send(cfd, buf, strlen(buf), 0);
    if (ret < 0) {
      ERR_MSG("send");
      return -1;
    }
  }
  return 0;
}
