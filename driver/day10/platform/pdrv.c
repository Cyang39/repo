#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>

//probe函数，匹配设备成功执行
int pdrv_probe(struct platform_device *pdev)
{
	printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
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

static int __init mycdev_init(void)
{
	//注销驱动信息对象
	platform_driver_register(&pdrv);
	return 0;
}

static void __exit mycdev_exit(void)
{
	//注销驱动信息对象
	platform_driver_unregister(&pdrv);
}

module_init(mycdev_init);
module_exit(mycdev_exit);
MODULE_LICENSE("GPL");
