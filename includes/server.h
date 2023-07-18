#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#define PORT 8888

#define ERR_MSG(msg)                                                           \
  do {                                                                         \
    fprintf(stderr, "line: %d\n", __LINE__);                                   \
    perror(msg);                                                               \
  } while (0)
