使用新版的 GPIO API

设备树文件同 [gpio/stm32mp157a-fsmp1a.dts](../gpio/stm32mp157a-fsmp1a.dts)

```bash
make arch=arm modname=mychrdev
cp ./mychrdev.ko ~/nfs/rootfs/

cp stm32mp157a-fsmp1a.dts linux/arch/arm/boot/dts/stm32mp157a-fsmp1a.dts
cd linux
make dtbs
cp arch/arm/boot/dts/stm32mp157a-fsmp1a.dtb ~/tftpboot
```

```bash
[root@fsmp1a ]# insmod mychrdev.ko 
[ 1799.711707] 解析设备树节点成功
```

