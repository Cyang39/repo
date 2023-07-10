#include <linux/init.h>
#include <linux/module.h>
#include <linux/of.h>

/*mynode@0x12345678{
        compatible = "hqyj,mynode";
        astring="hello 22061";
        uint  =<0xaabbccdd 0x11223344>;
        binarry=[00 0c 29 7b f9 be];
        mixed ="hello",[11 22],<0x12345678>;
     };*/

//定义一个指向设备节点的指针
struct device_node *node;
struct property *pr; //属性结构体指针
int len;
u32 a;
u32 b[2];
const char *str;
u8 c[6];

static int __init mycdev_init(void)
{
	int i;
	//根据设备节点路径获取设备节点信息
	node = of_find_node_by_path("/mynode@0x12345678");
	if (node == NULL) {
		printk("获取节点信息失败\n");
		return ENODATA;
	}
	printk("获取节点信息成功\n");
	//获取u32数值
	of_property_read_u32_index(node, "uint", 0, &a);
	printk("%#x\n", a);
	of_property_read_u32_index(node, "uint", 1, &a);
	printk("%#x\n", a);
	printk("u32\n");
	//获取u32数组
	of_property_read_variable_u32_array(node, "uint", b, 2, 2);
	printk("%#x,%#x\n", b[0], b[1]);
	printk("u32数组\n");
	//获取字符串类型的值
	of_property_read_string(node, "astring", &str);
	printk("%s\n", str);
	printk("字符串\n");
	//获取u8类型值
	of_property_read_u8_array(node, "binarry", c, 6);
	for (i = 0; i < 6; i++) {
		printk("%#x\n", c[i]);
	}
	printk("u8\n");
	return 0;
}

static void __exit mycdev_exit(void)
{
}

module_init(mycdev_init);
module_exit(mycdev_exit);
MODULE_LICENSE("GPL");
