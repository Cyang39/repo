#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

struct cdev *cdev;
unsigned int major = 500;
unsigned int minor = 0; //次设备号的起始值
dev_t devno;
struct class *cls;
struct device *dev;
char kbuf[128] = { 0 };
//定义等待队列头
wait_queue_head_t wq_head;
unsigned int condition = 0;
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
	wait_event_interruptible(wq_head, condition); //将进程切换为休眠

	ret = copy_to_user(ubuf, kbuf, size);
	if (ret) {
		printk("copy_tO_user filed\n");
		return -EIO;
	}
	printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
	condition = 0; //表示下一次硬件数据没有准备好
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
	condition = 1; //表示硬件数据就绪
	wake_up_interruptible(&wq_head); //唤醒休眠的进程

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
	.read = mycdev_read,
	.write = mycdev_write,
	.release = mycdev_close,
};

static int __init mycdev_init(void)
{
	int ret, i;
	//初始化等待队列头
	init_waitqueue_head(&wq_head);
	//1.分配字符设备驱动对象空间  cdev_alloc
	cdev = cdev_alloc();
	if (cdev == NULL) {
		printk("申请字符设备驱动对象空间失败\n");
		ret = -EFAULT;
		goto out1;
	}
	printk("字符设备驱动对象申请成功\n");
	//2.字符设备驱动对象部分初始化  cdev_init
	cdev_init(cdev, &fops);
	//3.申请设备号  register_chrdev_region/alloc_chrdev_region
	if (major > 0) //静态申请设备号
	{
		ret = register_chrdev_region(MKDEV(major, minor), 3, "myled");
		if (ret) {
			printk("静态指定设备号失败\n");
			goto out2;
		}
	} else //动态申请设备号
	{
		ret = alloc_chrdev_region(&devno, minor, 3, "myled");
		if (ret) {
			printk("动态申请设备号失败\n");
			goto out2;
		}
		major = MAJOR(devno); //根据设备号得到主设备号
		minor = MINOR(devno); //根据设备号得到次设备号
	}
	printk("申请设备号成功\n");
	//4.注册字符设备驱动对象  cdev_add()
	ret = cdev_add(cdev, MKDEV(major, minor), 3);
	if (ret) {
		printk("注册字符设备驱动对象失败\n");
		goto out3;
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
		dev = device_create(cls, NULL, MKDEV(major, i), NULL, "myled%d",
				    i);
		if (IS_ERR(dev)) {
			printk("向上提交节点信息失败\n");
			ret = -PTR_ERR(dev);
			goto out5;
		}
	}
	printk("向上提交设备节点信息成功\n");

	return 0;
out5:
	for (--i; i >= 0; i--) {
		//销毁上面提交的设备信息
		device_destroy(cls, MKDEV(major, i));
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
	for (i = 0; i < 3; i++) {
		device_destroy(cls, MKDEV(major, i));
	}
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
