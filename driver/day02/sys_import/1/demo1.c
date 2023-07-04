#include <linux/init.h>
#include <linux/module.h>

// 定义函数
int add(int i, int j) {
  return i + j;
}

static int __init mycdev_init(void)
{
  
  return 0;
}
// 声明导出当前函数的符号表
EXPORT_SYMBOL(add);
static void __exit mycdev_exit(void)
{


}
module_init(mycdev_init);
module_exit(mycdev_exit);
MODULE_LICENSE("GPL");
