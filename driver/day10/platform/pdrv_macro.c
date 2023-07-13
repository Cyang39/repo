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

module_platform_driver(pdrv);
MODULE_LICENSE("GPL");
