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

struct timer_list mytimer; //定义定时器对象

//定义定时器处理函数
void mytimer_func(struct timer_list *timer)
{
	printk("定时器处理函数执行\n");
	//再次启用定时器
	mod_timer(timer, jiffies + HZ);
}

static int __init mycdev_init(void)
{
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
