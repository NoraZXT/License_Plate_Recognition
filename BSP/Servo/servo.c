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

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); // ʹ�ܶ�ʱ��3ʱ��
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE); // ʹ��GPIO�����AFIO���ù���ģ��ʱ��

	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
    // ���ø�����Ϊ�����������, ���TIM3 CH2��PWM���岨�� GPIOA.7
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; // TIM_CH2
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // �����������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure); // ��ʼ��GPIO

    // ��ʼ��TIM3
    TIM_TimeBaseStructure.TIM_Period = arr; // ��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
    TIM_TimeBaseStructure.TIM_Prescaler = psc; // ����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; // ����ʱ�ӷָ�:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM���ϼ���ģʽ
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); // ����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

    // ��ʼ��TIM3 Channel2 PWMģʽ
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; // ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // �Ƚ����ʹ��
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; // �������:TIM����Ƚϼ��Ը�
    TIM_OC2Init(TIM3, &TIM_OCInitStructure); // ����Tָ���Ĳ�����ʼ������TIM3 OC2

    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable); // ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���

    TIM_Cmd(TIM3, ENABLE); // ʹ��TIM3
}
