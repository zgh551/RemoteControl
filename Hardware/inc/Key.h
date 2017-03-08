#ifndef KEY_H
#define KEY_H

#include "stm32f10x.h"
#include "crtp.h"
/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

#define KEY_LEFT_UP					GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_7)//PC7

#define KEY_RIGHT_UP  			GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14)//PB14

#define KEY_LEFT      			GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12)//PB12

#define KEY_RIGHT      			GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)//PB13

#define KEY_DIR_LEFT    		GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)//PC13

#define KEY_DIR_UP       		GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_15)//PC15

#define KEY_DIR_CENTER   		GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_14)//PC14

#define KEY_DIR_RIGHT    		GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)//PB9

#define KEY_DIR_DOWN     		GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)//PB8

extern u8 PlaneShowFlag;

extern u8 PlaneStateFlag;
extern u8 AltHoldFlag;
extern u16 THY_Up     ,THY_Center ,THY_Down  ;
extern u16 YAW_Left   ,YAW_Center ,YAW_Right ;
extern u16 ROll_Up    ,ROll_Center,ROll_Down ;
extern u16 PITH_Left  ,PITH_Center,PITH_Right;

void KEY_Init(void);
void Key_Handle(void);

void Qdcpt_KeyHandle_Task( unsigned portBASE_TYPE uxPriority );
u8 GetPlaneShowFlag(void);

#endif