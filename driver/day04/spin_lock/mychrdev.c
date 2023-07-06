#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include "head.h"

struct class *cls;
struct device *dev;
int major;
char kbuf[128] = { 0 };
gpio_t *vir_led1;
gpio_t *vir_led2;
gpio_t *vir_led3;
unsigned int *vir_rcc;
//定义自旋锁
spinlock_t lock;
int mycdev_open(struct inode *inode, struct file *file)
{
	printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
	return 0;
}
long mycdev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int which;
	//通过拷贝函数获取ioctl第三个参数对应空间的数值
	int ret = copy_from_user(&which, (void *)arg, 4);
	if (ret) {
		printk("拷贝用户空间数据失败\n");
		return -EIO;
	}
	//上锁
	spin_lock(&lock);
	//进行功能控制
	switch (cmd) {
	case LED_ON: // 开灯
		switch (which) {
		case 1: // LED1
			vir_led1->ODR |= (0x1 << 10);
			break;
		case 2: // LED2
			vir_led2->ODR |= (0x1 << 10);
			break;
		case 3: // LED3
			vir_led3->ODR |= (0x1 << 8);
			break;
		}
		break;
	case LED_OFF: // 关灯
		switch (which) {
		case 1:
			vir_led1->ODR &= (~(0X1 << 10));
			break;
		case 2:
			vir_led2->ODR &= (~(0X1 << 10));
			break;
		case 3:
			vir_led3->ODR &= (~(0X1 << 8));
			break;
		}
		break;
	}

	//解锁
	spin_unlock(&lock);
	return 0;
}
int mycdev_close(struct inode *inode, struct file *file)
{
	printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
	return 0;
}

// 定义操作方法结构体变量并赋值
struct file_operations fops = {
	.open = mycdev_open,
	.unlocked_ioctl = mycdev_ioctl,
	.release = mycdev_close,
};

int all_led_init(void)
{
	// 寄存器地址的映射
	vir_led1 = ioremap(PHY_LED1_ADDR, sizeof(gpio_t));
	if (vir_led1 == NULL) {
		printk("ioremap filed：%d\n", __LINE__);
		return -ENOMEM;
	}

	vir_led2 = ioremap(PHY_LED2_ADDR, sizeof(gpio_t));
	if (vir_led2 == NULL) {
		printk("ioremap filed：%d\n", __LINE__);
		return -ENOMEM;
	}
	vir_led3 = vir_led1;
	vir_rcc = ioremap(PHY_RCC_ADDR, 4);
	if (vir_rcc == NULL) {
		printk("ioremap filed：%d\n", __LINE__);
		return -ENOMEM;
	}

	printk("物理地址映射成功\n");

	// 寄存器的初始化
	// rcc
	(*vir_rcc) |= (3 << 4);
	// led1
	vir_led1->MODER &= (~(3 << 20));
	vir_led1->MODER |= (1 << 20);
	vir_led1->ODR &= (~(1 << 10));
	// led2
	vir_led2->MODER &= (~(3 << 20));
	vir_led2->MODER |= (1 << 20);
	vir_led2->ODR &= (~(1 << 10));
	// led3
	vir_led3->MODER &= (~(3 << 16));
	vir_led1->MODER |= (1 << 16);
	vir_led1->ODR &= (~(1 << 8));
	printk("寄存器初始化成功\n");

	return 0;
}

static int __init mycdev_init(void)
{
	int i;
	//初始化自旋锁
	spin_lock_init(&lock);
	// 字符设备驱动注册
	major = register_chrdev(0, "mychrdev", &fops);
	if (major < 0) {
		printk("字符设备驱动注册失败\n");
		return major;
	}
	printk("字符设备驱动注册成功:major=%d\n", major);
	// 向上提交目录
	cls = class_create(THIS_MODULE, "mychrdev");
	if (IS_ERR(cls)) {
		printk("向上提交目录失败\n");
		return -PTR_ERR(cls);
	}
	// 向上提交设备节点信息
	// 为三盏灯创建三个设备文件
	for (i = 0; i < 3; i++) {
		dev = device_create(cls, NULL, MKDEV(major, i), NULL,
				    "mychrdev%d", i);
		if (IS_ERR(dev)) {
			printk("向上提交设备节点信息失败\n");

			return -PTR_ERR(dev);
		}
	}
	// 寄存器映射以及初始化
	all_led_init();

	return 0;
}
static void __exit mycdev_exit(void)
{
	int i;
	// 取消地址映射
	iounmap(vir_led1);
	iounmap(vir_led2);
	iounmap(vir_rcc);
	// 销毁设备节点信息
	for (i = 0; i < 3; i++) {
		device_destroy(cls, MKDEV(major, i));
	}
	// 销毁目录信息
	class_destroy(cls);
	// 注销字符设备驱动
	unregister_chrdev(major, "mychrdev");
}
module_init(mycdev_init);
module_exit(mycdev_exit);
MODULE_LICENSE("GPL");
