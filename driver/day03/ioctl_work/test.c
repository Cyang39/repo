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
	char buf[128] = { 0 };
	int led_fd = open("/dev/led0", O_RDWR);
	if (led_fd < 0) {
		printf("打开设备文件失败\n");
		exit(-1);
	}
	int fan_fd = open("/dev/fan", O_RDWR);
  if (fan_fd < 0) {
    printf("打开设备文件失败\n");
    exit(-1);
  }
	int bee_fd = open("/dev/bee", O_RDWR);
  if (bee_fd < 0) {
    printf("打开设备文件失败\n");
    exit(-1);
  }
	int motor_fd = open("/dev/motor", O_RDWR);
  if (motor_fd < 0) {
    printf("打开设备文件失败\n");
    exit(-1);
  }
	while (1) {
		//从终端读取
		printf("请选择要控制的设备\n");
		printf("0(灯) 1(风扇) 2(蜂鸣器) 3(马达) >");

		scanf("%d", &a);
		if (a == 0) {
			printf("请输入要实现的功能\n");
			printf("0(关灯) 1(开灯) >");
			scanf("%d", &a);

			printf("请输入要控制的灯\n");
			printf("1(LED1) 2(LED2) 3(LED3)>");
			scanf("%d", &b);
			if (a == 1) {
				ioctl(led_fd, LED_ON, b);
			} else if (a == 0) {
				ioctl(led_fd, LED_OFF, b);
			}
		} else if (a == 1) {
			printf("请输入要实现的功能\n");
			printf("0(关风扇) 1(开风扇) >");
			scanf("%d", &a);
			if (a == 1) {
				ioctl(fan_fd, FAN_ON);
			} else if (a == 0) {
				ioctl(fan_fd, FAN_OFF);
			}
		} else if (a == 2) {
      printf("请输入要实现的功能\n");
      printf("0(关蜂鸣器) 1(开蜂鸣器) >");
      scanf("%d", &a);
      if (a == 1) {
        ioctl(bee_fd, BEE_ON);
      } else if (a == 0) {
        ioctl(bee_fd, BEE_OFF);
      }
    } else if (a == 3) {
      printf("请输入要实现的功能\n");
      printf("0(关马达) 1(开马达) >");
      scanf("%d", &a);
      if (a == 1) {
        ioctl(motor_fd, MOTOR_ON);
      } else if (a == 0) {
        ioctl(motor_fd, MOTOR_OFF);
      }
    }
	}
	close(led_fd);
	close(fan_fd);
	close(bee_fd);
  close(motor_fd);

	return 0;
}
