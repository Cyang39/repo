#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/epoll.h>
#include <signal.h>
/* According to earlier standards */
#include <sys/time.h>

int fd;
char buf[128] = { 0 };
void fasync_handler(int signo)
{
	memset(buf, 0, sizeof(buf));
	read(fd, buf, sizeof(buf));
	printf("buf:%s\n", buf);
}
int main(int argc, char const *argv[])
{
	fd = open("/dev/my_led0", O_RDWR);
	if (fd < 0) {
		printf("open err\n");
		exit(-1);
	}
	//信号处理函数的注册
	if (signal(SIGIO, fasync_handler) == SIG_ERR) {
		printf("信号处理函数注册失败\n");
		exit(-1);
	}
	//调用底层fasync函数
	unsigned int flags = fcntl(fd, F_GETFL); //获取文件描述符属性
	fcntl(fd, F_SETFL, flags | FASYNC);
	//告诉内核只能给当前进程发送信号
	fcntl(fd, F_SETOWN, getpid());
	while (1) {
		sleep(1);
	}
	return 0;
}
