#include <linux/slab.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/fs.h>

struct cdev *cdev;
unsigned int major = 500;
unsigned int minor = 0; // 次设备号的起始值
dev_t devno;
struct class *cls;
struct device *dev;

int mycdev_open(struct inode *inode, struct file *file)
{
	printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
	return 0;
}

long mycdev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
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
	ret = cdev_add(cdev, MKDEV(major, minor), 1);
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
		dev = device_create(cls, NULL, MKDEV(major, minor + i), NULL,
				    "myled%d", i);
		if (IS_ERR(dev)) {
			printk("向上提交设备节点失败\n");
			ret = -PTR_ERR(dev);
			goto out5;
		}
	}
	printk("向上提交设备节点成功\n");
	// init_all_led()
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
