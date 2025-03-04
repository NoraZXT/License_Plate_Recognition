#include "stm32f10x.h"
#include "license.h"
#include "usart.h"	  
#include "delay.h"
#include "lcd_init.h"
#include "lcd.h"
#include "led.h"


typedef struct 
{
	unsigned char Index[2];	
	unsigned char Msk[10];
}typLicenseProv; 

const typLicenseProv lic_prov[ ] = {
"��", 'W', 'a', 'n', 0, 0, 0, 0, 0, 0, 0,
"��", 'H', 'u', 0, 0, 0, 0, 0, 0, 0, 0,
"��", 'J', 'i', 'n', 0, 0, 0, 0, 0, 0, 0,
"��", 'Y', 'u', '^', 0, 0, 0, 0, 0, 0, 0,
"��", 'J', 'i', 0, 0, 0, 0, 0, 0, 0, 0,
"��", 'S', 'x', 0, 0, 0, 0, 0, 0, 0, 0,
"��", 'M', 'e', 'n', 'g', 0, 0, 0, 0, 0, 0,
"��", 'L', 'i', 'a', 'o', 0, 0, 0, 0, 0, 0,
"��", 'J', 'l', 0, 0, 0, 0, 0, 0, 0, 0,
"��", 'H', 'e', 'i', 0, 0, 0, 0, 0, 0, 0,
"��", 'S', 'u', 0, 0, 0, 0, 0, 0, 0, 0,
"��", 'Z', 'h', 'e', 0, 0, 0, 0, 0, 0, 0,
"��", 'J', 'i', 'n', 'g', 0, 0, 0, 0, 0, 0,
"��", 'M', 'i', 'n', 0, 0, 0, 0, 0, 0, 0,
"��", 'G', 'a', 'n', 0, 0, 0, 0, 0, 0, 0,
"³", 'L', 'u', 0, 0, 0, 0, 0, 0, 0, 0,
"ԥ", 'Y', 'u', 0, 0, 0, 0, 0, 0, 0, 0,
"��", 'E', '^', 0, 0, 0, 0, 0, 0, 0, 0,
"��", 'X', 'i', 'a', 'n', 'g', 0, 0, 0, 0, 0,
"��", 'Y', 'u', 'e', 0, 0, 0, 0, 0, 0, 0,
"��", 'G', 'u', 'i', '^', 0, 0, 0, 0, 0, 0,
"��", 'Q', 'i', 'o', 'n', 'g', 0, 0, 0, 0, 0,
"��", 'C', 'u', 'a', 'n', 0, 0, 0, 0, 0, 0,
"��", 'G', 'u', 'i', 0, 0, 0, 0, 0, 0, 0,
"��", 'Y', 'u', 'n', 0, 0, 0, 0, 0, 0, 0,
"��", 'Z', 'a', 'n', 'g', 0, 0, 0, 0, 0, 0,
"��", 'S', 'h', 'a', 'n', 0, 0, 0, 0, 0, 0,
"��", 'G', 'a', 'n', '^', 0, 0, 0, 0, 0, 0,
"��", 'Q', 'i', 'n', 'g', 0, 0, 0, 0, 0, 0,
"��", 'N', 'i', 'n', 'g', 0, 0, 0, 0, 0, 0,
"��", 'X', 'i', 'n', 0, 0, 0, 0, 0, 0, 0,
};


extern typLicensePlate LicensePlate[160];
/******************************************************************************
      ����˵������ʱ����ʼ��������1s��һ���ж�
      ����ֵ��  ��
******************************************************************************/
void TIM4_Init(void)
{
    TIM_TimeBaseInitTypeDef tim4_init_struct;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    
    tim4_init_struct.TIM_Prescaler = 7200 - 1;
    tim4_init_struct.TIM_CounterMode = TIM_CounterMode_Up;
    tim4_init_struct.TIM_Period = 10000 - 1;
    tim4_init_struct.TIM_ClockDivision = TIM_CKD_DIV1;    
    tim4_init_struct.TIM_RepetitionCounter = 0;
    
    TIM_TimeBaseInit(TIM4, &tim4_init_struct);
    TIM_InternalClockConfig(TIM4);
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);    
    TIM_ClearFlag(TIM4, TIM_FLAG_Update);
    

    
    NVIC_InitTypeDef nvic_initstruct;    
    nvic_initstruct.NVIC_IRQChannel = TIM4_IRQn;
    nvic_initstruct.NVIC_IRQChannelPreemptionPriority = 0;
    nvic_initstruct.NVIC_IRQChannelSubPriority = 0;
    nvic_initstruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_initstruct);

    TIM_Cmd(TIM4, ENABLE);
}
/******************************************************************************
      ����˵������ʱ��4�жϺ�����ÿ1���ӱ�־λ��1
      ����ֵ��  ��
******************************************************************************/
u8  time_cnt = 0, time_flag = 0;
void TIM4_IRQHandler(void)
{
    if(TIM_GetFlagStatus(TIM4, TIM_FLAG_Update))
    {
        time_cnt++;
        if(time_cnt >= 60)      //��ʱ1min
        {
            time_flag = 1;      //1min��־λ
            time_cnt = 0;
        }
        
    }
    
    TIM_ClearFlag(TIM4, TIM_FLAG_Update);
}
/******************************************************************************
      ����˵�����������ڽ��յ�����
      ������ݣ�lic��Ž����ĳ���
      ����ֵ��  ��
******************************************************************************/
void search_lic(u8 *lic)
{
    for(int i = 0;i<USART_REC_LEN;i++)
    {
        //�ҵ�����ͷ��0XA5��
        if(USART_RX_BUF[i] == 0X30 && USART_RX_BUF[i+1] == 0X58 && USART_RX_BUF[i+2] == 0X41 && USART_RX_BUF[i+3] == 0X35)
        {

            for(int j = 0;j<USART_REC_LEN - i - 7;j++)
            {
                //�ҵ�����β��0X5A��
                if(USART_RX_BUF[i+j +4] == 0X30 && USART_RX_BUF[i+j+1 +4] == 0X58 && USART_RX_BUF[i+j+2+4] == 0X35 && USART_RX_BUF[i+j+3+4] == 0x41)
                {
                    u8 k=0;
                    while(k<j)
                    {
                        *(lic+k) = USART_RX_BUF[i+4+k];
                        k++;
                    }
                    break;   
                }
            }
            break;
        }
    }     
}
/******************************************************************************
      ����˵�����������ƺţ������յ��ĳ��ƺ�������������ʡ�ݺ�ʣ�µ��ַ����ִ���������
      ������ݣ�lic ���ԭ���ĳ��ƺ�����
                province ��Ž�������ʡ�ݵ�ƴ��
                lic_num ��ų��ƺŵ���ĸ�����ֲ���
      ����ֵ��  ��
******************************************************************************/
void search_province(u8 *lic, u8 *province, u8 *lic_num)
{
    u8 cnt = 0;
    for(int i = 0; i < 20 + 1; i++)
    {
        if(*(lic +  i) != 0x20)
        {
            *(province + i) = *(lic + i);
            cnt++;
        }
        else
            break;
    }
    for(u8 j = cnt+1; j <= 20 + 1; j++)
    {
        if(lic[j] != 0x00)
        {
            lic_num[j - cnt-1] = lic[j];
        }
        else
            break;
    }
}
/******************************************************************************
      ����˵������ƴ����ʡ��ת�������Ķ�Ӧ��16����
      ������ݣ�prov ���ԭ���ĳ��ƺ�����
                prov_C ��Ž�����������ʡ�ݵ�16����
      ����ֵ��  ��
******************************************************************************/
void province_toC(u8 *prov, u8 *prov_C)
{
    for(u8 i = 0;i < 30; i++)
    {
        if((lic_prov[i].Msk[0] == *(prov)) && (lic_prov[i].Msk[1] == *(prov + 1)) && \

        (lic_prov[i].Msk[2] == *(prov + 2)) && (lic_prov[i].Msk[3] == *(prov + 3))&&\
        (lic_prov[i].Msk[4] == *(prov + 4)) && (lic_prov[i].Msk[5] == *(prov + 5))&&\
        (lic_prov[i].Msk[6] == *(prov + 6)) && (lic_prov[i].Msk[7] == *(prov +7))&&\
        (lic_prov[i].Msk[8] == *(prov + 8)) && (lic_prov[i].Msk[9] == *(prov + 9)))
        {
            *prov_C = lic_prov[i].Index[0];
            *(prov_C + 1) = lic_prov[i].Index[1];
        }
    }
}
/******************************************************************************
      ����˵������ͣ�����ڳ����ļ�ʱ
      ������ݣ�flag 1���ӱ�־λ
                license_plate ͣ�����ڳ��ƺ�����
      ����ֵ��  ��
******************************************************************************/
extern u8 lic_sum_num;
void time_cal(typLicensePlate *license_plate)
{
    if(time_flag)
    {
        time_flag = 0;
        for(u8 i = 0; i < lic_sum_num; i ++)
        {
            if(license_plate[i].flag == 1)
            {
                license_plate[i].Time ++;
            }
        }
    }
}
/******************************************************************************
      ����˵��������������������ת90�㣬ͣ��1s���ٴ�ת��
      ����ֵ��  ��
******************************************************************************/
void servo_handle()
{
    TIM_SetCompare2(TIM3, 5);
    delay_ms(320);
    TIM_SetCompare2(TIM3, 15);
    delay_ms(1000);
    TIM_SetCompare2(TIM3, 25);
    delay_ms(300); 
    TIM_SetCompare2(TIM3, 15);    
    delay_ms(1000);
}
/******************************************************************************
      ����˵������ʾ�����ڵĳ��������Լ�����
      ����ֵ��  ��
******************************************************************************/
void lcd_show(u8 price, u8 last)
{
    LCD_ShowChinese(0,0,"����",RED,WHITE,16,0);
    LCD_ShowString(32,0,":",RED,WHITE,16,0);
    LCD_ShowIntNum(48,0,price,2,RED,WHITE,16);
    LCD_ShowChinese(80,0,"Ԫ",RED,WHITE,16,0);
    LCD_ShowString(96,0,"/",RED,WHITE,16,0);
    LCD_ShowChinese(112,0,"ʱ",RED,WHITE,16,0);
    
    LCD_ShowChinese(0,20,"��λ����",RED,WHITE,16,0);
    LCD_ShowString(64,20,":",RED,WHITE,16,0);
    LCD_ShowIntNum(80,20,last,3,RED,WHITE,16);
    
    LCD_ShowChinese(0,40,"��λ����",RED,WHITE,16,0);
    LCD_ShowString(64,40,":",RED,WHITE,16,0);
    LCD_ShowIntNum(80,40,lic_sum_num,3,RED,WHITE,16);
       
}

/******************************************************************************
      ����˵�����ڽ�����ģʽ�£����ʶ����ĳ��ƺŽ����жϣ��Ƿ�ԭ�����ڳ����ڣ������ǣ��ҵ���λ�򽫳��ƺ�¼�룬
���޿�λ������ʾ��λ����
      ������ݣ�max_num ��ʶ��������ʶ��������������±�
                recognition ��ʶ������
                last_num ʣ�೵λ
      ����ֵ��  ��
******************************************************************************/
extern u8 lic_now_num;
extern u8 now_lic;
extern u8 PRICE;
void Enter_garage(u8 max_num, typLicensePlate *recognition, u8 last_num)
{
    for(u8 i = 0; i<160; i ++)
    {
        //���ʶ��ĳ������ڳ����ڵ�����
        if((LicensePlate[i].Index[0] == recognition[max_num].Index[0]) && (LicensePlate[i].Index[1] == recognition[max_num].Index[1]) &&\
            (LicensePlate[i].Msk[0] == recognition[max_num].Msk[0]) && (LicensePlate[i].Msk[1] == recognition[max_num].Msk[1]) && \
        (LicensePlate[i].Msk[2] == recognition[max_num].Msk[2]) && (LicensePlate[i].Msk[3] == recognition[max_num].Msk[3]) && \
        (LicensePlate[i].Msk[4] == recognition[max_num].Msk[4]) && (LicensePlate[i].Msk[5] == recognition[max_num].Msk[5]) && \
        (LicensePlate[i].Msk[6] == recognition[max_num].Msk[6]) && (LicensePlate[i].flag == 1))
        {
            lcd_show(PRICE,last_num);
            LCD_Fill(0,60,LCD_W,140,WHITE); 
            LCD_ShowChinese(0,60,"�������ڳ���",RED,WHITE,16,0);
            LCD_ShowChinese(0,80,recognition[max_num].Index,RED,WHITE,16,0); 
            LCD_ShowString(32, 80, recognition[max_num].Msk, RED, WHITE, 16,0);
            delay_ms(1000);
            LCD_Fill(0,60,LCD_W,140,WHITE); 
            goto __search2;
        }
    }                
    for(u8 j = 0;j < 160; j ++)
    {
        //�ҵ������ڵĿ���λ�ã���ֵ
        if(LicensePlate[j].flag == 0)
        {
            LicensePlate[j].Index[0] = recognition[max_num].Index[0];
            LicensePlate[j].Index[1] = recognition[max_num].Index[1];
            LicensePlate[j].Msk[0] = recognition[max_num].Msk[0];
            LicensePlate[j].Msk[1] = recognition[max_num].Msk[1];
            LicensePlate[j].Msk[2] = recognition[max_num].Msk[2];
            LicensePlate[j].Msk[3] = recognition[max_num].Msk[3];
            LicensePlate[j].Msk[4] = recognition[max_num].Msk[4];
            LicensePlate[j].Msk[5] = recognition[max_num].Msk[5];
            LicensePlate[j].Msk[6] = recognition[max_num].Msk[6];
            LicensePlate[j].flag = 1;      //��־λ����ʾ�˳��ƺ��ڳ�����
            lic_now_num ++;
            last_num = lic_sum_num - lic_now_num;
            lcd_show(PRICE, last_num);                        
            now_lic = j;
            LCD_Fill(0,60,LCD_W,140,WHITE); 
            LCD_ShowChinese(0,60,"��ӭ����",RED,WHITE,16,0);
            LCD_ShowChinese(0,80,LicensePlate[now_lic].Index,RED,WHITE,16,0); 
            LCD_ShowString(32, 80, LicensePlate[now_lic].Msk, RED, WHITE, 16,0);
            servo_handle();
            delay_ms(5000);                        
            LCD_Fill(0,60,LCD_W,140,WHITE); 
            break;
        }
        //
        else if((j == 160) && (LicensePlate[j].flag != 0))
        {
            LCD_Fill(0,60,LCD_W,100,WHITE); 
            LCD_ShowChinese(0,60,"��λ����",RED,WHITE,16,0);
            LED_G(0);
            for(u8 i = 0; i < 10; i ++)
            {
                LED_R(0);
                delay_ms(250);
                LED_R(1);
                delay_ms(250);                         
            }

        }
    }
    __search2:
    max_num = 0;
    
}

/******************************************************************************
      ����˵�����ڳ�����ģʽ�£����ʶ����ĳ��ƺŽ��м������Ƿ�ԭ�����ڳ����ڣ�
�ҳ����ڳ����ڵı�־λ��1,����������ڳ����ڣ����ʱ�����ڳ����ڣ���ȡͣ��ʱ��͵��ۣ�
���Ҽ����ܼ۸�������������������ʾ���������ڡ�
      ������ݣ�max_num ��ʶ��������ʶ��������������±�
                recognition ��ʶ������
                N_Price ���µ���
                last_num ʣ�೵λ
      ����ֵ��  ��
******************************************************************************/
u16 now_time;
void Out_of_garage(typLicensePlate *recognition, u8 max_num, float N_Price, u8 last_num)
{
    u8 kk  = 0;
    while(kk <= 160)
    {
        if((LicensePlate[kk].flag == 1) && (kk < 160))
        {
            if((LicensePlate[kk].Index[0] == recognition[max_num].Index[0]) && (LicensePlate[kk].Index[1] == recognition[max_num].Index[1])\
                && (LicensePlate[kk].Msk[0] == recognition[max_num].Msk[0]) && (LicensePlate[kk].Msk[1] == recognition[max_num].Msk[1])\
            && (LicensePlate[kk].Msk[2] == recognition[max_num].Msk[2]) && (LicensePlate[kk].Msk[3] == recognition[max_num].Msk[3])\
            && (LicensePlate[kk].Msk[4] == recognition[max_num].Msk[4]) && (LicensePlate[kk].Msk[5] == recognition[max_num].Msk[5])\
            && (LicensePlate[kk].Msk[6] == recognition[max_num].Msk[6]))
            {   
                 
                now_time = LicensePlate[kk].Time;            
                N_Price = PRICE * now_time/60.f;
                LicensePlate[kk].flag = 0;
                LicensePlate[kk].Time = 0;
                lic_now_num --;
                last_num = lic_sum_num - lic_now_num;
                lcd_show(PRICE, last_num);                             
                LCD_Fill(0,60,LCD_W,140,WHITE);
                LCD_ShowChinese(0,60,"лл�ݹ�",RED,WHITE,16,0);
                LCD_ShowChinese(0,80,LicensePlate[kk].Index,RED,WHITE,16,0); 
                LCD_ShowString(32, 80, LicensePlate[kk].Msk, RED, WHITE, 16,0);
                LCD_ShowChinese(0,100,"ʱ��",RED,WHITE,16,0);
                LCD_ShowIntNum(32,100,now_time,5,RED,WHITE,16);
                LCD_ShowChinese(72,100,"��",RED,WHITE,16,0);
                LCD_ShowChinese(0,120,"�۸�",RED,WHITE,16,0);
                LCD_ShowFloatNum1(32, 120, N_Price, 6, RED, WHITE, 16);
                LCD_ShowChinese(88,120,"Ԫ",RED,WHITE,16,0);
                servo_handle();                                                                                                      
                delay_ms(5000);                        
                LCD_Fill(0,60,LCD_W,100,WHITE);                             
                break;
            }
            else if(kk == 160)
            {
                lcd_show(PRICE,last_num);
                LCD_Fill(0,60,LCD_W,140,WHITE); 
                LCD_ShowChinese(0,60,"ʶ��Ϊ",RED,WHITE,16,0);
                LCD_ShowChinese(0,80,recognition[max_num].Index,RED,WHITE,16,0); 
                LCD_ShowString(32, 80, recognition[max_num].Msk, RED, WHITE, 16,0);                    
                LCD_ShowChinese(0,100,"����������",RED,WHITE,16,0);
                delay_ms(5000);
                LCD_Fill(0,60,LCD_W,140,WHITE);
                break;
            }
                
                kk ++;
        }
        else if(kk == 160)
        {
            lcd_show(PRICE,last_num);
            LCD_Fill(0,60,LCD_W,140,WHITE); 
            LCD_ShowChinese(0,60,"ʶ��Ϊ",RED,WHITE,16,0);
            LCD_ShowChinese(0,80,recognition[max_num].Index,RED,WHITE,16,0); 
            LCD_ShowString(32, 80, recognition[max_num].Msk, RED, WHITE, 16,0);                    
            LCD_ShowChinese(0,100,"����������",RED,WHITE,16,0);
            delay_ms(5000);
            LCD_Fill(0,60,LCD_W,140,WHITE);                        
            break;
        }
        else
        {
            lcd_show(PRICE,last_num);
            kk ++;                        
        }

    }
    max_num = 0;
}
