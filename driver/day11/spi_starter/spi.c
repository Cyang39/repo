#include <linux/init.h>
#include <linux/module.h>
#include <linux/spi/spi.h>

int m74hc595_probe(struct spi_device *spi)
{
	printk("%s:%d\n", __FILE__, __LINE__);
	return 0;
}

int m74hc595_remove(struct spi_device *spi)
{
	printk("%s:%d\n", __FILE__, __LINE__);
	return 0;
}

//设备树匹配表
struct of_device_id of_table[] = {
	{ .compatible = "mahiro,m74hc595" },
	{},
};

//定义SPI对象并且初始化
struct spi_driver m74hc595 ={ 
    .probe=m74hc595_probe,
    .remove=m74hc595_remove,
    .driver={
        .name="m74hc595",
        .of_match_table=of_table,
    },
};

module_spi_driver(m74hc595);
MODULE_LICENSE("GPL");
