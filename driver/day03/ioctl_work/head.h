#ifndef __HEAD_H__
#define __HEAD_H__

#include "rcc.h"
#include "gpio.h"

#define LED_ON _IOW('l', 1, int) //开灯
#define LED_OFF _IOW('l', 0, int) //关灯

#define FAN_ON _IO('f', 0)
#define FAN_OFF _IO('f', 1)

#define BEE_ON _IO('b', 0)
#define BEE_OFF _IO('b', 1)

#define MOTOR_ON _IO('m', 0)
#define MOTOR_OFF _IO('m', 1)
#endif
