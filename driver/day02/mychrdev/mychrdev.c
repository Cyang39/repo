#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>

int major; // 保存主设备号

// 封装操作方法
int mycdev_open(struct inode *inode, struct file *file)
{
	printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
	return 0;
}

ssize_t mycdev_read(struct file *file, char *ubuf, size_t size, loff_t *iof)
{
	printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
	return 0;
}

ssize_t mycdev_write(struct file *file, const char *ubuf, size_t size,
		     loff_t *lof)
{
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
  unregister_chrdev(major, "mychrdev");
}

module_init(mycdev_init);
module_exit(mycdev_exit);
MODULE_LICENSE("GPL");
