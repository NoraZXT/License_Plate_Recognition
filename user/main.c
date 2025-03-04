#include "stm32f10x.h"
#include "delay.h"
#include "lcd_init.h"
#include "lcd.h"

#include "servo.h"
#include "led.h"
#include "key.h"
#include "usart.h" 
#include "license.h"

u8 lic_sum_num = 3;
u8 lic_now_num = 0;
u8 now_lic = 0; //��ǰ�����������е�λ��
extern u8 time_flag;

typLicensePlate LicensePlate[160] = {0x00},recognition_recognition[100] = {0x00};
u8 PRICE=30;
u8 CNT11 = 0;
u8 in_out=0,pre_in_out = 0;
u8 cal_MODE=0,pre_MODE = 0;

void KEY_handle(void);      //����������

int main(void)
{
    float now_price = 0;    
    
    u8 LAST = 0;
    u8 Province[10]={0x00};
    u8 Lic[20]={0x00};
    u8 Lic_num[7] = {0x00};
    u8 pro_c[2];
    u8  count = 0;
    delay_init();
    LCD_Init();//LCD��ʼ��
    LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
    LED_G_init();
    LED_R_init();
    TIM2_Init();   //��������ʱ������
    key_init();
    uart_init(115200);
    TIM4_Init();   //��¼ͣ����ʱ��
    
    TIM3_PWM_Init(200-1,7200-1);  //���
    while(1)
    {
        LAST = lic_sum_num - lic_now_num;
        for(u8 bb = 0; bb < 200; bb ++)
        {
            USART_RX_BUF[bb] = 0x00;
        }
        if(CNT11 == 100)     //ʶ��100�Σ�ѡ��������ĳ��ƣ�������ʶ��
        {
            u8 k = 0,max_num = 0;
            u32 max = 0;
            while(k < 100)    //�ҵ�ʶ����ĳ��������У�ʶ����������ĳ��ƺ�
            {
                if(max <= recognition_recognition[k].Time)
                {
                    max = recognition_recognition[k].Time;
                    max_num = k;
                }
                k ++;
                recognition_recognition[k].Time = 0;
            }
            if((!in_out) && LAST > 0)  //��Ϊ���복��
            {
                Enter_garage(max_num, recognition_recognition, LAST);
                
            }
            else     //��Ϊ������
            {
                Out_of_garage(recognition_recognition, max_num, now_price, LAST);
            }
            CNT11 = 0;
            LCD_Fill(0,60,LCD_W,140,WHITE);
            LCD_ShowChinese(0,60,"ʶ����",RED,WHITE,16,0);
            LCD_ShowIntNum(48,60,CNT11,3,RED,WHITE,16);
            LCD_ShowString(80, 60, "%", RED, WHITE, 16,0);
            count = 0;
        }
        time_cal(LicensePlate);   //��¼�����ڲ�ͬ���ƺŵ�ʱ��
        LAST = lic_sum_num - lic_now_num;     //���³����ڳ���������
        if(LAST > 0)        //��λ������ ���̵�
        {
            LED_G(1);
            LED_R(0);
        }
        else          //��������ƣ�LCD��ʾ��λ����
        {
            LAST = 0;
            LED_G(0);
            LED_R(1);
            if(!in_out)
            {
                LCD_Fill(0,60,LCD_W,100,WHITE);
                LCD_ShowChinese(0,60,"��λ����",RED,WHITE,16,0);            
            }
        }
        
        pre_MODE = cal_MODE;
        pre_in_out = in_out;
        if(!in_out)
        {
            lcd_show(PRICE, LAST);
            LCD_ShowChinese(0, 140, "���", RED, WHITE, 16,0);
        }
        else
        {
            lcd_show(PRICE,LAST);
            LCD_ShowChinese(0, 140, "����", RED, WHITE, 16,0);            
        }
        KEY_handle();         

        USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);   //�������ںʹ��ڽ����ж�
        USART_Cmd(USART1, ENABLE);          
        search_lic(Lic);
        search_province(Lic,Province,Lic_num);
        province_toC(Province,pro_c);
        if(!((pro_c[0] == 0x00) && (pro_c[1] == 0x00)))  //��ʶ�����Ϊ0�ĳ��ƺ�
        {   
            USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);     //�رմ��ںʹ��ڽ����ж�
            USART_Cmd(USART1, DISABLE);
//            delay_ms(10);
             
            if((LAST <= 0) && (!in_out))     //��û��ʣ�೵λ�ҵ�ǰ���ڳ�����ͣ����ģʽ
            {
                LCD_Fill(0,60,LCD_W,140,WHITE); 
                LCD_ShowChinese(0,60,"��λ����",RED,WHITE,16,0);
                LED_G(0);
                for(u8 i = 0; i < 10; i ++)        //�����˸��ʾ����ͷǰ�ĳ���
                {
                    LED_R(0);
                    delay_ms(50);
                    LED_R(1);
                    delay_ms(50);                         
                }
                LCD_Fill(0,60,LCD_W,140,WHITE); 
            }
            else    //����λ��ʣ��
            {
                CNT11 ++;           
                if(CNT11 > 100)    //�Ѽ��100��
                {
                    pro_c[0] = 0X00;
                    pro_c[1] = 0x00;
                    CNT11 = 100;
                }
                else             //δ���100��  
                {
                    u8 j = 0;
                    LCD_ShowChinese(0,60,"ʶ����",RED,WHITE,16,0);
                    LCD_ShowIntNum(48,60,CNT11,3,RED,WHITE,16);
                    LCD_ShowString(80, 60, "%", RED, WHITE, 16,0);
                    __search11:
                    if(j < count + 1)
                    {   //����ǰʶ��ĳ��ƺź���ʶ��ĳ��ƺ���ͬ����ʶ�����+1
                        if((recognition_recognition[j].Index[0] == pro_c[0]) && (recognition_recognition[j].Index[1] == pro_c[1])\
                            && (recognition_recognition[j].Msk[0] == Lic_num[0]) && (recognition_recognition[j].Msk[1] == Lic_num[1])\
                        && (recognition_recognition[j].Msk[2] == Lic_num[2]) && (recognition_recognition[j].Msk[3] == Lic_num[3])\
                        && (recognition_recognition[j].Msk[4] == Lic_num[4]) && (recognition_recognition[j].Msk[5] == Lic_num[5])\
                        && (recognition_recognition[j].Msk[6] == Lic_num[6]))
                        {
                            recognition_recognition[j].Time++;
                        }
                        else
                        {
                            j++;
                            goto __search11;
                        }
                    }
                    else
                    {
                        //����ʶ�𵽵ĳ��Ƽӽ�������
                            recognition_recognition[count].Index[0] = pro_c[0];
                            recognition_recognition[count].Index[1] = pro_c[1];
                            recognition_recognition[count].Msk[0] = Lic_num[0];
                            recognition_recognition[count].Msk[1] = Lic_num[1];
                            recognition_recognition[count].Msk[2] = Lic_num[2];
                            recognition_recognition[count].Msk[3] = Lic_num[3];
                            recognition_recognition[count].Msk[4] = Lic_num[4];
                            recognition_recognition[count].Msk[5] = Lic_num[5];
                            recognition_recognition[count].Msk[6] = Lic_num[6];
                            recognition_recognition[count].Time = 1;
                            count ++;                
                    }                    
                }            
            }                    
        }
        pro_c[0] = 0X00;
        pro_c[1] = 0x00;             
        for(int i = 0; i < 20; i++)
        {
            Lic[i] = 0x00;
            Province[i] = 0x00;
        }
        for(u8 bb = 0; bb < 200; bb ++)
        {
            USART_RX_BUF[bb] = 0x00;
        }         
    }    
}

void KEY_handle(void)
{
    switch(scan_key())
    {
        case KEY1_PRESS:   //KEY1�̰��л����ۼӼ�
            if(pre_MODE == 0)
            {
                cal_MODE = 1;
            }
            else if(pre_MODE == 1)
            {
                cal_MODE = 0;
            }
            pre_MODE = cal_MODE;
            break;
        case KEY1_LONG_PRESS:    //KEY1����1s�л��ܳ�λ�Ӽ�
            if(pre_in_out == 0)
            {
                in_out = 1;   //OUT
            }
            else if(pre_in_out == 1)
            {
                in_out = 0;       //IN
            }
            pre_in_out = in_out;
            break;
        case KEY2_PRESS:            //KEY2 �̰��ı䵥��
            if(cal_MODE == 0)
            {
                PRICE ++;     
            }
            else if(cal_MODE == 1)
            {
                PRICE --;
                if(PRICE<=0)
                    PRICE = 0;
            }
            break;
        case KEY2_LONG_PRESS:       //KEY2���� �ı���ͣ��λ
            if(in_out == 0)
            {
                lic_sum_num++;
                if(lic_sum_num >= 160)     //�������޲�����160��ͣ��λ
                    lic_sum_num = 160;
            }
            else if(in_out == 1)
            {
                lic_sum_num --;                      //�������޲����ڳ����������ܳ���
                if(lic_sum_num <= lic_now_num)
                    lic_sum_num = lic_now_num;
            }
            break;
    }
}
