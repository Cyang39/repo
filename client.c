#include "includes/client.h"

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
    printf("请输入>>> ");
    fgets(buf, sizeof(buf), stdin);
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
  close(sfd);
  return 0;
}
