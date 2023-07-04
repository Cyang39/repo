#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
int major; // 用于保存主设备号
char kbuf[128] = { 0 };

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
	if (sizeof(kbuf) <
	    size) //判断用户空间的需求是否能被驱动满足，满足不了就给能给的最好的
		size = sizeof(kbuf);
	ret = copy_from_user(kbuf, ubuf, size);
	if (ret) {
		printk("copy_from_user filed\n");
		return -EIO;
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
	return 0;
}

static void __exit mycdev_exit(void)
{
	//字符设备驱动的注销
	unregister_chrdev(major, "mychrdev");
}

module_init(mycdev_init);
module_exit(mycdev_exit);
MODULE_LICENSE("GPL");
