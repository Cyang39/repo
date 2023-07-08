#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/time.h>

int main(int argc, char const *argv[])
{
	char buf[128] = { 0 };
	// 1.打开文件描述符
	int fd1, fd2, ret;
	fd1 = open("/dev/myled0", O_RDWR);
	if (fd1 < 0) {
		printf("打开设备文件失败\n");
		exit(-1);
	}
	fd2 = open("/dev/input/mouse0", O_RDWR);
	if (fd2 < 0) {
		printf("打开鼠标设备文件失败\n");
		exit(-1);
	}
	// 创建读事件集合
	fd_set readfds;

	while (1) {
		// 清空事件集合
		FD_ZERO(&readfds);
		// 将要监听的文件描述符添加到集合
		FD_SET(fd1, &readfds);
		FD_SET(fd2, &readfds);
		// 调用select监听
		ret = select(fd2 + 1, &readfds, NULL, NULL, NULL);
		if (ret < 0) {
			printf("select调用失败\n");
			return ret;
		}
		if (FD_ISSET(fd1, &readfds)) {
			memset(buf, 0, sizeof(buf));
			read(fd1, buf, sizeof(buf));
			printf("自定义事件发生  buf:%s\n", buf);
		}
		if (FD_ISSET(fd2, &readfds)) {
			memset(buf, 0, sizeof(buf));
			read(fd2, buf, sizeof(buf));
			printf("鼠标事件发生  buf:%s\n", buf);
		}
	}
	close(fd1);
	close(fd2);
	return 0;
}
