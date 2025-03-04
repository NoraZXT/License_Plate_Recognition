#include "stm32f10x.h"
#include "servo.h"

void TIM3_Init(void)
{
    TIM_TimeBaseInitTypeDef tim3_init_struct;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    
    tim3_init_struct.TIM_Prescaler = 7200 - 1;
    tim3_init_struct.TIM_CounterMode = TIM_CounterMode_Up;
    tim3_init_struct.TIM_Period = 199;
    tim3_init_struct.TIM_ClockDivision = TIM_CKD_DIV1;    
    tim3_init_struct.TIM_RepetitionCounter = 0;
    
    TIM_TimeBaseInit(TIM3, &tim3_init_struct);
    TIM_InternalClockConfig(TIM3);
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);    
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);
    

    
    NVIC_InitTypeDef nvic_initstruct;    
//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    nvic_initstruct.NVIC_IRQChannel = TIM3_IRQn;
    nvic_initstruct.NVIC_IRQChannelPreemptionPriority = 3;
    nvic_initstruct.NVIC_IRQChannelSubPriority = 0;
    nvic_initstruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_initstruct);

    TIM_Cmd(TIM3, ENABLE);
}

void TIM3_IRQHandler(void)
{
    if(TIM_GetFlagStatus(TIM3, TIM_FLAG_Update))
    {  
        TIM_ClearFlag(TIM3, TIM_FLAG_Update);
//        timeCount = 1;
    }
    
}

void TIM3_PWM_Init(u16 arr, u16 psc)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); // 使能定时器3时钟
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE); // 使能GPIO外设和AFIO复用功能模块时钟

	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
    // 设置该引脚为复用输出功能, 输出TIM3 CH2的PWM脉冲波形 GPIOA.7
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; // TIM_CH2
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure); // 初始化GPIO

    // 初始化TIM3
    TIM_TimeBaseStructure.TIM_Period = arr; // 设置在下一个更新事件装入活动的自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TIM_Prescaler = psc; // 设置用来作为TIMx时钟频率除数的预分频值
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; // 设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM向上计数模式
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); // 根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

    // 初始化TIM3 Channel2 PWM模式
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; // 选择定时器模式:TIM脉冲宽度调制模式2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 比较输出使能
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; // 输出极性:TIM输出比较极性高
    TIM_OC2Init(TIM3, &TIM_OCInitStructure); // 根据T指定的参数初始化外设TIM3 OC2

    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable); // 使能TIM3在CCR2上的预装载寄存器

    TIM_Cmd(TIM3, ENABLE); // 使能TIM3
}
