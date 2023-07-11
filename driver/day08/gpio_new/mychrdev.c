#include <linux/init.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>

/*
   myleds{
       led1=<&gpioe 10 0>;    
       led2=<&gpiof 10 0>; 
       led3=<&gpioe 8 0>;  
    };
*/

struct device_node *dnode;
struct gpio_desc *gpiono;

static int __init mycdev_init(void)
{
	//解析led灯的设备树节点
	dnode = of_find_node_by_path("/myleds");
	if (dnode == NULL) {
		printk("解析设备树节点失败\n");
		return -ENXIO;
	}
	printk("解析设备树节点成功\n");
	//根据设备树节点解析led1 gpio结构体并向内核注册
	gpiono = gpiod_get_from_of_node(dnode, "led1", 0, GPIOD_OUT_LOW, NULL);
	if (IS_ERR(gpiono)) {
		printk("申请gpio失败\n");
		return -PTR_ERR(gpiono);
	}
	//亮灯
	gpiod_set_value(gpiono, 1);

	return 0;
}

static void __exit mycdev_exit(void)
{
	//灭灯
	gpiod_set_value(gpiono, 0);
	//注销gpio信息
	gpiod_put(gpiono);
}

module_init(mycdev_init);
module_exit(mycdev_exit);
MODULE_LICENSE("GPL");
