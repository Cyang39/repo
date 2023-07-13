#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/mod_devicetable.h>
#include <linux/gpio.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>
#include <linux/interrupt.h>

int major;
struct class *cls;
struct device *dev;

struct resource *res;
struct device_node *dnode;
unsigned int irqno;
struct gpio_desc *gpiono;

/*
  myplatform{
    compatible = "mahiro,myplatform";
    reg=<0X12345678 0X400>;
    interrupt-parent=<&gpiof>;
    interrupts=<9 0>;
    led1=<&gpioe 10 0>;
  };
*/

int number = 0;
unsigned int condition = 0;
wait_queue_head_t wq_head;

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

irqreturn_t myirq_handler(int irq, void *dev_id)
{
	number = !number;
	gpiod_set_value(gpiono, number);
  condition = 1;
  wake_up_interruptible(&wq_head);
	return IRQ_HANDLED;
}

//probe函数，匹配设备成功执行
int pdrv_probe(struct platform_device *pdev)
{
  int ret;
	printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
  //初始化等待队列头
  init_waitqueue_head(&wq_head);
  //注册字符设备驱动
  major = register_chrdev(0, "mycdev", &fops);
  if (major < 0) {
    printk("register_chrdev failed\n");
    return major;
  }
  //创建设备类
  cls = class_create(THIS_MODULE, "mycdev");
  if (IS_ERR(cls)) {
    printk("class_create failed\n");
    return PTR_ERR(cls);
  }
  //创建设备文件
  dev = device_create(cls, NULL, MKDEV(major, 0), NULL, "led1");
  if (IS_ERR(dev)) {
    printk("device_create failed\n");
    return PTR_ERR(dev);
  }
	//获取设备信息
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (res == NULL) {
		printk("获取资源失败\n");
		return -ENXIO;
	}
	printk("获取资源信息成功 %x\n", res->start);
	irqno = platform_get_irq(pdev, 0);
	if (irqno < 0) {
		printk("获取中断资源失败\n");
		return irqno;
	}
	printk("中断类型资源为%d\n", irqno);
  dnode = of_find_node_by_path("/myplatform");
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
	//获取gpio信息
	//pdev->dev.of_node设备树匹配之后会把
	//设备树节点结构体首地址赋值给dev的of_node成员
	gpiono = gpiod_get_from_of_node(pdev->dev.of_node, "led1", 0,
					GPIOD_OUT_LOW, NULL);
	if (IS_ERR(gpiono)) {
		printk("解析GPIO信息失败\n");
		return -PTR_ERR(gpiono);
	}
	return 0;
}

//remove 设备和驱动分离时执行
int pdrv_remove(struct platform_device *pdev)
{
	printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
  device_destroy(cls, MKDEV(major, 0));
  class_destroy(cls);
  unregister_chrdev(major, "mycdev");
	//释放gpio信息
	gpiod_put(gpiono);
  //释放中断资源
  free_irq(irqno, NULL);
	return 0;
}

//构建设备树匹配的表
struct of_device_id oftable[] = {
	{
		.compatible = "mahiro,myplatform",
	},
	{},
};

struct platform_driver pdrv={
  .probe=pdrv_probe,
  .remove=pdrv_remove,
  .driver={
    .name="aaaaa", 
    .of_match_table=oftable,//设置设备树匹配
  },
};

//一键注册宏
module_platform_driver(pdrv);
MODULE_LICENSE("GPL");
