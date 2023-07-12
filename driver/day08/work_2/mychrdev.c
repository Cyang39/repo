#include <linux/kernel.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include "head.h"

/*
  myleds{
     led1=<&gpioe 10 0>;    
     led2=<&gpiof 10 0>; 
     led3=<&gpioe 8 0>;  
  };
*/

struct device_node *dnode;
struct gpio_desc *led[3];
int major;
struct class *cls;
struct device *dev;

static int mycdev_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int mycdev_release(struct inode *inode, struct file *filp)
{
	return 0;
}

long mycdev_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	switch (cmd) {
	case LED_TOGGLE:
		gpiod_set_value(led[arg], !gpiod_get_value(led[arg]));
		break;
	}
	return 0;
}

struct file_operations fops = {
	.open = mycdev_open,
	.unlocked_ioctl = mycdev_ioctl,
	.release = mycdev_release,
};

static int __init mycdev_init(void)
{
	int i;
	char str[5];
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
	//解析led灯的设备树节点
	dnode = of_find_node_by_path("/myleds");
	if (dnode == NULL) {
		printk("解析设备树节点失败\n");
		return -ENXIO;
	}
	printk("解析设备树节点成功\n");
	//根据设备树节点解析 led123 gpio结构体并向内核注册
	for (i = 0; i < 3; i++) {
		sprintf(str, "led%d", i + 1);
		led[i] = gpiod_get_from_of_node(dnode, str, 0, GPIOD_OUT_LOW,
						NULL);
		if (IS_ERR(led[i])) {
			printk("申请gpio失败\n");
			return -PTR_ERR(led[i]);
		}
	}
	return 0;
}

static void __exit mycdev_exit(void)
{
	int i;
	//灭灯
	for (i = 0; i < 3; i++)
		gpiod_set_value(led[i], 0);
	//注销gpio信息
	for (i = 0; i < 3; i++)
		gpiod_put(led[i]);
	for (i = 0; i < 3; i++)
		device_destroy(cls, MKDEV(major, i));
	class_destroy(cls);
	unregister_chrdev(major, "mychrdev");
}

module_init(mycdev_init);
module_exit(mycdev_exit);
MODULE_LICENSE("GPL");
