#include "linux/leds.h"
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include "head.h"

struct class *cls;
struct device *dev;

int major; // 用于保存主设备号
char kbuf[128] = { 0 };

unsigned int *vir_gpiob;
unsigned int *vir_rcc;

// 封装操作方法
int mycdev_open(struct inode *inode, struct file *file)
{
	printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
	return 0;
}

long mycdev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	switch (cmd) {
	case BEE_ON:
		((gpio_t *)vir_gpiob)->ODR |= (0x1 << 6);
		break;
	case BEE_OFF:
		((gpio_t *)vir_gpiob)->ODR &= (~(0x1 << 6));
		break;
	}
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
	.unlocked_ioctl = mycdev_ioctl,
	.release = mycdev_close,
};

int all_led_init(void)
{
	// 地址映射
	vir_gpiob = ioremap((unsigned int)GPIOB, sizeof(gpio_t));
	if (vir_gpiob == NULL) {
		printk("GPIOB结构体映射表失败\n");
		return -EFAULT;
	}
	vir_rcc = ioremap((unsigned int)RCC, sizeof(rcc_t));
	if (vir_rcc == NULL) {
		printk("RCC结构体映射表失败\n");
		return -EFAULT;
	}
	printk("寄存器地址映射成功\n");
	// 寄存器初始化
	((rcc_t *)vir_rcc)->MP_AHB4ENSETR |= (1 << 1);
	// BEE 初始化 PB6
	((gpio_t *)vir_gpiob)->MODER &= (~(0x3 << 12));
	((gpio_t *)vir_gpiob)->MODER |= (0x1 << 12);
	((gpio_t *)vir_gpiob)->ODR &= (~(0x1 << 6));
	return 0;
}

static int __init mycdev_init(void)
{
	//字符设备驱动注册
	major = register_chrdev(0, "mybeedev", &fops);
	if (major < 0) {
		printk("字符设备驱动注册失败\n");
		return major;
	}
	printk("字符设备驱动注册成功major=%d\n", major);
	// 向上提交目录
	cls = class_create(THIS_MODULE, "mybeecls");
	if (IS_ERR(cls)) {
		printk("创建类失败\n");
		return -EFAULT;
	}
	// 向上提交设备节点信息
	dev = device_create(cls, NULL, MKDEV(major, 0), NULL, "bee");
	if (IS_ERR(dev)) {
		printk("创建设备节点失败\n");
		return -EFAULT;
	}
	//映射物理寄存器
	all_led_init();
	return 0;
}

static void __exit mycdev_exit(void)
{
	//取消寄存器地址映射
	iounmap(vir_gpiob);
	iounmap(vir_rcc);
	device_destroy(cls, MKDEV(major, 0));
	class_destroy(cls);
	//字符设备驱动的注销
	unregister_chrdev(major, "mybeedev");
}

module_init(mycdev_init);
module_exit(mycdev_exit);
MODULE_LICENSE("GPL");
