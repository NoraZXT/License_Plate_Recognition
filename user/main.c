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
u8 now_lic = 0; //当前车牌在数组中的位置
extern u8 time_flag;

typLicensePlate LicensePlate[160] = {0x00},recognition_recognition[100] = {0x00};
u8 PRICE=30;
u8 CNT11 = 0;
u8 in_out=0,pre_in_out = 0;
u8 cal_MODE=0,pre_MODE = 0;

void KEY_handle(void);      //按键处理函数

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
    LCD_Init();//LCD初始化
    LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
    LED_G_init();
    LED_R_init();
    TIM2_Init();   //按键按下时长控制
    key_init();
    uart_init(115200);
    TIM4_Init();   //记录停车场时间
    
    TIM3_PWM_Init(200-1,7200-1);  //舵机
    while(1)
    {
        LAST = lic_sum_num - lic_now_num;
        for(u8 bb = 0; bb < 200; bb ++)
        {
            USART_RX_BUF[bb] = 0x00;
        }
        if(CNT11 == 100)     //识别100次，选择次数最多的车牌，减少误识别
        {
            u8 k = 0,max_num = 0;
            u32 max = 0;
            while(k < 100)    //找到识别出的车牌数组中，识别出次数最多的车牌号
            {
                if(max <= recognition_recognition[k].Time)
                {
                    max = recognition_recognition[k].Time;
                    max_num = k;
                }
                k ++;
                recognition_recognition[k].Time = 0;
            }
            if((!in_out) && LAST > 0)  //若为进入车库
            {
                Enter_garage(max_num, recognition_recognition, LAST);
                
            }
            else     //若为出车库
            {
                Out_of_garage(recognition_recognition, max_num, now_price, LAST);
            }
            CNT11 = 0;
            LCD_Fill(0,60,LCD_W,140,WHITE);
            LCD_ShowChinese(0,60,"识别中",RED,WHITE,16,0);
            LCD_ShowIntNum(48,60,CNT11,3,RED,WHITE,16);
            LCD_ShowString(80, 60, "%", RED, WHITE, 16,0);
            count = 0;
        }
        time_cal(LicensePlate);   //记录车库内不同车牌号的时间
        LAST = lic_sum_num - lic_now_num;     //更新车库内车辆的数量
        if(LAST > 0)        //车位有余量 亮绿灯
        {
            LED_G(1);
            LED_R(0);
        }
        else          //否则，亮红灯，LCD显示车位已满
        {
            LAST = 0;
            LED_G(0);
            LED_R(1);
            if(!in_out)
            {
                LCD_Fill(0,60,LCD_W,100,WHITE);
                LCD_ShowChinese(0,60,"车位已满",RED,WHITE,16,0);            
            }
        }
        
        pre_MODE = cal_MODE;
        pre_in_out = in_out;
        if(!in_out)
        {
            lcd_show(PRICE, LAST);
            LCD_ShowChinese(0, 140, "入口", RED, WHITE, 16,0);
        }
        else
        {
            lcd_show(PRICE,LAST);
            LCD_ShowChinese(0, 140, "出口", RED, WHITE, 16,0);            
        }
        KEY_handle();         

        USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);   //开启串口和串口接收中断
        USART_Cmd(USART1, ENABLE);          
        search_lic(Lic);
        search_province(Lic,Province,Lic_num);
        province_toC(Province,pro_c);
        if(!((pro_c[0] == 0x00) && (pro_c[1] == 0x00)))  //若识别出不为0的车牌号
        {   
            USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);     //关闭串口和串口接收中断
            USART_Cmd(USART1, DISABLE);
//            delay_ms(10);
             
            if((LAST <= 0) && (!in_out))     //若没有剩余车位且当前处于车辆进停车场模式
            {
                LCD_Fill(0,60,LCD_W,140,WHITE); 
                LCD_ShowChinese(0,60,"车位已满",RED,WHITE,16,0);
                LED_G(0);
                for(u8 i = 0; i < 10; i ++)        //红灯闪烁提示摄像头前的车辆
                {
                    LED_R(0);
                    delay_ms(50);
                    LED_R(1);
                    delay_ms(50);                         
                }
                LCD_Fill(0,60,LCD_W,140,WHITE); 
            }
            else    //若车位有剩余
            {
                CNT11 ++;           
                if(CNT11 > 100)    //已检测100次
                {
                    pro_c[0] = 0X00;
                    pro_c[1] = 0x00;
                    CNT11 = 100;
                }
                else             //未检测100次  
                {
                    u8 j = 0;
                    LCD_ShowChinese(0,60,"识别中",RED,WHITE,16,0);
                    LCD_ShowIntNum(48,60,CNT11,3,RED,WHITE,16);
                    LCD_ShowString(80, 60, "%", RED, WHITE, 16,0);
                    __search11:
                    if(j < count + 1)
                    {   //若当前识别的车牌号和已识别的车牌号相同，则识别次数+1
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
                        //否则将识别到的车牌加进数组里
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
        case KEY1_PRESS:   //KEY1短按切换单价加减
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
        case KEY1_LONG_PRESS:    //KEY1长按1s切换总车位加减
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
        case KEY2_PRESS:            //KEY2 短按改变单价
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
        case KEY2_LONG_PRESS:       //KEY2长按 改变总停车位
            if(in_out == 0)
            {
                lic_sum_num++;
                if(lic_sum_num >= 160)     //限制上限不超过160个停车位
                    lic_sum_num = 160;
            }
            else if(in_out == 1)
            {
                lic_sum_num --;                      //限制下限不少于车库内现有总车辆
                if(lic_sum_num <= lic_now_num)
                    lic_sum_num = lic_now_num;
            }
            break;
    }
}
