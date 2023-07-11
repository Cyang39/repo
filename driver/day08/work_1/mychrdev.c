#include "linux/gpio/consumer.h"
#include "linux/printk.h"
#include <linux/init.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/timer.h>

/*
  myleds{
     led1=<&gpioe 10 0>;    
     led2=<&gpiof 10 0>; 
     led3=<&gpioe 8 0>;  
  };
*/

struct device_node *dnode;
struct gpio_desc *gpiono;

struct timer_list mytimer; //定义定时器对象

//定义定时器处理函数
void mytimer_func(struct timer_list *timer)
{
	static int status = 0;
	status = !status;
	gpiod_set_value(gpiono, status);
	//再次启用定时器
	mod_timer(timer, jiffies + HZ);
}

static int __init mycdev_init(void)
{
	//解析 led 灯的设备树节点
	dnode = of_find_node_by_path("/myleds");
	if (dnode == NULL) {
		printk("解析设备树节点失败\n");
		return -ENXIO;
	}
	printk("解析设备树节点成功\n");
	//根据设备树节点获取 led 灯的 gpio 编号并向内核注册
	gpiono = gpiod_get_from_of_node(dnode, "led1", 0, GPIOD_OUT_LOW, NULL);
	if (IS_ERR(gpiono)) {
		printk("申请gpio失败\n");
		return -PTR_ERR(gpiono);
	}
	//初始化定时器对象
	mytimer.expires = jiffies + HZ;
	timer_setup(&mytimer, mytimer_func, 0);
	//将定时器对象注册进内核
	add_timer(&mytimer);

	return 0;
}

static void __exit mycdev_exit(void)
{
	del_timer(&mytimer);
}

module_init(mycdev_init);
module_exit(mycdev_exit);
MODULE_LICENSE("GPL");
