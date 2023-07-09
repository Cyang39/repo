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
	char buf[128] = "hello world";
	int fd = open("/dev/my_led0", O_RDWR);
	if (fd < 0) {
		printf("打开设备文件失败\n");
		exit(-1);
	}
	write(fd, buf, sizeof(buf)); //写数据
	close(fd);

	return 0;
}
