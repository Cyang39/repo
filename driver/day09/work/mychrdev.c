#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/of_irq.h>
#include <linux/of_gpio.h>
#include <linux/interrupt.h>

int major;
struct class *cls;
struct device *dev;

int number = 0;
unsigned int condition = 0;
wait_queue_head_t wq_head;

// 所有的设备都在同一个节点下
struct device_node *dnode;
unsigned int irqno;
struct gpio_desc *gpiono;

ssize_t mycdev_read(struct file *file, char *ubuf, size_t size, loff_t *lof)
{
	int ret;
	wait_event_interruptible(wq_head, condition);

	ret = copy_to_user(ubuf, &number, sizeof(number));
	if (ret) {
		printk("copy_to_user failed\n");
		return -EIO;
	}
	condition = 0;
	return 0;
}

struct file_operations fops = {
	.read = mycdev_read,
};

// 中断处理函数
irqreturn_t myirq_handler(int irq, void *dev_id)
{
	number = !number;
	gpiod_set_value(gpiono, number);
  condition = 1;
  wake_up_interruptible(&wq_head);
	return IRQ_HANDLED;
}

static int __init mycdev_init(void)
{
	int ret;
	// 初始化等待队列头
	init_waitqueue_head(&wq_head);
	// 字符设备注册
	major = register_chrdev(0, "mycdev", &fops);
	if (major < 0) {
		printk("register_chrdev failed\n");
		return major;
	}
	// 向上提交目录
	cls = class_create(THIS_MODULE, "mycdev");
	if (IS_ERR(cls)) {
		printk("class_create failed\n");
		return -PTR_ERR(cls);
	}
	// 创建设备文件
	dev = device_create(cls, NULL, MKDEV(major, 0), NULL, "led");
	if (IS_ERR(dev)) {
		printk("device_create failed\n");
		return -PTR_ERR(dev);
	}
	dev = device_create(cls, NULL, MKDEV(major, 1), NULL, "key");
	if (IS_ERR(dev)) {
		printk("device_create failed\n");
		return -PTR_ERR(dev);
	}
	dnode = of_find_node_by_path("/mychrdev");
	if (!dnode) {
		printk("of_find_node_by_path failed\n");
		return -ENXIO;
	}
	irqno = irq_of_parse_and_map(dnode, 0);
	if (!irqno) {
		printk("irq_of_parse_and_map failed\n");
		return -ENXIO;
	}
	ret = request_irq(irqno, myirq_handler, IRQF_TRIGGER_FALLING, "key",
			  NULL);
	if (ret) {
		printk("request_irq failed\n");
		return ret;
	}
	gpiono = gpiod_get_from_of_node(dnode, "led", 0, GPIOD_OUT_LOW, NULL);
	if (IS_ERR(gpiono)) {
		printk("gpiod_get_from_of_node failed\n");
		return -PTR_ERR(gpiono);
	}
	return 0;
}

static void __exit mycdev_exit(void)
{
	// 注销设备文件
	device_destroy(cls, MKDEV(major, 0));
	device_destroy(cls, MKDEV(major, 1));
	// 注销目录
	class_destroy(cls);
	// 注销字符设备
	unregister_chrdev(major, "mycdev");
	// 注销中断号
	free_irq(irqno, NULL);
	// 释放gpio
	gpiod_put(gpiono);
}

module_init(mycdev_init);
module_exit(mycdev_exit);
MODULE_LICENSE("GPL");
