#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "head.h"

int main(int argc, char const *argv[])
{
	int a, fd[3];
	char str[15];
	for (a = 0; a < 3; a++) {
		sprintf(str, "/dev/mychrdev%d", a);
		fd[a] = open("/dev/mychrdev0", O_RDWR);
		if (fd[a] < 0) {
			printf("打开设备文件失败\n");
			exit(-1);
		}
	}
	while (1) {
		printf("请选择要开关的灯\n");
		printf("0(LED1) 1(LED2) 2(LED3)>");

		scanf("%d", &a);
		ioctl(fd[a], LED_TOGGLE, a);
	}
	for (a = 0; a < 3; a++)
		close(fd[a]);

	return 0;
}
