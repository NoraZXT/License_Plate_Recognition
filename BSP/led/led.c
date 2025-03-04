#include "led.h"
#include "stm32f10x.h"

void LED_G_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    
    LED_G_GPIO_CLK_ENABLE();
    
    gpio_init_struct.GPIO_Pin = LED_G_GPIO_PIN;
    gpio_init_struct.GPIO_Mode =GPIO_Mode_Out_PP;
    gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LED_G_GPIO_PORT, &gpio_init_struct);
    LED_G(0);
}

void LED_R_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    
    LED_R_GPIO_CLK_ENABLE();
    
    gpio_init_struct.GPIO_Pin = LED_R_GPIO_PIN;
    gpio_init_struct.GPIO_Mode =GPIO_Mode_Out_PP;
    gpio_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LED_R_GPIO_PORT, &gpio_init_struct);
    LED_R(0);
}
