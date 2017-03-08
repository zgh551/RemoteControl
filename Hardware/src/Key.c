#include "stm32_qdcpt.h"
#include "Key.h"
#include "OLED.h"
#include "interface.h"
#include "stm32f10x.h"
#include "stm32_qdcpt_adc.h"



enum KeyValue{
Key_LeftUp 		=1,
Key_RightUp 	=2,
Key_RockLeft  =3,
Key_RockRight =4,
Key_Left			=5,
Key_Up				=6,
Key_Center		=7,
Key_Right			=8,
Key_Down			=9
};

u16 THY_Up=0   ,THY_Center=0 ,THY_Down=0  ;
u16 YAW_Left   ,YAW_Center=0 ,YAW_Right ;
u16 ROll_Up    ,ROll_Center=0,ROll_Down ;
u16 PITH_Left  ,PITH_Center=0,PITH_Right;

//static xQueueHandle RevisePackDelivery;//数据传送队列
//CRTPPacket RevisePack;

u8 MenuMain=1, MenuSecond=1;
u8 CurrentLevel=1;
u8 THY_Revise_Select=0;
u8 PlaneShowFlag=0;
u8 PlaneStateFlag;
u8 AltHoldFlag=0;

u8 GetPlaneShowFlag(void)
{
 return PlaneShowFlag;
}

void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* GPIOD Periph clock enable */
  RCC_APB2PeriphClockCmd(QDCPT_KEY_CLK, ENABLE);
	
	/* Configure KEY in intput pushpull mode */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//设置为内部上拉
	
	GPIO_InitStructure.GPIO_Pin = QDCPT_KEY_PINB;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = QDCPT_KEY_PINC;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
//	RevisePackDelivery = xQueueCreate(2, sizeof(CRTPPacket));
}

u8 Key_Scan(u8 mode)
{
	static u8 key_up=1;//按键按松开标志
	if(mode)key_up=1;  //支持连按		  
	if(key_up&&(KEY_LEFT_UP  ==0|| KEY_RIGHT_UP ==0||KEY_LEFT      ==0||KEY_RIGHT==0||
							KEY_DIR_LEFT ==0|| KEY_DIR_UP   ==0||KEY_DIR_CENTER==0|| 
							KEY_DIR_RIGHT==0|| KEY_DIR_DOWN ==0))
	{
		vTaskDelay(10/portTICK_RATE_MS);//去抖动 
		key_up=0;//表明有按键按下
		
		if     (KEY_LEFT_UP==0)		return 1;
		else if(KEY_RIGHT_UP==0)	return 2;
		else if(KEY_LEFT==0)			return 3;
		else if(KEY_RIGHT==0)			return 4;
		else if(KEY_DIR_LEFT==0)	return 5;
		else if(KEY_DIR_UP==0)		return 6;
		else if(KEY_DIR_CENTER==0)return 7;
		else if(KEY_DIR_RIGHT==0)	return 8;
		else if(KEY_DIR_DOWN==0)	return 9;	
	}
	else if(KEY_LEFT_UP  ==1&& KEY_RIGHT_UP ==1&& KEY_LEFT       ==1&& KEY_RIGHT     ==1&&
					KEY_DIR_LEFT ==1&& KEY_DIR_UP   ==1&& KEY_DIR_CENTER ==1&& KEY_DIR_RIGHT ==1&& KEY_DIR_DOWN==1)
	key_up=1; //按键全部释放	    
 	return 0;// 无按键按下
}

void InterfaceDisplay()
{
	switch(MenuMain)
	{
		case 1:
			switch(CurrentLevel)
			{
				case 1:
					ReviseInterface();
					break;
				
				case 2:
					switch(MenuSecond)
					{
						case 1:
							THY_ReviseInterface(THY_Revise_Select,THY_Up,THY_Down);
							break;
						case 2:		
							Homing_ReviseInterface(THY_Center,YAW_Center,ROll_Center,PITH_Center);
							break;
							
						default:
							THY_ReviseInterface(THY_Revise_Select,THY_Up,THY_Down);
							break;
					}				
					break;
				default:
					break;
			}		
			break;
		
		case 2:
			switch(CurrentLevel)
			{
				case 1:
					PlaneInterface();
					break;
				
				case 2:
					switch(MenuSecond)
					{
						case 1:
							OLED_Clear();//刷新一次，以后不刷了
							State_PlaneInterface();	
						  PlaneStateFlag=1;
							break;
						case 2:		
							OLED_Clear();//刷新一次，以后不刷了
							State1_PlaneInterface();	
							PlaneStateFlag=2;
							break;
							
						default:
							OLED_Clear();//刷新一次，以后不刷了
							State_PlaneInterface();	
							break;
					}				
					break;
				default:
					break;
			}	
			break;
		
		case 3:
			switch(CurrentLevel)
			{
				case 1:
				PowerInterface();
					break;
				
				case 2:
					
					break;
				default:
					break;
			}	
			break;
		default:
			ReviseInterface();
			break;
	}
}

//左上按键事件处理
void KeyLeftUpEvent()
{
	if(MenuMain==2&&CurrentLevel==2)
	{
	SetModel(false);
		AltHoldFlag=0;
	}
}

//右上按键事件处理
void KeyRightUpEvent(void)
{
	if(MenuMain==2&&CurrentLevel==2)
	{
	SetModel(true);
		AltHoldFlag=1;
	}
}

void KeyRockRightEvent(void)
{
	switch(MenuMain)
	{
		case 1:
		CurrentLevel++;
		if(CurrentLevel > 2)
			CurrentLevel =2;
		break;
		
		case 2:
			CurrentLevel++;
			if(CurrentLevel > 2)
				CurrentLevel =2;
				PlaneShowFlag |= 0x01;
			break;
		
		case 3:
			break;
		default:
			CurrentLevel=1;
			break;
	}
}

void KeyRockLeftEvent(void)
{
	switch(MenuMain)
	{
		case 1:
			CurrentLevel--;
			if(CurrentLevel == 0)
				CurrentLevel=1;
			break;
		
		case 2:
			CurrentLevel--;
			if(CurrentLevel == 0)
				CurrentLevel=1;
			PlaneShowFlag &= 0xFE;
			break;
		
		case 3:	
			break;
		default:
			MenuMain    =1;
			CurrentLevel=1;	
			break;
	}
}
//五向按键上 事件处理
void KeyUpEvent()
{
	switch(MenuMain)
	{
		case 1:
			switch(CurrentLevel)
			{
				case 1:
					break;
				
				case 2:
					THY_Revise_Select=0;
					break;
				default:
					break;
			}		
			break;
		
		case 2:
			switch(CurrentLevel)
			{
				case 1:

					break;
				
				case 2:
					
					break;
				default:
					break;
			}	
			break;
		
		case 3:
			switch(CurrentLevel)
			{
				case 1:

					break;
				
				case 2:
					
					break;
				default:
					break;
			}	
			break;
		default:
			switch(CurrentLevel)
			{
				case 1:

					break;
				
				case 2:
					
					break;
				default:
					break;
			}	
			break;
	}
}

void KeyDownEvent()
{
	switch(MenuMain)
	{
		case 1:
			switch(CurrentLevel)
			{
				case 1:
					break;
				
				case 2:
					THY_Revise_Select=~0;
					break;
				default:
					break;
			}		
			break;
		
		case 2:
			switch(CurrentLevel)
			{
				case 1:

					break;
				
				case 2:
					
					break;
				default:
					break;
			}	
			break;
		
		case 3:
			switch(CurrentLevel)
			{
				case 1:

					break;
				
				case 2:
					
					break;
				default:
					break;
			}	
			break;
		default:
			switch(CurrentLevel)
			{
				case 1:

					break;
				
				case 2:
					
					break;
				default:
					break;
			}	
			break;
	}
}

void KeyLeftEvent(void)
{
	switch(CurrentLevel)
	{
		case 1:
			MenuMain--;
			if(MenuMain==0)
				MenuMain =3;
			break;
		
		case 2:
			MenuSecond--;
			if(MenuSecond==0)
				MenuSecond=2;
			break;
		
		default:
			MenuMain=1;
			CurrentLevel =1;
			break;
	}
}

void KeyRightEvent(void)
{
		switch(CurrentLevel)
	{
		case 1:
			MenuMain++;
			if(MenuMain>3)
				MenuMain =1;
			break;
		
		case 2:
			MenuSecond++;
			if(MenuSecond>2)
				MenuSecond=1;
			break;
		
		default:
			MenuMain=1;
			CurrentLevel =1;
			break;
	}
}

void KeyCenterEvent()
{
	switch(MenuMain)
	{
		case 1:
			switch(CurrentLevel)
			{
				case 1:
					break;
				
				case 2:
					switch(MenuSecond)
					{
						case 1:
							if(THY_Revise_Select == 0)
							{
								THY_Up = Get_THY();
								PlaneShowFlag |= 0x02;
							}
							else
							{
								THY_Down = Get_THY();
								PlaneShowFlag |= 0x04;
							}
							break;
						case 2:
							THY_Center  = Get_THY();
							YAW_Center  = Get_YAW();
							ROll_Center = Get_ROLL();
							PITH_Center = Get_PITCH();
							PlaneShowFlag |= 0x08;
							break;
						default:
							break;
					}
					break;
				default:
					break;
			}		
			break;
		
		case 2:
			switch(CurrentLevel)
			{
				case 1:

					break;
				
				case 2:
					
					break;
				default:
					break;
			}	
			break;
		
		case 3:
			switch(CurrentLevel)
			{
				case 1:

					break;
				
				case 2:
					
					break;
				default:
					break;
			}	
			break;
		default:
			switch(CurrentLevel)
			{
				case 1:

					break;
				
				case 2:
					
					break;
				default:
					break;
			}	
			break;
	}
}

void Key_Handle(void)
{
	enum KeyValue Key_Value=0;
	Key_Value = Key_Scan(0);
	switch(Key_Value)
	{
		case Key_LeftUp:
			KeyLeftUpEvent();
			InterfaceDisplay();
			break;
		
		case Key_RightUp:
			KeyRightUpEvent();
			InterfaceDisplay();
			break;
		
		case Key_RockLeft:
			KeyRockLeftEvent();
			InterfaceDisplay();
			break;
		
		case Key_RockRight:			
			KeyRockRightEvent();
			InterfaceDisplay();
			break;
		
		case Key_Left:
			KeyLeftEvent();
			InterfaceDisplay();
			break;
		case Key_Up:
			KeyUpEvent();
			InterfaceDisplay();
			break;
		
		case Key_Center:
			KeyCenterEvent();
			InterfaceDisplay();
			break;
		
		case Key_Right:
			KeyRightEvent();
			InterfaceDisplay();
			break;
		
		case Key_Down:
			KeyDownEvent();
			InterfaceDisplay();
			break;
		default:		
			break;
	}
	
}

static portTASK_FUNCTION( KeyHandle, pvParameters )
{
	uint32_t lastWakeTime;
	lastWakeTime = xTaskGetTickCount();
	for(;;)
	{	
		vTaskDelayUntil((portTickType *)&lastWakeTime,100/portTICK_RATE_MS);//// 500Hz 
		Key_Handle();
	}
}


//void SetReviseCRTPPacket(CRTPPacket *p)
//{
//	RevisePack.header = 0x12;
//	RevisePack.size = 6;
//	
//	RevisePack.data[0]=THY_Up;
//	RevisePack.data[1]=THY_Down;
//	RevisePack.data[2]=THY_Center;
//	RevisePack.data[3]=YAW_Center;
//	RevisePack.data[4]=ROll_Center;
//	RevisePack.data[5]=PITH_Center;
//	
//	xQueueSend(RevisePackDelivery, &RevisePack, 0);
//}

//int GetReviseCRTPPacket(CRTPPacket *p)
//{
//  if (xQueueReceive(RevisePackDelivery, p, 0) == pdTRUE)
//  {
//    return 1;
//  }
//		return 0;
//}

void Qdcpt_KeyHandle_Task( unsigned portBASE_TYPE uxPriority )
{
	xTaskCreate( KeyHandle, ( signed char * ) "KeyHandle", configMINIMAL_STACK_SIZE, NULL, uxPriority , ( xTaskHandle * ) NULL );
}



