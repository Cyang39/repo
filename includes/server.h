#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_CLIENT 100
#define PORT 8888

#define ERR_MSG(msg)                                                           \
  do {                                                                         \
    fprintf(stderr, "line: %d\n", __LINE__);                                   \
    perror(msg);                                                               \
  } while (0)
