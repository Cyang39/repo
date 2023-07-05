#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include "head.h"

int main(int argc, char const *argv[])
{
	int a;
	char buf[128] = { 0 };
	int fd = open("/dev/mychrdev0", O_RDWR);
	if (fd < 0) {
		printf("打开设备文件失败\n");
		exit(-1);
	}
	while (1) {
		//从终端读取
		printf("请选择要实现的功能\n");
		printf("0(关灯) 1(开灯)>");
		scanf("%d", &a);
		if (a == 1) {
			ioctl(fd, LED_ON);
		} else if (a == 0) //关灯
		{
			ioctl(fd, LED_OFF);
		}
	}

	close(fd);

	return 0;
}
