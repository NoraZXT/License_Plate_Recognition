#ifndef __LICENSE_H
#define __LICENSE_H
#include "stm32f10x.h"

typedef struct 
{
    unsigned char Index[2];	
    unsigned char Msk[7];
    u16 Time;
    u32 price;
    u8  flag;
}typLicensePlate; 

void search_lic(u8 *lic);
void search_province(u8 *lic, u8 *province, u8 *lic_num);
void province_toC(u8 *prov, u8 *prov_C);
void TIM4_Init(void);
void time_cal(typLicensePlate *license_plate);
void lcd_show(u8 price, u8 last);
void Enter_garage(u8 max_num, typLicensePlate *recognition,u8 last_num);
void Out_of_garage(typLicensePlate *recognition, u8 max_num, float N_Price, u8 last_num);
#endif
