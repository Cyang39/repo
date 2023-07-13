platform 总线

```bash
cyang in 🌐 vmware in driver/day10/platform on  driver [?] via C v12.2.0-gcc
❯ sudo insmod pdev.ko

cyang in 🌐 vmware in driver/day10/platform on  driver [?] via C v12.2.0-gcc
❯ sudo dmesg

cyang in 🌐 vmware in driver/day10/platform on  driver [?] via C v12.2.0-gcc took 2s
❯ sudo insmod pdrv.ko

cyang in 🌐 vmware in driver/day10/platform on  driver [?] via C v12.2.0-gcc
❯ sudo dmesg
[39125.953177] /home/cyang/FSMP1A/linux-stm32mp-5.10.61-stm32mp-r2-r0/linux-5.10.61/repo/driver/day10/platform/pdrv.c:pdrv_probe:8

cyang in 🌐 vmware in driver/day10/platform on  driver [?] via C v12.2.0-gcc
❯ sudo rmmod pdev

cyang in 🌐 vmware in driver/day10/platform on  driver [?] via C v12.2.0-gcc
❯ sudo dmesg
[39125.953177] /home/cyang/FSMP1A/linux-stm32mp-5.10.61-stm32mp-r2-r0/linux-5.10.61/repo/driver/day10/platform/pdrv.c:pdrv_probe:8
[39168.285724] /home/cyang/FSMP1A/linux-stm32mp-5.10.61-stm32mp-r2-r0/linux-5.10.61/repo/driver/day10/platform/pdrv.c:pdrv_remove:15
[39168.285898] /home/cyang/FSMP1A/linux-stm32mp-5.10.61-stm32mp-r2-r0/linux-5.10.61/repo/driver/day10/platform/pdev.c:pdev_release:8 
```
