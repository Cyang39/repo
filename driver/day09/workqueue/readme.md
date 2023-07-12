- 使用 [key_interrupt/stm32mp157a-fsmp1a.dts](../key_interrupt/stm32mp157a-fsmp1a.dts) 设备树
- 按下按键 1 和 2 可能出现 3，这是按键抖动，正常现象

1. 将设备树移到 `/linux-5.10.61/arch/arm/boot/dts/stm32mp157a-fsmp1a.dts`
2. 编译设备树 `make dtbs`
3. 拷贝 `/linux-5.10.61/arch/arm/boot/dts/stm32mp157a-fsmp1a.dtb` 到 tftp
4. 编译驱动 `make arch=arm modname=mychrdev`
5. 拷贝 `mychrdev.ko` 到 `nfs/rootfs`
6. 加载驱动 `[fsmp1a >] insmod mychrdev.ko`
7. 按下 KEY1/2/3 测试
