#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include "head.h"

int main(int argc, char const *argv[])
{
	int a, b;
	while (1) {
		//从终端读取
		printf("请选择要控制的灯\n");
		printf("0(LED1) 1(LED2) 2(LED3) >");

		scanf("%d", &a);
		printf("请选择要控制的状态\n");
		printf("0(ON) 1(OFF) >");
		scanf("%d", &b);
		//写入设备文件
		if (a == 0) {
			int led0_fd = open("/dev/myled0", O_RDWR);
			if (led0_fd < 0) {
				printf("打开/dev/myled0文件失败\n");
				exit(-1);
			}
			if (b == 0) {
				ioctl(led0_fd, LED_ON);

			} else {
				ioctl(led0_fd, LED_OFF);
			}
			close(led0_fd);
		} else if (a == 1) {
			int led1_fd = open("/dev/myled1", O_RDWR);
			if (led1_fd < 0) {
				printf("打开/dev/myled1文件失败\n");
				exit(-1);
			}
			if (b == 0) {
				ioctl(led1_fd, LED_ON);
			} else {
				ioctl(led1_fd, LED_OFF);
			}
			close(led1_fd);
		} else if (a == 2) {
			int led2_fd = open("/dev/myled2", O_RDWR);
			if (led2_fd < 0) {
				printf("打开/dev/myled2文件失败\n");
				exit(-1);
			}
			if (b == 0) {
				ioctl(led2_fd, LED_ON);
			} else {
				ioctl(led2_fd, LED_OFF);
			}
			close(led2_fd);
		}
	}

	return 0;
}
