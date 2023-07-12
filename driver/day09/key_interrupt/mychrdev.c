#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>

/*
*/

struct device_node *dnode;
unsigned int irqno;

// 中断处理函数
irqreturn_t myirq_handler(int irqno, void *dev_id)
{
	printk("key1 interrupt\n");
	return IRQ_HANDLED;
}

static int __init mycdev_init(void)
{
	int ret;
	dnode = of_find_node_by_name(NULL, "mykeys");
	if (dnode == NULL) {
		printk("of_find_node_by_name failed\n");
		return -ENXIO;
	}
	printk("dnode = %p\n", dnode);
	// 获取软中断号
	irqno = irq_of_parse_and_map(dnode, 0);
	if (!irqno) {
		printk("irq_of_parse_and_map failed\n");
		return -ENOMEM;
	}
	printk("irqno = %d\n", irqno);
	// 注册软中断
	ret = request_irq(irqno, myirq_handler, IRQF_TRIGGER_FALLING, "key1",
			  NULL);
	if (ret) {
		printk("request_irq failed\n");
		return ret;
	}
	printk("request_irq success\n");
	return 0;
}

static void __exit mycdev_exit(void)
{
	// 注销中断
	free_irq(irqno, NULL);
}

module_init(mycdev_init);
module_exit(mycdev_exit);
MODULE_LICENSE("GPL");
