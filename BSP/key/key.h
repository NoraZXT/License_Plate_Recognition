#ifndef __KEY_H_
#define __KEY_H_

#define KEY2_GPIO_PORT          GPIOB
#define KEY2_GPIO_PIN           GPIO_Pin_1
#define KEY2_IRQHandler         EXTI1_IRQHandler               
#define KEY2_STATUS             GPIO_ReadInputDataBit(KEY1_GPIO_PORT, KEY2_GPIO_PIN)


#define KEY1_GPIO_PORT          GPIOB
#define KEY1_GPIO_PIN           GPIO_Pin_0
#define KEY1_IRQHandler         EXTI0_IRQHandler
#define KEY1_STATUS             GPIO_ReadInputDataBit(KEY2_GPIO_PORT, KEY1_GPIO_PIN)


#define KEY1_PRESS                  1
#define KEY1_LONG_PRESS             2
#define KEY2_PRESS                  3
#define KEY2_LONG_PRESS             4

typedef struct
{
    unsigned char short_press;
    unsigned char long_press;
}key_val_typedef;

void TIM2_Init(void);
void key_init(void);
unsigned char scan_key(void);

#endif

