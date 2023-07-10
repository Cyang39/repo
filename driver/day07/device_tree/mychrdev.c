#include <linux/init.h>
#include <linux/module.h>
#include <linux/of.h>
/*  mynode@0x12345678{
        compatible = "hqyj,mynode";
        astring="hello 23031";
        uint  =<0xaabbccdd 0x11223344>;
        binarry=[00 0c 29 7b f9 be];
        mixed ="hello",[11 22],<0x12345678>;
     };*/
struct device_node *dnode; //用来指向目标结构体首地址
struct property *pro;
int length;

static int __init mycdev_init(void)
{
	int i;
	//解析设备树节点信息
	dnode = of_find_node_by_name(NULL, "mynode");
	if (dnode == NULL) { // 解析失败
		printk("解析设备树节点失败\n");
		return -ENXIO;
	}
	printk("解析设备树节点成功\n");
	//根据节点和指定键名找到目标信息
	pro = of_find_property(dnode, "uint", &length);
	if (pro == NULL) {
		printk("解析设备树指定的属性失败\n");
		return -ENXIO;
	}
	printk("name=%s value=%x %x\n", pro->name,
	       __be32_to_cpup((unsigned int *)pro->value),
	       __be32_to_cpup((unsigned int *)pro->value + 1));
	pro = of_find_property(dnode, "binarry", &length);
	printk("name=%s ", pro->name);
	for (i = 0; i < length; i++) {
		printk("value=%x\n", *((char *)pro->value + i));
	}

	return 0;
}

static void __exit mycdev_exit(void)
{
}

module_init(mycdev_init);
module_exit(mycdev_exit);
MODULE_LICENSE("GPL");
