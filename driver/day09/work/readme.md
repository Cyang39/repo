使用驱动代码实现如下要求
1. 使用驱动通过阻塞 io 模型读取 number 变量的值
2. number 是内核驱动中的一个变量
3. number 的值随着按键按下而改变（按键中断）
    - 例如 number = 0 按下按键 number = 1，再次按下按键 number = 0
4. 在按下按键的时候需要同时将 led1 的状态取反
5. 驱动中需要编写字符设备驱动
6. 驱动中需要自动创建设备节点
7. 在这个驱动需要的所有设备信息放在设备树的同一个节点中

--- 

1. 将设备树移到 `/linux-5.10.61/arch/arm/boot/dts/stm32mp157a-fsmp1a.dts`
2. 编译设备树 `make dtbs`
3. 拷贝 `/linux-5.10.61/arch/arm/boot/dts/stm32mp157a-fsmp1a.dtb` 到 tftp
4. 编译驱动 `make arch=arm modname=mychrdev`
5. 拷贝 `mychrdev.ko` 到 `nfs/rootfs`
6. 加载驱动 `[fsmp1a >] insmod mychrdev.ko`
7. 按下 KEY1 测试

