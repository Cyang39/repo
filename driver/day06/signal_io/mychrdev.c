#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/cdev.h>
#include <linux/ioctl.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/poll.h>

int minor = 0; //次设备号从0开始
#if 0
unsigned int major = 0;//动态申请
#else
unsigned int major = 500; //静态指定设备号
#endif
char kbuf[128] = {}; //定义数组用于存放和用户之间拷贝的数据
struct cdev *cdev;
struct class *cls;
struct device *dev;
unsigned int count = 3;
//先去定义一个fasync_struct对象
struct fasync_struct *fapp;
//对应的是open()
int mycdev_open(struct inode *inode, struct file *file)
{
	printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
	return 0;
}
//read()
ssize_t mycdev_read(struct file *file, char __user *ubuf, size_t size,
		    loff_t *loff)
{
	//size参数是用户期待读到的字节长度
	int ret;
	//把父进程拷贝到内核的数据再拷贝给子进程
	if (size > sizeof(kbuf))
		size = sizeof(kbuf);
	ret = copy_to_user(ubuf, kbuf, size);
	if (ret) {
		printk("数据从内核向用户拷贝失败\n");
		return -EIO;
	}

	return size;
}
//write()
ssize_t mycdev_write(struct file *file, const char __user *ubuf, size_t size,
		     loff_t *loff)
{
	int ret;
	if (size > sizeof(kbuf))
		size = sizeof(kbuf);
	ret = copy_from_user(kbuf, ubuf, size);
	if (ret) {
		printk("数据从内核向用户拷贝失败\n");
		return -EIO;
	}
	//信号的发送
	kill_fasync(&fapp, SIGIO, POLL_IN);

	return size;
}
int mycdev_fasync(int fd, struct file *file, int on)
{
	//发信号前的准备工作
	return fasync_helper(fd, file, on, &fapp);
}

int mycdev_close(struct inode *inode, struct file *file)
{
	printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
	return 0;
}
//操作方法结构体的初始化
struct file_operations fops = {
	.open = mycdev_open,
	.read = mycdev_read,
	.write = mycdev_write,
	.fasync = mycdev_fasync,
	.release = mycdev_close,
};
//入口函数，当驱动安装的时候执行
static int __init mycdev_init(void)
{
	int ret, i;
	//分配对象
	dev_t devno;
	cdev = cdev_alloc();
	if (cdev == NULL) {
		printk("cdev alloc memory err\n");

		ret = -ENOMEM;
		goto ERR1;
	}
	printk("对象分配成功\n");
	//对象的初始化
	cdev_init(cdev, &fops);
	//设备号的申请

	if (major == 0) //动态申请
	{
		ret = alloc_chrdev_region(&devno, minor, count, "my_led");
		if (ret) {
			printk("动态申请设备号失败\n");
			goto ERR2;
		}
		major = MAJOR(devno);
		minor = MINOR(devno);
		printk("动态申请设备号成功\n");
	} else {
		ret = register_chrdev_region(MKDEV(major, minor), count,
					     "my_led");
		if (ret) {
			printk("静态申请设备号失败\n");
			goto ERR2;
		}
		printk("静态申请设备号成功\n");
	}
	//注册字符设备驱动
	ret = cdev_add(cdev, MKDEV(major, minor), count);
	if (ret) {
		printk("字符设备驱动注册失败\n");
		goto ERR3;
	}
	printk("注册字符设备驱动成功\n");

	//自动创建设备节点
	cls = class_create(THIS_MODULE, "led");
	if (IS_ERR(cls)) {
		printk("创建逻辑节点目录失败\n");
		ret = PTR_ERR(cls);
		goto ERR4;
	}
	printk("创建逻辑节点目录成功\n");
	//向上提交节点信息
	for (i = 0; i < 3; i++) {
		dev = device_create(cls, NULL, MKDEV(major, i), NULL,
				    "my_led%d", i);
		if (IS_ERR(dev)) {
			printk("创建逻辑节点失败\n");
			ret = PTR_ERR(dev);
			goto ERR5;
		}
	}
	printk("创建逻辑节点成功\n");

	return 0;
ERR5:
	for (--i; i >= 0; i--) {
		device_destroy(cls, MKDEV(major, i));
	}
	class_destroy(cls);
ERR4:
	cdev_del(cdev);
ERR3:
	unregister_chrdev_region(MKDEV(major, minor), count);
ERR2:
	kfree(cdev);
ERR1:
	return ret;
}
static void __exit mycdev_exit(void)
{
	//1.销毁设备节点
	int i;
	for (i = 0; i < count; i++) {
		device_destroy(cls, MKDEV(major, i));
	}
	class_destroy(cls);
	//2.注销字符设备驱动
	cdev_del(cdev);
	//3.释放设备号
	unregister_chrdev_region(MKDEV(major, minor), count);
	//4.释放动态申请的空间
	kfree(cdev);
}
module_init(mycdev_init);
module_exit(mycdev_exit);
MODULE_LICENSE("GPL");
