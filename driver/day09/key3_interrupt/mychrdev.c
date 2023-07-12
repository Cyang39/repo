#include <linux/init.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/interrupt.h>
#include <linux/of_irq.h>

/*mykeys{
    interrupt-parent=<&gpiof>;
    interrupts=<9 0>,<7 0>,<8 0>;
};*/

struct device_node *dnode;
unsigned int irqno[3];

// 中断处理函数
irqreturn_t myirq_handler(int irqno, void *dev_id)
{
	unsigned int arg = (unsigned int)dev_id;
	switch (arg) {
	case 0:
		printk("key1 interrupt\n");
		break;
	case 1:
		printk("key2 interrupt\n");
		break;
	case 2:
		printk("key3 interrupt\n");
		break;
	}

	return IRQ_HANDLED;
}

static int __init mycdev_init(void)
{
	int ret, i;
	// 解析设备树节点
	dnode = of_find_node_by_name(NULL, "mykeys");
	if (dnode == NULL) {
		printk("解析设备树节点失败\n");
		return -ENXIO;
	}
	printk("设备树节点解析成功\n");

	for (i = 0; i < 3; i++) {
		// 获取软中断号
		irqno[i] = irq_of_parse_and_map(dnode, i);
		if (!irqno[i]) {
			printk("软中断号获取失败\n");
			return -ENOMEM;
		}
		printk("软中断号获取成功irqno=%d\n", irqno[i]);
		// 注册中断
		ret = request_irq(irqno[i], myirq_handler, IRQF_TRIGGER_FALLING,
				  "key", (void *)i);
		if (ret) {
			printk("注册驱动失败\n");
			goto out;
		}
		printk("key1中断注册成功\n");
	}
	return 0;
out:
	for (; i >= 0; i--)
		free_irq(irqno[i], (void *)i);
	return ret;
}

static void __exit mycdev_exit(void)
{
	int i;
	// 注销中断
	for (i = 0; i < 3; i++) {
		free_irq(irqno[i], (void *)i);
	}
}

module_init(mycdev_init);
module_exit(mycdev_exit);
MODULE_LICENSE("GPL");
