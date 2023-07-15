/**
 * 这个代码有临界资源抢占的问题，
 * 因为有必要做笔记，所以保留一下
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/spi/spi.h>
#include <linux/timer.h>

#define NUM_DIGITS 2

// 数字的位模式定义
const u8 digitPatterns[NUM_DIGITS][2] = { { 0x1, 0x6D }, { 0x2, 0x6D } };

struct spi_device *spi_dev;
struct timer_list mytimer;

void mytimer_func(struct timer_list *timer)
{
  static int curr_digit = 0;

  u8 buf[2];
  buf[0] = digitPatterns[curr_digit][0];
  buf[1] = digitPatterns[curr_digit][1];

  spi_write(spi_dev, buf, sizeof(buf));

  // 循环索引更新
  curr_digit = (curr_digit + 1) % NUM_DIGITS;

  // 再次启用
  mod_timer(timer, jiffies + HZ);
}

int m74hc595_probe(struct spi_device *spi)
{
  spi_dev = spi;

  mytimer.expires = jiffies + HZ;
  timer_setup(&mytimer, mytimer_func, 0);
  add_timer(&mytimer);

  return 0;
}

int m74hc595_remove(struct spi_device *spi)
{
  u8 buf[2] = { 0x0, 0x0 };
  // 删除定时器
  del_timer(&mytimer);
  // 熄灭数码管
  spi_write(spi, buf, sizeof(buf));

  return 0;
}

// 设备树匹配表
struct of_device_id of_table[] = {
  { .compatible = "mahiro,m74hc595" },
{},
};

// 定义 SPI 对象并初始化
struct spi_driver m74hc595 = {
  .probe = m74hc595_probe,
  .remove = m74hc595_remove,
  .driver = {
    .name = "m74hc595",
    .of_match_table = of_table,
  },
};

module_spi_driver(m74hc595);
MODULE_LICENSE("GPL");
