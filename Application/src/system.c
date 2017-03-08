 /**
 ******************************************************************************
 * @file    system.c
 * @author  Zhu Guohua
 * @version V1.0
 * @date    07-March-2015
 * @brief   This file provides
 *            - over the printf function
 ******************************************************************************
 * @attention
 * Quadcopter = qdcpt
 * COPYRIGHT 2015 
 ******************************************************************************  
 */

#include "system.h"
#include "Key.h"
#include "interface.h"
/* FreeRtos includes */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "console.h"
#include "comm.h"
#include "ledseq.h"
#include "crtp.h"
#include "debug.h"
#include "nRF24L01.h"
#include "radiolink.h"
#include "stm32_qdcpt_adc.h"

enum ControlModel{
UART_Model = 0,
ROCK_Model
}CurrentModel= ROCK_Model;

CRTPPacket TxPack;
CRTPPacket RxPack;

uint8_t i,state;
/* Private functions */
void RX_Task(void *pvParameters);
void TX_Task(void *pvParameters);
static void systemTask(void *arg);

/* Public functions */
void systemLaunch(void)
{
	xTaskCreate(systemTask,( signed portCHAR* )"SYSTEM",
              6*configMINIMAL_STACK_SIZE, NULL, /*Piority*/2, NULL);
}

void systemTask(void *arg)
{
	//communication initialise
	CurrentModel= ROCK_Model;
	commInit();

	Qdcpt_Rock_Task(2);  //摇杆值获取任务
//	Qdcpt_Usart_Tasks(2);//串口数据打包任务
	Qdcpt_KeyHandle_Task(2);
	Qdcpt_InterfaceRefresh_Task(2);
	xTaskCreate(RX_Task,( signed portCHAR* )"task1",
								3*configMINIMAL_STACK_SIZE,NULL,2,NULL);
	xTaskCreate(TX_Task,( signed portCHAR* )"task2",
								3*configMINIMAL_STACK_SIZE,NULL,2,NULL);
	
	while(1)
	vTaskDelay(portMAX_DELAY);
}


void RX_Task(void *pvParameters)
{
	char *uartTaskName;
	uartTaskName = (char * ) pvParameters;
	for( ;; )
	{
		if(radioReceivePacket(&RxPack) == 1)
		{
			switch(CurrentModel)
			{
//			case UART_Model:
//				uartSendCRTPPacket(&RxPack);
//				break;
			case ROCK_Model:
				if(PlaneShowFlag)
				{
					InterfaceReceiveCRTPPacket(&RxPack);
				}
				break;
			default:
				break;
			}
			
		}
	}
}

void TX_Task(void *pvParameters)
{
	char *uartTaskName;
	uartTaskName = (char * ) pvParameters;
	for( ;; )
	{
		switch(CurrentModel)
		{
//			case UART_Model:
//				if(uartReceiveCRTPPacket(&TxPack)==0)//接收打包好的数据然后无线发送出去
//				{
//					nrfWritePayload(TxPack.raw,TxPack.size+1);//TX mode use itradioSendPacket(&TxPack);
//				}
//				break;
			
			case ROCK_Model:
				if(ADC_RockReceiveCRTPPacket(&TxPack) == 0)
				{
					nrfWritePayload(TxPack.raw,TxPack.size+1);//TX mode use it radioSendPacket(&TxPack);	
					state = nrfReadReg(REG_STATUS);
				}	
				break;
			default :
				break;
		}
	}
}