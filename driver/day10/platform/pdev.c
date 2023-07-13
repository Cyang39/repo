#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>

//release函数,用于释放申请的资源
void pdev_release(struct device *dev)
{
	printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
}

//资源数组
struct resource res[]={
  [0]={
    .start=0X12345678,
    .end=0X12345678+59,
    .flags=IORESOURCE_MEM,     
  },   
  [1]={
    .start=71,
    .end=71,
    .flags=IORESOURCE_IRQ,     
  },   
};

//定义并初始化
struct platform_device pdev={
  .name="aaaaa",
  .id=PLATFORM_DEVID_AUTO,
  .dev={
    .release=pdev_release,     
  },
  .num_resources=ARRAY_SIZE(res),
  .resource=res,
};

static int __init mycdev_init(void)
{
	//注册
	platform_device_register(&pdev);
	return 0;
}

static void __exit mycdev_exit(void)
{
	//注销
	platform_device_unregister(&pdev);
}

module_init(mycdev_init);
module_exit(mycdev_exit);
MODULE_LICENSE("GPL");
