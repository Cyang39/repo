#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/mod_devicetable.h>
#include <linux/gpio.h>
#include <linux/of.h>
#include <linux/of_gpio.h>

struct resource *res;
unsigned int irqno;
struct gpio_desc *gpiono;

/*
  myplatform{
    compatible = "hqyj,myplatform";
    reg=<0X12345678 0X400>;
    interrupt-parent=<&gpiof>;
    interrupts=<9 0>;
    led1=<&gpioe 10 0>;
  };
*/

//probe函数，匹配设备成功执行
int pdrv_probe(struct platform_device *pdev)
{
	printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
	//获取设备信息
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res == NULL) {
		printk("获取资源失败\n");
		return -ENXIO;
	}
	printk("获取资源信息成功 %x\n", res->start);
	irqno = platform_get_irq(pdev, 0);
	if (irqno < 0) {
		printk("获取中断资源失败\n");
		return irqno;
	}
	printk("中断类型资源为%d\n", irqno);
	//获取gpio信息
	//pdev->dev.of_node  设备树匹配之后会把设备树节点结构体首地址赋值给dev的of_node成员
	gpiono = gpiod_get_from_of_node(pdev->dev.of_node, "led1", 0,
					GPIOD_OUT_LOW, NULL);
	if (IS_ERR(gpiono)) {
		printk("解析GPIO信息失败\n");
		return -PTR_ERR(gpiono);
	}
	//亮灯
	gpiod_set_value(gpiono, 1);
	return 0;
}

//remove 设备和驱动分离时执行
int pdrv_remove(struct platform_device *pdev)
{
	printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
	//灭灯
	gpiod_set_value(gpiono, 0);
	//s释放gpio信息
	gpiod_put(gpiono);
	return 0;
}

//构建设备树匹配的表
struct of_device_id oftable[] = {
	{
		.compatible = "hqyj,myplatform",
	},
	{
		.compatible = "hqyj,myplatform1",
	},
	{},
};

struct platform_driver pdrv={
  .probe=pdrv_probe,
  .remove=pdrv_remove,
  .driver={
    .name="aaaaa", 
    .of_match_table=oftable,//设置设备树匹配
  },
};

//一键注册宏
module_platform_driver(pdrv);
MODULE_LICENSE("GPL");
