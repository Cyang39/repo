1. 将设备树移到 `/linux-5.10.61/arch/arm/boot/dts/stm32mp157a-fsmp1a.dts`
2. 编译设备树 `make dtbs`
3. 拷贝 `/linux-5.10.61/arch/arm/boot/dts/stm32mp157a-fsmp1a.dtb` 到 tftp
4. 编译驱动 `make arch=arm modname=mychrdev`
5. 拷贝 `mychrdev.ko` 到 `nfs/rootfs`
6. 加载驱动 `[fsmp1a >] insmod mychrdev.ko`
7. 按下 KEY1 测试
