arch?=arm
modname?=demo
#指定内核顶层目录的路径
ifeq ($(arch),arm)
  KERNELDIR:=/home/cyang/FSMP1A/linux-stm32mp-5.10.61-stm32mp-r2-r0/linux-5.10.61
else
  KERNELDIR:=/lib/modules/$(shell uname -r)/build
endif

PWD:=$(shell pwd)  #将shell命令pwd的执行结果赋值给变量PWD
all:
	make -C $(KERNELDIR) M=$(PWD) modules
clean:
#编译清除
	make -C $(KERNELDIR) M=$(PWD) clean
	rm a.out
#将指定的.o文件独立链接为模块文件
obj-m:=$(modname).o
