#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
	char buf[128] = { 0 };
	int fd = open("/dev/mycdev", O_RDWR);
	if (fd < 0) {
		printf("打开设备文件失败\n");
		exit(-1);
	}
	read(fd, buf, sizeof(buf));
	write(fd, buf, sizeof(buf));
	close(fd);
	return 0;
}
