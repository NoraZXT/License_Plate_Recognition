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
"皖", 'W', 'a', 'n', 0, 0, 0, 0, 0, 0, 0,
"沪", 'H', 'u', 0, 0, 0, 0, 0, 0, 0, 0,
"津", 'J', 'i', 'n', 0, 0, 0, 0, 0, 0, 0,
"渝", 'Y', 'u', '^', 0, 0, 0, 0, 0, 0, 0,
"冀", 'J', 'i', 0, 0, 0, 0, 0, 0, 0, 0,
"晋", 'S', 'x', 0, 0, 0, 0, 0, 0, 0, 0,
"蒙", 'M', 'e', 'n', 'g', 0, 0, 0, 0, 0, 0,
"辽", 'L', 'i', 'a', 'o', 0, 0, 0, 0, 0, 0,
"吉", 'J', 'l', 0, 0, 0, 0, 0, 0, 0, 0,
"黑", 'H', 'e', 'i', 0, 0, 0, 0, 0, 0, 0,
"苏", 'S', 'u', 0, 0, 0, 0, 0, 0, 0, 0,
"浙", 'Z', 'h', 'e', 0, 0, 0, 0, 0, 0, 0,
"京", 'J', 'i', 'n', 'g', 0, 0, 0, 0, 0, 0,
"闽", 'M', 'i', 'n', 0, 0, 0, 0, 0, 0, 0,
"赣", 'G', 'a', 'n', 0, 0, 0, 0, 0, 0, 0,
"鲁", 'L', 'u', 0, 0, 0, 0, 0, 0, 0, 0,
"豫", 'Y', 'u', 0, 0, 0, 0, 0, 0, 0, 0,
"鄂", 'E', '^', 0, 0, 0, 0, 0, 0, 0, 0,
"湘", 'X', 'i', 'a', 'n', 'g', 0, 0, 0, 0, 0,
"粤", 'Y', 'u', 'e', 0, 0, 0, 0, 0, 0, 0,
"桂", 'G', 'u', 'i', '^', 0, 0, 0, 0, 0, 0,
"琼", 'Q', 'i', 'o', 'n', 'g', 0, 0, 0, 0, 0,
"川", 'C', 'u', 'a', 'n', 0, 0, 0, 0, 0, 0,
"贵", 'G', 'u', 'i', 0, 0, 0, 0, 0, 0, 0,
"云", 'Y', 'u', 'n', 0, 0, 0, 0, 0, 0, 0,
"藏", 'Z', 'a', 'n', 'g', 0, 0, 0, 0, 0, 0,
"陕", 'S', 'h', 'a', 'n', 0, 0, 0, 0, 0, 0,
"甘", 'G', 'a', 'n', '^', 0, 0, 0, 0, 0, 0,
"青", 'Q', 'i', 'n', 'g', 0, 0, 0, 0, 0, 0,
"宁", 'N', 'i', 'n', 'g', 0, 0, 0, 0, 0, 0,
"新", 'X', 'i', 'n', 0, 0, 0, 0, 0, 0, 0,
};


extern typLicensePlate LicensePlate[160];
/******************************************************************************
      函数说明：定时器初始化，设置1s进一次中断
      返回值：  无
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
      函数说明：定时器4中断函数，每1分钟标志位置1
      返回值：  无
******************************************************************************/
u8  time_cnt = 0, time_flag = 0;
void TIM4_IRQHandler(void)
{
    if(TIM_GetFlagStatus(TIM4, TIM_FLAG_Update))
    {
        time_cnt++;
        if(time_cnt >= 60)      //计时1min
        {
            time_flag = 1;      //1min标志位
            time_cnt = 0;
        }
        
    }
    
    TIM_ClearFlag(TIM4, TIM_FLAG_Update);
}
/******************************************************************************
      函数说明：解析串口接收的数据
      入口数据：lic存放解析的车牌
      返回值：  无
******************************************************************************/
void search_lic(u8 *lic)
{
    for(int i = 0;i<USART_REC_LEN;i++)
    {
        //找到数据头“0XA5”
        if(USART_RX_BUF[i] == 0X30 && USART_RX_BUF[i+1] == 0X58 && USART_RX_BUF[i+2] == 0X41 && USART_RX_BUF[i+3] == 0X35)
        {

            for(int j = 0;j<USART_REC_LEN - i - 7;j++)
            {
                //找到数据尾“0X5A”
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
      函数说明：解析车牌号，将接收到的车牌号数组分离出解析省份和剩下的字符数字串两个数组
      入口数据：lic 存放原来的车牌号数组
                province 存放解析出的省份的拼音
                lic_num 存放车牌号的字母和数字部分
      返回值：  无
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
      函数说明：将拼音的省份转换成中文对应的16进制
      入口数据：prov 存放原来的车牌号数组
                prov_C 存放解析出的中文省份的16进制
      返回值：  无
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
      函数说明：对停车场内车辆的计时
      入口数据：flag 1分钟标志位
                license_plate 停车场内车牌号数组
      返回值：  无
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
      函数说明：舵机处理函数，舵机旋转90°，停顿1s后，再次转回
      返回值：  无
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
      函数说明：显示车库内的车辆余量以及单价
      返回值：  无
******************************************************************************/
void lcd_show(u8 price, u8 last)
{
    LCD_ShowChinese(0,0,"单价",RED,WHITE,16,0);
    LCD_ShowString(32,0,":",RED,WHITE,16,0);
    LCD_ShowIntNum(48,0,price,2,RED,WHITE,16);
    LCD_ShowChinese(80,0,"元",RED,WHITE,16,0);
    LCD_ShowString(96,0,"/",RED,WHITE,16,0);
    LCD_ShowChinese(112,0,"时",RED,WHITE,16,0);
    
    LCD_ShowChinese(0,20,"车位余量",RED,WHITE,16,0);
    LCD_ShowString(64,20,":",RED,WHITE,16,0);
    LCD_ShowIntNum(80,20,last,3,RED,WHITE,16);
    
    LCD_ShowChinese(0,40,"车位总量",RED,WHITE,16,0);
    LCD_ShowString(64,40,":",RED,WHITE,16,0);
    LCD_ShowIntNum(80,40,lic_sum_num,3,RED,WHITE,16);
       
}

/******************************************************************************
      函数说明：在进车库模式下，针对识别出的车牌号进行判断，是否原来就在车库内，若不是，找到空位则将车牌号录入，
若无空位，则提示车位已满
      入口数据：max_num 待识别数组中识别次数最多的数组下标
                recognition 待识别数组
                last_num 剩余车位
      返回值：  无
******************************************************************************/
extern u8 lic_now_num;
extern u8 now_lic;
extern u8 PRICE;
void Enter_garage(u8 max_num, typLicensePlate *recognition, u8 last_num)
{
    for(u8 i = 0; i<160; i ++)
    {
        //如果识别的车牌已在车库内的数组
        if((LicensePlate[i].Index[0] == recognition[max_num].Index[0]) && (LicensePlate[i].Index[1] == recognition[max_num].Index[1]) &&\
            (LicensePlate[i].Msk[0] == recognition[max_num].Msk[0]) && (LicensePlate[i].Msk[1] == recognition[max_num].Msk[1]) && \
        (LicensePlate[i].Msk[2] == recognition[max_num].Msk[2]) && (LicensePlate[i].Msk[3] == recognition[max_num].Msk[3]) && \
        (LicensePlate[i].Msk[4] == recognition[max_num].Msk[4]) && (LicensePlate[i].Msk[5] == recognition[max_num].Msk[5]) && \
        (LicensePlate[i].Msk[6] == recognition[max_num].Msk[6]) && (LicensePlate[i].flag == 1))
        {
            lcd_show(PRICE,last_num);
            LCD_Fill(0,60,LCD_W,140,WHITE); 
            LCD_ShowChinese(0,60,"车辆已在车库",RED,WHITE,16,0);
            LCD_ShowChinese(0,80,recognition[max_num].Index,RED,WHITE,16,0); 
            LCD_ShowString(32, 80, recognition[max_num].Msk, RED, WHITE, 16,0);
            delay_ms(1000);
            LCD_Fill(0,60,LCD_W,140,WHITE); 
            goto __search2;
        }
    }                
    for(u8 j = 0;j < 160; j ++)
    {
        //找到数组内的空闲位置，赋值
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
            LicensePlate[j].flag = 1;      //标志位，表示此车牌号在车库内
            lic_now_num ++;
            last_num = lic_sum_num - lic_now_num;
            lcd_show(PRICE, last_num);                        
            now_lic = j;
            LCD_Fill(0,60,LCD_W,140,WHITE); 
            LCD_ShowChinese(0,60,"欢迎光临",RED,WHITE,16,0);
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
            LCD_ShowChinese(0,60,"车位已满",RED,WHITE,16,0);
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
      函数说明：在出车库模式下，针对识别出的车牌号进行检索，是否原来就在车库内，
且车牌在车库内的标志位置1,则表明车牌在车库内，则此时车牌在车库内，获取停车时间和单价，
并且计算总价格。若检索不到车牌则显示车辆不在内。
      入口数据：max_num 待识别数组中识别次数最多的数组下标
                recognition 待识别数组
                N_Price 最新单价
                last_num 剩余车位
      返回值：  无
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
                LCD_ShowChinese(0,60,"谢谢惠顾",RED,WHITE,16,0);
                LCD_ShowChinese(0,80,LicensePlate[kk].Index,RED,WHITE,16,0); 
                LCD_ShowString(32, 80, LicensePlate[kk].Msk, RED, WHITE, 16,0);
                LCD_ShowChinese(0,100,"时长",RED,WHITE,16,0);
                LCD_ShowIntNum(32,100,now_time,5,RED,WHITE,16);
                LCD_ShowChinese(72,100,"分",RED,WHITE,16,0);
                LCD_ShowChinese(0,120,"价格",RED,WHITE,16,0);
                LCD_ShowFloatNum1(32, 120, N_Price, 6, RED, WHITE, 16);
                LCD_ShowChinese(88,120,"元",RED,WHITE,16,0);
                servo_handle();                                                                                                      
                delay_ms(5000);                        
                LCD_Fill(0,60,LCD_W,100,WHITE);                             
                break;
            }
            else if(kk == 160)
            {
                lcd_show(PRICE,last_num);
                LCD_Fill(0,60,LCD_W,140,WHITE); 
                LCD_ShowChinese(0,60,"识别为",RED,WHITE,16,0);
                LCD_ShowChinese(0,80,recognition[max_num].Index,RED,WHITE,16,0); 
                LCD_ShowString(32, 80, recognition[max_num].Msk, RED, WHITE, 16,0);                    
                LCD_ShowChinese(0,100,"车辆不在内",RED,WHITE,16,0);
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
            LCD_ShowChinese(0,60,"识别为",RED,WHITE,16,0);
            LCD_ShowChinese(0,80,recognition[max_num].Index,RED,WHITE,16,0); 
            LCD_ShowString(32, 80, recognition[max_num].Msk, RED, WHITE, 16,0);                    
            LCD_ShowChinese(0,100,"车辆不在内",RED,WHITE,16,0);
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
