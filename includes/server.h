#ifndef __SERVER_H__
#define __SERVER_H__

#include "util.h"
#include "message.h"
#include "dbhandler.h"
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

#endif
