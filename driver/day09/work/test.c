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
  int number;
	int fd = open("/dev/led", O_RDWR);
	if (fd < 0) {
		printf("打开设备文件失败\n");
		exit(-1);
	}
	while (1) {
		read(fd, &number, sizeof(number)); //读取数据
		printf("read:%d\n", number);
	}

	close(fd);
	return 0;
}
