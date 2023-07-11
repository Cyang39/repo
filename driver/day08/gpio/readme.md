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
[   29.684603] 解析设备树节点成功
[   29.687274] 解析gpio编号成功gpiono=74
```

