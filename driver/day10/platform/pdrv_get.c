#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>

struct resource *res;
unsigned int irqno;

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
	printk("获取资源信息成功 %llx\n", res->start);
	irqno = platform_get_irq(pdev, 0);
	if (irqno < 0) {
		printk("获取中断资源失败\n");
		return irqno;
	}
	printk("中断类型资源为%d\n", irqno);
	return 0;
}

//remove 设备和驱动分离时执行
int pdrv_remove(struct platform_device *pdev)
{
	printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
	return 0;
}

struct platform_driver pdrv={
  .probe=pdrv_probe,
  .remove=pdrv_remove,
  .driver={
    .name="aaaaa", 
  },
};

//一键注册宏
module_platform_driver(pdrv);
MODULE_LICENSE("GPL");
