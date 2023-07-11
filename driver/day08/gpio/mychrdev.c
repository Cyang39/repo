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
unsigned int gpiono;

static int __init mycdev_init(void)
{
	int ret;
	//解析led灯的设备树节点
	dnode = of_find_node_by_path("/myleds");
	if (dnode == NULL) {
		printk("解析设备树节点失败\n");
		return -ENXIO;
	}
	printk("解析设备树节点成功\n");
	//根据设备树节点解析led1gpio编号
	gpiono = of_get_named_gpio(dnode, "led1", 0);
	if (gpiono < 0) {
		printk("gpio编号解析失败\n");
	}
	printk("解析gpio编号成功gpiono=%d\n", gpiono);
	//申请gpio编号
	ret = gpio_request(gpiono, NULL);
	if (ret < 0) {
		printk("申请gpio编号失败\n");
		return ret;
	}
	//led1对应的gpio的管脚为输出
	gpio_direction_output(gpiono, 0);
	//设置管脚输出高电平，亮灯
	gpio_set_value(gpiono, 1);

	return 0;
}

static void __exit mycdev_exit(void)
{
	//灭灯
	gpio_set_value(gpiono, 0);
	//释放gpio编号
	gpio_free(gpiono);
}

module_init(mycdev_init);
module_exit(mycdev_exit);
MODULE_LICENSE("GPL");
