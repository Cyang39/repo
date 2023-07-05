#ifndef __HEAD_H__
#define __HEAD_H__
typedef struct {
	unsigned int MODER;
	unsigned int OTYPER;
	unsigned int OSPEEDR;

	unsigned int PUPDR;
	unsigned int IDR;
	unsigned int ODR;
} gpio_t;
#define PHY_LED1_ADDR 0X50006000
#define PHY_LED2_ADDR 0X50007000
#define PHY_LED3_ADDR 0X50006000
#define PHY_RCC_ADDR 0X50000A28

#define LED_ON _IO('l', 1) //开灯
#define LED_OFF _IO('l', 0) //关灯

#endif
