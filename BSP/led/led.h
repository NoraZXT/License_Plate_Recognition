#ifndef __LED_H_
#define __LED_H_

#define LED_G_GPIO_PORT          GPIOA
#define LED_G_GPIO_PIN           GPIO_Pin_8
#define LED_G_GPIO_CLK_ENABLE()       do{RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); }while(0)
#define LED_G(x)  do{ x ? \
                        GPIO_WriteBit(LED_G_GPIO_PORT, LED_G_GPIO_PIN, Bit_SET): \
                        GPIO_WriteBit(LED_G_GPIO_PORT, LED_G_GPIO_PIN, Bit_RESET); \
                    }while(0)

#define LED_R_GPIO_PORT          GPIOA
#define LED_R_GPIO_PIN           GPIO_Pin_9
#define LED_R_GPIO_CLK_ENABLE()       do{RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); }while(0)

#define LED_R(x)  do{ x ? \
                        GPIO_WriteBit(LED_R_GPIO_PORT, LED_R_GPIO_PIN, Bit_SET): \
                        GPIO_WriteBit(LED_R_GPIO_PORT, LED_R_GPIO_PIN, Bit_RESET); \
                    }while(0)                   
                    

void LED_G_init(void);                    
void LED_R_init(void);                    
#endif
