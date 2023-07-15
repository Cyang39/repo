#include <linux/init.h>
#include <linux/module.h>
#include <linux/spi/spi.h>
#include <linux/kthread.h>

#define NUM_DIGITS 2

// 数字的位模式定义
const u8 digitPatterns[NUM_DIGITS][2] = {
  { 0x1, 0x6D }, // 数字 5 的位模式
  { 0x2, 0x6D } // 数字 6 的位模式
};

static struct spi_device *spi_dev;
static struct task_struct *display_thread;

int display_thread_func(void *data)
{
  int curr_digit = 0;

  while (!kthread_should_stop()) {
    u8 buf[2];

    buf[0] = digitPatterns[curr_digit][0];
    buf[1] = digitPatterns[curr_digit][1];

    spi_write(spi_dev, buf, sizeof(buf));

    curr_digit = (curr_digit + 1) % NUM_DIGITS;

    schedule(); // 调度其他任务
  }

  return 0;
}

int m74hc595_probe(struct spi_device *spi)
{
  spi_dev = spi;

  // 在另一个线程中显示数字
  display_thread =
    kthread_run(display_thread_func, NULL, "display_thread");

  return 0;
}

int m74hc595_remove(struct spi_device *spi)
{
  u8 buf[2] = { 0x0, 0x0 };
  kthread_stop(display_thread);

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
