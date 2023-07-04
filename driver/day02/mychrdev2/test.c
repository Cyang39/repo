#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
int main(int argc, char const *argv[])
{
	char buf[128] = { 0 };
	int fd = open("/dev/mychrdev", O_RDWR);
	if (fd < 0) {
		printf("打开设备文件失败\n");
		exit(-1);
	}
	fgets(buf, sizeof(buf), stdin); //在终端读取
	buf[strlen(buf) - 1] = '\0'; //吃掉末尾的‘\n’;
	write(fd, buf, sizeof(buf));
	memset(buf, 0, sizeof(buf)); //清空buf
	read(fd, buf, sizeof(buf));
	printf("buf:%s\n", buf);
	close(fd);
	return 0;
}
