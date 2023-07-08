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
/* According to earlier standards */
#include <sys/time.h>

int main(int argc, char const *argv[])
{
	int fd1, fd2, epfd;
	struct epoll_event event;
	struct epoll_event events[10]; //存放就绪事件描述符的数组
	char buf[128] = { 0 };
	//创建epoll句柄
	epfd = epoll_create(1);
	if (epfd < 0) {
		printf("epoll_create filed\n");
		exit(-1);
	}
	//打开设备文件
	fd1 = open("/dev/input/mouse0", O_RDWR);
	if (fd1 < 0) {
		printf("打开鼠标设备文件失败\n");
		exit(-1);
	}
	fd2 = open("/dev/myled0", O_RDWR);
	if (fd2 < 0) {
		printf("打开自定义设备文件失败\n");
		exit(-1);
	}
	//添加准备就绪事件进入epoll;
	event.events = EPOLLIN; //读事件
	event.data.fd = fd1;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd1, &event) < 0) {
		printf("epoll_ctl add filed\n");
	}
	event.events = EPOLLIN; //读事件
	event.data.fd = fd2;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd2, &event) < 0) {
		printf("epoll_ctl add filed\n");
	}

	//监听事件是否发生
	while (1) {
		//如果成功，ret接收返回的事件个数，把就绪的事件放在events数组中
		int ret = epoll_wait(epfd, events, 10, -1);
		if (ret < 0) {
			printf("epoll_wait filed\n");
			exit(-1);
		}
		int i;
		//循环遍历数组，做事件的处理
		for (i = 0; i < ret; i++) {
			if (events[i].events & EPOLLIN) {
				read(events[i].data.fd, buf, sizeof(buf));
				printf("buf:%s\n", buf);
			}
		}
	}
	close(fd1);
	close(fd2);
	return 0;
}
