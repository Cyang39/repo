#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include "head.h"
int main(int argc, char const *argv[])
{
  int tem, hum;
  float tem1, hum1;
  int fd = open("/dev/si7006", O_RDWR);
  if (fd < 0) {
    printf("设备文件打开失败\n");
    exit(-1);
  }
  while (1) {
    //获取数据
    ioctl(fd, GET_HUM, &hum);
    ioctl(fd, GET_TEM, &tem);
    //大小端转换
    hum = ntohs(hum);
    tem = ntohs(tem);
    //计算数据
    hum1 = 125.0 * hum / 65536 - 6;
    tem1 = 175.72 * tem / 65536 - 46.85;
    printf("tem=%f,hum=%f\n", tem1, hum1);
    sleep(1);
  }
  return 0;
}
