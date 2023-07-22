#ifndef __SERVER_H__
#define __SERVER_H__

#include "dbhandler.h"
#include "message.h"
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

typedef struct online {
  int fd;
  char name[20];
  struct online *next;
} online_t;

sqlite3 *db;            // 数据库
online_t *online_store; // 在线用户链表

int check_online(char *name);
int store_online(int fd, char *name);
int delete_online(int fd);
char *get_name(int fd, char *name); // 遍历，获取 fd 对应的用户名

#endif
