#include <linux/slab.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/uacce.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/io.h>
#include "head.h"

struct cdev *cdev;
unsigned int major = 0; // 自动分配主设备号
unsigned int minor = 0; // 次设备号的起始值
dev_t devno;
struct class *cls;
struct device *dev;

gpio_t *vir_gpioe;
gpio_t *vir_gpiof;
rcc_t *vir_rcc;

int all_led_init(void)
{
	// 地址映射
	vir_gpioe = (gpio_t *)ioremap((unsigned int)GPIOE, sizeof(gpio_t));
	if (vir_gpioe == NULL) {
		printk("GPIOE地址映射失败\n");
		return -1;
	}
	vir_gpiof = (gpio_t *)ioremap((unsigned int)GPIOF, sizeof(gpio_t));
	if (vir_gpiof == NULL) {
		printk("GPIOF地址映射失败\n");
		return -1;
	}
	vir_rcc = (rcc_t *)ioremap((unsigned int)RCC, sizeof(rcc_t));
	if (vir_rcc == NULL) {
		printk("RCC地址映射失败\n");
		return -1;
	}
	// 寄存器初始化
	vir_rcc->MP_AHB4ENSETR |= (1 << 4) | (1 << 5); // 使能GPIOE和GPIOF时钟
	// LED1
	vir_gpioe->MODER &= (~(0x3 << 20));
	vir_gpioe->MODER |= (0x1 << 20); // 设置PE10为输出模式
	vir_gpioe->ODR &= (~(1 << 10)); // 默认PE10输出低电平
	// LED2
	vir_gpiof->MODER &= (~(0x3 << 20));
	vir_gpiof->MODER |= (0x1 << 20); // 设置PF10为输出模式
	vir_gpiof->ODR &= (~(1 << 10)); // 默认PF10输出低电平
	// LED3
	vir_gpioe->MODER &= (~(0x3 << 16));
	vir_gpioe->MODER |= (0x1 << 16); // 设置PE8为输出模式
	vir_gpioe->ODR &= (~(1 << 8)); // 默认PE8输出低电平
	return 0;
}

int mycdev_open(struct inode *inode, struct file *file)
{
	unsigned int aaa = MINOR(inode->i_rdev); // 得到次设备号
	file->private_data = (void *)aaa; // 将次设备号保存到file->private_data
	printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
	return 0;
}

long mycdev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	unsigned int _minor = (unsigned int)file->private_data;
	switch (_minor) {
	case 0: // 对应 LED1
		switch (cmd) {
		case LED_ON:
			vir_gpioe->ODR |= (1 << 10);
			break;
		case LED_OFF:
			vir_gpioe->ODR &= ~(1 << 10);
			break;
		}
		break;
	case 1: // 对应 LED2
		switch (cmd) {
		case LED_ON:
			vir_gpiof->ODR |= (1 << 10);
			break;
		case LED_OFF:
			vir_gpiof->ODR &= ~(1 << 10);
			break;
		}
		break;
	case 2: // 对应 LED3
		switch (cmd) {
		case LED_ON:
			vir_gpioe->ODR |= (1 << 8);
			break;
		case LED_OFF:
			vir_gpioe->ODR &= ~(1 << 8);
			break;
		}
		break;
	}
	return 0;
}

int mycdev_close(struct inode *inode, struct file *file)
{
	printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
	return 0;
}

struct file_operations fops = {
	.open = mycdev_open,
	.unlocked_ioctl = mycdev_ioctl,
	.release = mycdev_close,
};

static int __init mycdev_init(void)
{
	int ret, i;
	//1.分配字符设备驱动对象空间  cdev_alloc
	cdev = cdev_alloc();
	if (cdev == NULL) {
		printk("字符设备驱动对象申请空间失败\n");
		return -EFAULT;
		goto out1;
	}
	printk("字符设备驱动对象申请空间成功\n");
	//2.字符设备驱动对象部分初始化  cdev_init
	cdev_init(cdev, &fops);
	//3.申请设备号  register_chrdev_region/alloc_chrdev_region
	if (major > 0) { // 静态指定设备号
		ret = register_chrdev_region(MKDEV(major, minor), 3, "myled");
		if (ret) {
			printk("静态指定设备号失败\n");
			goto out2;
		}
	} else { // 动态申请设备号
		ret = alloc_chrdev_region(&devno, minor, 3, "myled");
		if (ret) {
			printk("动态申请设备号失败\n");
			goto out2;
		}
		major = MAJOR(devno); // 根据设备号获取主设备号
		minor = MINOR(devno); // 根据设备号获取次设备号
	}
	printk("申请设备号成功——主设备号:%d, 次设备号:%d\n", major, minor);
	//4.注册字符设备驱动对象  cdev_add()
	for (i = 0; i < 3; i++) {
		ret = cdev_add(cdev, MKDEV(major, minor + i), 1);
		if (ret) {
			printk("注册字符设备驱动对象失败\n");
			goto out3;
		}
	}
	printk("注册字符设备驱动对象成功\n");
	//5.向上提交目录
	cls = class_create(THIS_MODULE, "myled");
	if (IS_ERR(cls)) {
		printk("向上提交目录失败\n");
		ret = -PTR_ERR(cls);
		goto out4;
	}
	printk("向上提交目录成功\n");
	//6.向上提交设备节点
	for (i = 0; i < 3; i++) {
		dev = device_create(cls, NULL, MKDEV(major, minor + i), NULL,
				    "myled%d", i);
		if (IS_ERR(dev)) {
			printk("向上提交设备节点失败\n");
			ret = -PTR_ERR(dev);
			goto out5;
		}
	}
	printk("向上提交设备节点成功\n");
	all_led_init();
	return 0;
out5:
	for (--i; i >= 0; i--) { // 从失败的位置开始回退
		device_destroy(cls, MKDEV(major, minor + i));
	}
	class_destroy(cls);
out4:
	cdev_del(cdev);
out3:
	unregister_chrdev_region(MKDEV(major, minor), 3);
out2:
	kfree(cdev);
out1:
	return ret;
}

static void __exit mycdev_exit(void)
{
	//1.销毁设备信息  device_destroy
	int i;
	for (i = 0; i < 3; i++)
		device_destroy(cls, MKDEV(major, minor + i));
	//2.销毁目录  class_destroy
	class_destroy(cls);
	//3.注销对象  cdev_del()
	cdev_del(cdev);
	//4.释放设备号   unregister_chrdev_region()
	unregister_chrdev_region(MKDEV(major, minor), 3);
	//5.释放对象空间  kfree()
	kfree(cdev);
}

module_init(mycdev_init);
module_exit(mycdev_exit);

MODULE_LICENSE("GPL");
