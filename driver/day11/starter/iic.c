#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>

int i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	// 字符设备驱动的注册
	// 字符节点的创建
	// 设备信息的获取
	printk("i2c_probe\n");
	return 0;
}

int i2c_remove(struct i2c_client *client)
{
	// 设备信息的释放
	// 字符节点的销毁
	// 字符设备驱动的注销
	printk("i2c_remove\n");
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
