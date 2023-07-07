/**
 * 进程1: 以阻塞的形式读取设备文件
 */
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>

int main(int argc, char const *argv[])
{
	char buf[128] = { 0 };
	int fd = open("/dev/myled0", O_RDWR);
	if (fd < 0) {
		printf("打开设备文件失败\n");
		exit(-1);
	}
	while (1) {
		memset(buf, 0, sizeof(buf)); //清空
		read(fd, buf, sizeof(buf)); //读取数据
		printf("buf:%s\n", buf);
	}

	close(fd);
	return 0;
}
