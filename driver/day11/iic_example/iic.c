#include "linux/printk.h"
#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/fs.h>
#include <linux/device.h>
#include "head.h"

unsigned int major = 0;
struct class *cls;
struct device *devs;
struct i2c_client *client1;

// 读取温湿度的函数
int i2c_read_hum_tem(char reg)
{
	short value;
	char r_buf[] = { reg };
	int ret;
	// 封装消息
	struct i2c_msg r_msg[] = {
    [0] = {
      .addr = client1->addr,
      .flags = 0,
      .len = sizeof(r_buf),
      .buf = r_buf,
    },
    [1] = {
      .addr = client1->addr,
      .flags = 1,
      .len = 1,
      .buf = (char *)&value,
    },
  };
	ret = i2c_transfer(client1->adapter, r_msg, 2);
	if (ret < 0) {
		printk("i2c_transfer error\n");
		return -EIO;
	}
	return value;
};

int si7006_open(struct inode *inode, struct file *filp)
{
	printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
	return 0;
}

long si7006_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int tem, hum, ret;
	switch (cmd) {
	case GET_HUM: // 读取湿度
		hum = i2c_read_hum_tem(0xE5);
		ret = copy_to_user((void *)arg, &hum, 4); // 数据大小是4字节
		if (ret < 0) {
			printk("copy_to_user error\n");
			return -EIO;
		}
		break;
	case GET_TEM: // 读取温度
		tem = i2c_read_hum_tem(0xE3);
		ret = copy_to_user((void *)arg, &tem, 4); // 数据大小是4字节
		if (ret < 0) {
			printk("copy_to_user error\n");
			return -EIO;
		}
		break;
	}
	return 0;
}

int si7006_close(struct inode *inode, struct file *filp)
{
	printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
	return 0;
}

struct file_operations fops = {
  .open = si7006_open,
  .unlocked_ioctl = si7006_ioctl,
  .release = si7006_close,
};

int i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	client1 = client;
	// 字符设备驱动的注册
	major = register_chrdev(0, "si7006", &fops);
	if (major < 0) {
		printk("register_chrdev error\n");
		return major;
	}
	printk("major = %d\n", major);
	// 字符节点的创建
	cls = class_create(THIS_MODULE, "si7006");
	if (IS_ERR(cls)) {
		printk("class_create error\n");
		return PTR_ERR(cls);
	}
	printk("class_create ok\n");
	devs = device_create(cls, NULL, MKDEV(major, 0), NULL, "si7006");
	if (IS_ERR(devs)) {
		printk("device_create error\n");
		return PTR_ERR(devs);
	}
	printk("device_create ok\n");
	return 0;
}

int i2c_remove(struct i2c_client *client)
{
	// 设备信息的释放
	device_destroy(cls, MKDEV(major, 0));
	// 字符节点的销毁
	class_destroy(cls);
	// 字符设备驱动的注销
	unregister_chrdev(major, "si7006");
	return 0;
}

struct of_device_id oftable[] = {
	{ .compatible = "mahiro,si7006" },
	{},
};

struct i2c_driver i2c_drv = {
  .probe = i2c_probe,
  .remove = i2c_remove,
  .driver = {
    .name = "si7006",
    .of_match_table = oftable,
  },
};

module_i2c_driver(i2c_drv);
MODULE_LICENSE("GPL");
