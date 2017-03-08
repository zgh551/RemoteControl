/*********************
 SCLK	 ->PA0
 DIN   ->PC3
 DC    ->PA1
 CS    ->PA2
*/
#ifndef OLED_H
#define OLED_H

#include "stm32f10x.h"
/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

//CS
#define OLED_CS_CLR()		GPIO_ResetBits(GPIOA,GPIO_Pin_2)
#define OLED_CS_SET()		GPIO_SetBits(GPIOA,GPIO_Pin_2)
//DC
#define OLED_DC_CLR()		GPIO_ResetBits(GPIOA,GPIO_Pin_1)
#define OLED_DC_SET()		GPIO_SetBits(GPIOA,GPIO_Pin_1)

#define OLED_CLK_CLR()		GPIO_ResetBits(GPIOA,GPIO_Pin_0)
#define OLED_CLK_SET()		GPIO_SetBits(GPIOA,GPIO_Pin_0)

#define OLED_DIN_CLR()		GPIO_ResetBits(GPIOC,GPIO_Pin_3)
#define OLED_DIN_SET()		GPIO_SetBits(GPIOC,GPIO_Pin_3)

#define OLED_CMD  	0		//写命令
#define OLED_DATA 	1		//写数据




//OLED控制用函数
void OLED_Init(void);

void OLED_WR_Byte(u8 dat,u8 cmd);	    
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Refresh_Gram(void);		   
void OLED_Clear(void);	


void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_Line(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void OLED_Rectangle(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode);
void OLED_ShowNum(u8 x,u8 y,u16 num,u8 len,u8 size,u8 mode);
void OLED_ShowFloatNum(u8 x,u8 y,float num_f,u8 integer_len,u8 decimals_len,u8 size,u8 mode);
void OLED_ShowString(u8 x,u8 y,const u8 *p,u8 size);
void OLED_ShowChinese(u8 x,u8 y,u8 chr,u8 size,u8 mode);
void ShowBMP(u8 width,u8 hight,s8 x1,s8 y1,u8 size,u8 image);
#endif