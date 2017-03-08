#include "interface.h"
#include "fp16.h"

u8 PackData[24];
float yaw_f=0.0,roll_f=0.0,pitch_f=0.0,height_f=0.0;
float chip_temperature_f,SpeedAcc_f,Battery_f;
float Err_f,PIDVal_f;

void FlashOpen(void)
{
	s16 i;
	u16 j,k;
	for(i=0;i<16;i++)
	{
		OLED_Fill(32,0,96,64,0);
		ShowBMP(64,64,32,7-i,64,0);
		OLED_Refresh_Gram();//更新显示到OLED	
		for(j=0;j<65535;j++)
		{
			for(k=0;k<1;k++)
			{
			}
		}
	}
	
	for(i=-64;i<129;i+=4)
	{
		OLED_Clear();
		ShowBMP(64,64,i,0,64,1);
		OLED_Refresh_Gram();//更新显示到OLED	
	}
}

void ReviseInterface(void)
{
		OLED_Clear();
		OLED_ShowChinese(  0,0,0,16,1); //校
		OLED_ShowChinese(112,0,1,16,1);//正
		
		ShowBMP(64,64,32 ,0,64,2);
		ShowBMP(32,32,0  ,2,32,0);
		ShowBMP(32,32,96 ,2,32,1);
		OLED_Refresh_Gram();//更新显示到OLED	
}

void THY_ReviseInterface(u8 SelectFlag,u16 max,u16 min)
{
	OLED_Clear();
	OLED_ShowChinese(24,0,6,16,1);//油
	OLED_ShowChinese(40,0,7,16,1);//门
	OLED_ShowChinese(56,0,8,16,1);//值
	OLED_ShowChinese(72,0,0,16,1);//校
	OLED_ShowChinese(88,0,1,16,1);//正
	
	OLED_ShowChinese(0,0,14,16,1);//左
	OLED_ShowChinese(112,0,15,16,1);//右
	
	OLED_Rectangle(23,0,105,16,0);//矩形框
	OLED_Rectangle(0,16,127,63,0);//矩形框
	OLED_Line(0,39,127,39,1);//画线
	OLED_Line(67,16,67,63,1);//画线
	//最大油门
	OLED_ShowChinese(2 ,20,9 ,16,SelectFlag);//最
	OLED_ShowChinese(18,20,10,16,SelectFlag);//大
	OLED_ShowChinese(34,20,6 ,16,SelectFlag);//油
	OLED_ShowChinese(50,20,7 ,16,SelectFlag);//门
	OLED_ShowNum(79,20,max,4,16,1);
	
	//最小油门
	OLED_ShowChinese(2 ,43,9 ,16,~SelectFlag);//最
	OLED_ShowChinese(18,43,11,16,~SelectFlag);//小
	OLED_ShowChinese(34,43,6 ,16,~SelectFlag);//油
	OLED_ShowChinese(50,43,7 ,16,~SelectFlag);//门
	OLED_ShowNum(79,43,min,4,16,1);

	OLED_Refresh_Gram();//更新显示到OLED	
}

//归位值校正
void Homing_ReviseInterface(u16 thy,u16 yaw,u16 roll,u16 pitch)
{
	OLED_Clear();
	OLED_ShowChinese(24,0,12,16,1);//归
	OLED_ShowChinese(40,0,13,16,1);//位
	OLED_ShowChinese(56,0,8,16,1); //值
	OLED_ShowChinese(72,0,0,16,1); //校
	OLED_ShowChinese(88,0,1,16,1); //正
	
	OLED_ShowChinese(0,0,14,16,1);//左
	OLED_ShowChinese(112,0,15,16,1);//右
	
	OLED_Rectangle(23,0,105,16,0);//矩形框
	OLED_Rectangle(0,16,127,63,0);//矩形框
	OLED_Line(0,39,127,39,1);//画线
	OLED_Line(63,16,63,63,1);//画线
	OLED_Line(24,16,24,63,1);//画线
	OLED_Line(85,16,85,63,1);//画线
	
	OLED_ShowNum(28,20,thy,4,16,1);
	OLED_ShowNum(28,43,yaw,4,16,1);
	OLED_ShowNum(93,20,roll,4,16,1);
	OLED_ShowNum(93,43,pitch,4,16,1);
	
	OLED_ShowChinese(4,20,16,16,0);//油
	OLED_ShowChinese(4,43,17,16,0);//偏
	OLED_ShowChinese(66,20,18,16,0);//翻
	OLED_ShowChinese(66,43,19,16,0);//俯

	OLED_Refresh_Gram();//更新显示到OLED	
}

//飞行器状态显示界面
void PlaneInterface(void)
{
		OLED_Clear();
		OLED_ShowChinese(0  ,0,2,16,1);//显
		OLED_ShowChinese(112,0,3,16,1);//示
		
		ShowBMP(64,64,32,0,64,3);//飞机状态显示
		ShowBMP(32,32,0  ,2,32,0);//图标左
		ShowBMP(32,32,96 ,2,32,1);//图标右
		OLED_Refresh_Gram();//更新显示到OLED	
}

void State_PlaneInterface(void)
{
//	OLED_Clear();//刷新一次，以后不刷了
	OLED_Rectangle(0,0,127,63,0);//矩形框
	OLED_Line(0,15,127,15,1);//画线
	OLED_Line(0,31,127,31,1);//画线
	OLED_Line(0,47,127,47,1);//画线
	
	OLED_ShowString(8, 2,"YAW:",12);
	OLED_ShowString(5,18,"ROLL:",12);
	OLED_ShowString(2,34,"PITCH:",12);
	OLED_ShowString(2,50,"THRUST",12);
	OLED_Line(38,1,38,63,1);//画线
	
	if(yaw_f<0)
	{
		OLED_ShowChar(40,2 ,'-',12,1);
		OLED_ShowFloatNum(46,2 ,- yaw_f  ,3,1,12,1);
	}
	else
	{
		OLED_ShowChar(40,2,' ',12,1);
		OLED_ShowFloatNum(46,2 ,yaw_f  ,3,1,12,1);
	}
	/****************************/
	if(roll_f<0)
	{
		OLED_ShowChar(40,18,'-',12,1);
		OLED_ShowFloatNum(46,18,- roll_f ,3,1,12,1);
	}
	else
	{
		OLED_ShowChar(40,18,' ',12,1);
		OLED_ShowFloatNum(46,18,roll_f ,3,1,12,1);
	}
	/****************************/
	if(pitch_f<0)
	{
		OLED_ShowChar(40,34,'-',12,1);
		OLED_ShowFloatNum(46,34,- pitch_f,3,1,12,1);
	}
	else
	{
		OLED_ShowChar(40,34,' ',12,1);	
		OLED_ShowFloatNum(46,34,pitch_f,3,1,12,1);
	}
	
	OLED_ShowFloatNum(40,50,height_f,5,1,12,1);
	
	OLED_Refresh_Gram();//更新显示到OLED
}

void State1_PlaneInterface(void)
{
//	OLED_Clear();//刷新一次，以后不刷了
	OLED_Rectangle(0,0,127,63,0);//矩形框
	OLED_Line(0,15,127,15,1);//画线
	OLED_Line(0,31,127,31,1);//画线
	OLED_Line(0,47,127,47,1);//画线
	
	OLED_ShowString(8, 2,"Err:",12);
	OLED_ShowString(2,18,"PIDVal",12);
	OLED_ShowString(8,34,"Bat:",12);
	OLED_ShowString(2,50,"THRUST",12);
	OLED_Line(38,1,38,63,1);//画线
	
	if(Err_f<0)
	{
		OLED_ShowChar(40,2 ,'-',12,1);
		OLED_ShowFloatNum(46,2 ,- Err_f  ,3,1,12,1);
	}
	else
	{
		OLED_ShowChar(40,2,' ',12,1);
		OLED_ShowFloatNum(46,2 ,Err_f  ,3,1,12,1);
	}
	/****************************/
	if(PIDVal_f<0)
	{
		OLED_ShowChar(40,18,'-',12,1);
		OLED_ShowFloatNum(46,18,- PIDVal_f ,3,1,12,1);
	}
	else
	{
		OLED_ShowChar(40,18,' ',12,1);
		OLED_ShowFloatNum(46,18,PIDVal_f ,3,1,12,1);
	}
	/****************************/
	if(Battery_f<0)
	{
		OLED_ShowChar(40,34,'-',12,1);
		OLED_ShowFloatNum(46,34,- Battery_f,1,1,12,1);
	}
	else
	{
		OLED_ShowChar(40,34,' ',12,1);	
		OLED_ShowFloatNum(46,34,Battery_f,1,1,12,1);
	}
	
	OLED_ShowFloatNum(40,50,height_f,5,1,12,1);
	
	OLED_Refresh_Gram();//更新显示到OLED
}

void PowerInterface(void)
{
		OLED_Clear();
		OLED_ShowChinese(0  ,0,4,16,1);//显
		OLED_ShowChinese(112,0,5,16,1);//示
		
		ShowBMP(64,64,32 ,0,64,4);//飞机状态显示
		ShowBMP(32,32,0  ,2,32,0);//图标左
		ShowBMP(32,32,96 ,2,32,1);//图标右
		OLED_Refresh_Gram();//更新显示到OLED	
}

void InterfaceReceiveCRTPPacket(CRTPPacket *p)
{
  if(p->header == 0x80)
	{
		memcpy(PackData, p->data, p->size);
		if(p->size <= 28)
		{
			
			roll_f  						= ByteToFloat(&PackData[0]);
			pitch_f 						= ByteToFloat(&PackData[4]);
			yaw_f   						= ByteToFloat(&PackData[8]);
			
			Err_f 	            = ByteToFloat(&PackData[12]);
			height_f 						= ByteToFloat(&PackData[16]);
			PIDVal_f 						= ByteToFloat(&PackData[20]);
			Battery_f 					= ByteToFloat(&PackData[24]);
		}
	}	
}

static portTASK_FUNCTION( InterfaceRefresh, pvParameters )
{
	uint32_t lastWakeTime;
	lastWakeTime = xTaskGetTickCount();
	for(;;)
	{	
		vTaskDelayUntil((portTickType *)&lastWakeTime,200/portTICK_RATE_MS);//// 500Hz 
		if(PlaneShowFlag & 0x01)
		{
			if(PlaneStateFlag == 1)
			{
				State_PlaneInterface();
			}
			else if(PlaneStateFlag == 2)
			{
				State1_PlaneInterface();
			}
		}
	}
}

void Qdcpt_InterfaceRefresh_Task( unsigned portBASE_TYPE uxPriority )
{
	xTaskCreate( InterfaceRefresh, ( signed char * ) "InterfaceRefresh", configMINIMAL_STACK_SIZE, NULL, uxPriority , ( xTaskHandle * ) NULL );
}
