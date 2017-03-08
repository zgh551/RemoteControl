/**
  ******************************************************************************
  * @file    USART/Printf/main.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "Stm32Print.h"
#include "stm32_qdcpt.h"
#include "stm32_qdcpt_usart.h"
#include "stm32_qdcpt_led.h"

//#include "stm32_qdcpt_system.h"
#include "ledseq.h"
#include "radiolink.h"
#include "comm.h"
#include "console.h"
#include "crtp.h"
#include "system.h"
/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

#include "OLED.h"
#include "Key.h"
#include "power.h"
#include "interface.h"
#include "stm32_qdcpt_adc.h"
#define mainDELAY_LOOP_COUNT 100


/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
	LedseqInit();
	OLED_Init();				//≥ı ºªØOLED
  ADC_Configuration();
	KEY_Init();
	Power_Init();
	
	FlashOpen();
	ReviseInterface();
//State_PlaneInterface();
	//	PlaneInterface();
	//	PowerInterface();
//	THY_ReviseInterface(0);
	systemLaunch();

	vTaskStartScheduler();
  while (1);
}
	//QdcptUsartInit();
	//QdcptUsartNvicInit();
	//radiolinkInit();
	//crtpSetLink(uartGetLink());
	//crtpInit();
	//QdcptUsartDmaInit();
	//uartSendDataDma(4, text);
	/*LED Init*/
	//USART_SendData(QDCPT_USART,0xAA);
	//uartSendCRTPPacket(&mypack);
	//Stm32QdcptInitLed();	
	//Stm32QdcptLedSet(LEDL,OFF);
	//Stm32QdcptLedSet(LEDR,OFF);
	//LedseqRun(LEDR, seq_charging);
	//LedseqRun(LEDL, seq_linkup);
	//while(DMA_GetFlagStatus(DMA1_FLAG_TC4)==0);
	//uartSendDataDma(2, text);
	//uartSendData(4,text);
	//Qdcpt_Usart_Tasks(2);
/******************* (C) COPYRIGHT 2015 ZhuGuoHua *****END OF FILE****/