#include <linux/init.h>
#include <linux/module.h>
#include <linux/spi/spi.h>

int m74hc595_probe(struct spi_device *spi)
{
  // 0x6D 的二进制为 0110 1101
  // 对应显示的数字为 5
  char buf[] = { 0x1, 0x6D };
  spi_write(spi, buf, sizeof(buf));
  printk("%s:%d\n", __FILE__, __LINE__);
  return 0;
}

int m74hc595_remove(struct spi_device *spi)
{
  char buf[] = { 0x0, 0x0 };
  spi_write(spi, buf, sizeof(buf));
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
