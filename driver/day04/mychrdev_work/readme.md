## 题目要求
编写LED灯的驱动，创建三个设备文件，每个设备文件和一个LED灯绑定，当操作这个设备文件时只能控制设备文件对应的这盏灯。

```c
int mycdev_open(struct inode *inode, struct file *file)
{
    unsigned int aaa=MINOR(inode->i_rdev);//得到打开的文件对应的设备号
    file->private_data=(void *)aaa;
    printk("%s:%s:%d\n", __FILE__, __func__, __LINE__);
    return 0;
}
 
 
long mycdev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
   unsigned int aaa=(int)file->private_data;
   switch(aaa)
   {
   case 0:
       //控制LED1的逻辑
       break;   
    case 1:
       //控制LED2的逻辑
       break; 
    case 2:
       //控制LED3的逻辑
       break; 
   }
}
```

