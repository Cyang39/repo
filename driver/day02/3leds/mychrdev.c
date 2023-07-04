#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include "gpio.h"
#include "rcc.h"
#include "head.h"

int major; // 用于保存主设备号
char kbuf[128] = { 0 };
unsigned int *vir_moder;
unsigned int *vir_odr;
unsigned int *vir_rcc;

// 封装操作方法
int mycdev_open(struct inode *inode, struct file *file)
{
	printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
	return 0;
}

ssize_t mycdev_read(struct file *file, char *ubuf, size_t size, loff_t *lof)
{
	int ret;
	if (sizeof(kbuf) < size)
		size = sizeof(kbuf);
	ret = copy_to_user(ubuf, kbuf, size);
	if (ret) {
		printk("copy_tO_user filed\n");
		return -EIO;
	}
	printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
	return 0;
}

ssize_t mycdev_write(struct file *file, const char *ubuf, size_t size,
		     loff_t *lof)
{
	int ret;
	//判断用户空间的需求是否能被驱动满足，满足不了就给能给的最好的
	if (sizeof(kbuf) < size)
		size = sizeof(kbuf);
	ret = copy_from_user(kbuf, ubuf, size);
	if (ret) {
		printk("copy_from_user filed\n");
		return -EIO;
	}
	if (kbuf[0] == '1') { // 开关 LED1
		// (*vir_odr) ^= (0x1 << 10);
    ((gpio_t *)vir_odr)->ODR ^= (0x1 << 10);
	} 
	printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
	return 0;
}

int mycdev_close(struct inode *inode, struct file *file)
{
	printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
	return 0;
}

// 定义操作方法结构体变量并完成初始化
struct file_operations fops = {
	.open = mycdev_open,
	.read = mycdev_read,
	.write = mycdev_write,
	.release = mycdev_close,
};

static int __init mycdev_init(void)
{
	//字符设备驱动注册
	major = register_chrdev(0, "mychrdev", &fops);
	if (major < 0) {
		printk("字符设备驱动注册失败\n");
		return major;
	}
	printk("字符设备驱动注册成功major=%d\n", major);
	//映射物理寄存器
  vir_odr = ioremap((volatile unsigned int)GPIOE, sizeof(gpio_t));
	if (vir_odr == NULL) {
		printk("GPIOE结构体映射表失败\n");
		return -EFAULT;
	}
  vir_rcc = ioremap((volatile unsigned int)RCC, sizeof(rcc_t));
	if (vir_rcc == NULL) {
		printk("RCC结构体映射表失败\n");
		return -EFAULT;
	}
	printk("寄存器地址映射成功\n");
	//寄存器初始化
  ((rcc_t *)vir_rcc)->MP_AHB4ENSETR |= (1 << 4) | (1 << 5);
  ((gpio_t *)vir_odr)->MODER &= (~(0X3 << 20));
  ((gpio_t *)vir_odr)->MODER |= (0x1 << 20);
  ((gpio_t *)vir_odr)->ODR &= (~(0x1 << 10));
	printk("硬件寄存器初始化成功\n");
	return 0;
}

static void __exit mycdev_exit(void)
{
	//取消寄存器地址映射
	iounmap(vir_moder);
	iounmap(vir_odr);
	iounmap(vir_rcc);
	//字符设备驱动的注销
	unregister_chrdev(major, "mychrdev");
}

module_init(mycdev_init);
module_exit(mycdev_exit);
MODULE_LICENSE("GPL");
