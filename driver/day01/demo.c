#include <linux/init.h>
#include <linux/module.h>
 
int a=10;
module_param(a,int,0664);//声明当前变量可以进行命令行传参
MODULE_PARM_DESC(a,"value is a int");
// 内核模块入口函数
static int __init mycdev_init(void)
{
    printk(KERN_ERR "模块安装，执行入口函数\n");
    printk("a=%d\n",a);
    return 0;
}
// 内核模块出口函数
static void __exit mycdev_exit(void)
{
   printk(KERN_ERR "模块卸载，执行出口函数\n");
}
 
module_init(mycdev_init);
module_exit(mycdev_exit);
MODULE_LICENSE("GPL");
