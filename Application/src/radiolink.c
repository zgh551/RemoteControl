 /**
 ******************************************************************************
 * @file    radiolink.c
 * @author  Zhu Guohua
 * @version V1.0
 * @date    07-February-2015
 * @brief   This file provides
 *            - nrf24L01 link 
 ******************************************************************************
 * @attention
 * Quadcopter = qdcpt
 * COPYRIGHT 2015 
 ******************************************************************************  
 */
 
 #include "radiolink.h"
 #include "nRF24L01.h"
 #include "crtp.h"
 #include "errno.h"
 #include "stm32_qdcpt_led.h"
 #include "ledseq.h"
 #include "OLED.h"
 
 static bool isInit;

#define RADIO_CONNECTED_TIMEOUT   M2T(2000)
 /* Synchronisation */
 static xSemaphoreHandle dataRdy;
 /* Data queue */
 static xQueueHandle txQueue;
 static xQueueHandle rxQueue;
 
 static uint32_t LastPacketTick;
 
 uint8_t reg_state;
 //Union used to efficiently handle the packets (Private type)
typedef union
{
  CRTPPacket crtp;
  struct {
    uint8_t size;
    uint8_t data[32];
  } __attribute__((packed)) raw;
} RadioPacket;

RadioPacket Rx_pk;
RadioPacket Tx_pk;
static struct {
 bool enabled;
}state;


enum {
RX_1=0,
TX_1,
RX_2,
TX_2
};

#define RX_PLOAD_WIDTH  32  	
#define TX_PLOAD_WIDTH  32

uint8_t myrxtest[32]={0};
unsigned char dataLen;
/********************************************************************************
 *@brief	reset the txQueue and clear the TX FIFO register
 *@param	None
 *@retval	None
 ********************************************************************************/
void radioReset(void)
{
  xQueueReset(txQueue);
  nrfFlushTx();
}

/**************************************************
 *@brief set the radio enable
 *@param enable:input true or false 
 *@retval None 
**/
int  radioSetEnable(bool enable)
{
  nrfSetEnable(enable);
  state.enabled = enable;
	return 0;
}

/*****************************************************************************
 *@brief	send the packet data to txQueue
 *@param[in]*pk: the packet pointer with information
 *@retval	if state is not enable then return ENETDOWN,otherwise return zero.
 *****************************************************************************/
int radioSendPacket(CRTPPacket * pk)
{
  //if (!state.enabled)
  //  return ENETDOWN;
  xQueueSend( txQueue, pk, 0);

  return 0;
}

/*********************************************************************************************
 *@brief	  receive the packet data from the txQueue and put the packet into the pk pointer 
 *@param[out] *pk: the packet pointer with information
 *@retval	  if state is not enable then return ENETDOWN,otherwise return zero.
 *********************************************************************************************/
int radioReceivePacket(CRTPPacket * pk)
{
  //if (!state.enabled)
  //  return ENETDOWN;

  if(xQueueReceive(rxQueue, pk, 0) == pdTRUE)//1111
	return 1;
	else
  return 0;
}

/*****************************************************************************
 *@brief  judge the radio whether is connected or not
 *@param  None
 *@retval if radio is connected then return true,otherwise return false
 *****************************************************************************/
static bool radioIsConnected(void)
{
  if ((xTaskGetTickCount() - LastPacketTick) > RADIO_CONNECTED_TIMEOUT)
    return false;

  return true;
}

static struct crtpLinkOperations radioOp =
{
  radioSetEnable,                     //int (*setEnable)(bool enable);
  radioSendPacket,                    //int (*sendPacket)(CRTPPacket *pk);
  radioReceivePacket,                 //int (*receivePacket)(CRTPPacket *pk);
  radioIsConnected,                   //bool (*isConnected)(void);
  radioReset                          //int (*reset)(void);
};

/*******************************************************************************************
 *@brief  Radio task handles the CRTP packet transfers as well as the radio link
 *        specific communications (eg. Scann and ID ports, communication error handling
 *        and so much other cool things that I don't have time for it ...)
 *@param  NUll
 *@retval None 
 *******************************************************************************************/
static void radiolinkTask(void * arg)
{
//	uint32_t lastWakeTime;
//	lastWakeTime = xTaskGetTickCount();
  //Packets handling loop
  while(1)
  {
    xSemaphoreTake(dataRdy, portMAX_DELAY);
    LastPacketTick = xTaskGetTickCount();
		//LedseqRun(LEDR, seq_testPassed);
    nrfSetEnable(false);
    //Fetch all the data (Loop until the RX Fifo is NOT empty)
    while(!(nrfReadReg(REG_FIFO_STATUS)&0x01) )
    {
			dataLen = nrfRxLength();
      if (dataLen>32)          //If a packet has a wrong size it is dropped
        nrfFlushRx();		   //clear the RX FIFO register
      else                     //Else, it is processed
      {
        //Fetch the data
        Rx_pk.raw.size = dataLen-1;
        nrfRxPayload((uint8_t*)Rx_pk.raw.data,dataLen);//read the RX payload 
				xQueueSend(rxQueue, &Rx_pk, 0);
      }
    }
//  
    //Push the data to send (Loop until the TX Fifo is full or there is no more data to send)
   /*while((uxQueueMessagesWaiting((xQueueHandle)txQueue) > 0) && !(nrfReadReg(REG_FIFO_STATUS)&0x20))
    {
      xQueueReceive(txQueue, &Tx_pk, 0); 
			Tx_pk.raw.size++;
      //uartSendCRTPPacket((CRTPPacket *)&pk.raw);
			LedseqRun(LEDL, seq_linkup);
			nrfWritePayload((uint8_t*) Tx_pk.raw.data, Tx_pk.raw.size);
      //nrfWriteAck(0, (uint8_t*) pk.crtp.data, pk.crtp.size);
    }*/

    //clear the interruptions flags
    nrfWriteReg(REG_STATUS, 0x70);
    
    //Re-enable the radio
    nrfSetEnable(true);
	}
/*****************************************************************************************/		
		
//		OLED_ShowString(0,0,"accept",16);
//		OLED_Refresh_Gram();
//		vTaskDelayUntil((portTickType *)&lastWakeTime,40/portTICK_RATE_MS);//// 500Hz
//		nrfSetEnable(false);
//		reg_state = nrfReadReg(REG_STATUS); //REG_FIFO_STATUS nrfReadReg(REG_FIFO_STATUS)&0x02
//		vTaskDelay(10/portTICK_RATE_MS);//去抖动
//		OLED_ShowNum(0,0,reg_state,3,16,1);
//		if(reg_state&0x40)//接收中断
//		{				
//			//Fetch all the data (Loop until the RX Fifo is NOT empty)
//			while( !(nrfReadReg(REG_FIFO_STATUS)&0x01) )
//			{
//				dataLen = nrfRxLength();					         					
//				if (dataLen>32)                    //it is processed
//				{
//					nrfFlushRx();		   //clear the RX FIFO register
//				}
//				else//If a packet has a wrong size it is dropped
//				{
//					Rx_pk.raw.size = dataLen-1;//-->crtp one is flag other is data
//					nrfRxPayload((uint8_t*)Rx_pk.raw.data,dataLen);//read the RX payload 						
//					xQueueSend(rxQueue, &Rx_pk, 0);
//				}
//			}		
//		}
//		
//		if(reg_state&0x20)//数据发送完成中断
//		{
//			OLED_ShowString(80,2,"tx",12);		
//			OLED_Refresh_Gram();//更新显示到OLED
//		}
//			
//		if(reg_state&0x10)//达到最大次重发中断
//		{
//			if(reg_state & 0x01)//TX FIFO 寄存器满
//			{
//				nrfFlushTx();
//			}
//		}	
//		nrfSetEnable(true);
//		nrfWriteReg(REG_STATUS,reg_state);
//		reg_state = nrfReadReg(REG_STATUS);
//	}
}

/************************************************************************
 *@brief  initialize the nRF24L01 build the connected
 *@param  None
 *@retval None
 ************************************************************************/
static void radiolinkInitNRF24L01P(uint8_t modle)
{
  int i;
  uint8_t radioAddress[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
	
	RADIO_DIS_CE;
  //Set radio RX P0 address
  nrfSetRxAddress(0, radioAddress);
	
	//Set the TX address
	nrfSetTxAddress(radioAddress);
	
	//Enable the receive channal0
	nrfWriteReg(REG_EN_RXADDR,0x01);
	
  //Set the radio channel 40
  nrfSetChannel(40);
	
  //Set the radio data rate 
  nrfSetDateRate(RADIO_RATE_1M);
	
	//Set the channal 0 Auto Ack
  nrfWriteReg(REG_EN_AA, 0x00);
	
	//Set 500us and 10 times
  nrfWriteReg(REG_SETUP_RETR, 0x1a);
	
  switch(modle)
	{
		case RX_1:
			//Set the Rx data width
			nrfWriteReg(REG_RX_PW_P0,RX_PLOAD_WIDTH);	
			//Power the radio, Enable the RX_RD AND MAX_RT interruption, set the radio in PRX mode
			nrfWriteReg(REG_CONFIG, 0x0F);
			break;
		case TX_1:
			//Set the Tx data width
			nrfWriteReg(REG_RX_PW_P0,TX_PLOAD_WIDTH);	
			//Power the radio, Enable the TX_DS AND MAX_RT interruption, set the radio in PRX mode
			nrfWriteReg(REG_CONFIG, 0x0E);
			break;
		case RX_2:
			//Flush RX
			for(i=0;i<3;i++)
			nrfFlushRx();
			//Flush TX
			for(i=0;i<3;i++)
			nrfFlushTx();
			//Power the radio, Enable the RX_RD AND MAX_RT interruption, set the radio in PRX mode
			nrfWriteReg(REG_CONFIG, 0x0F);
			//active the regist
			nrfActivate();
			// Enable the dynamic payload size and the ack payload for the pipe 0
			nrfWriteReg(REG_DYNPD,   0x01);
			nrfWriteReg(REG_FEATURE, 0x06);
			
			break;
		case TX_2:
			//Flush RX
			for(i=0;i<3;i++)
			nrfFlushRx();
			//Flush TX
			for(i=0;i<3;i++)
			nrfFlushTx();
			//Power the radio, Enable the TX_DS AND MAX_RT interruption, set the radio in PRX mode
			nrfWriteReg(REG_CONFIG, 0x0E);
			//active the regist
			nrfActivate();
			// Enable the dynamic payload size and the ack payload for the pipe 0
			nrfWriteReg(REG_DYNPD,   0x01);
			nrfWriteReg(REG_FEATURE, 0x06);
			
			break;
			default:
			
			break;
	}
	RADIO_EN_CE;
	vTaskDelay(M2T(100)); //Wait for the chip to be ready
}

/****************************************************************************************
 *@brief check the nRF24L01 whether exist
 *@param None
 *@retval if one then successful,otherwise if zero then false
 ***************************************************************************************/  
char nrf24l01Check(void)
{
    uint8_t buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
    uint8_t i;
    //SPIx_SetSpeed(SPI_SPEED_8); //spi速度为9Mhz（24L01的最大SPI时钟为10Mhz） 
    
    nrfWriteBuf(REG_TX_ADDR,buf,5);//写入5个字节的地址.    
    nrfReadBuf (REG_TX_ADDR,buf,5); //读出写入的地址  
    for(i=0;i<5;i++)if(buf[i]!=0XA5)break;                                 
    if(i!=5)return 0;//检测24L01错误    
    return 1;        //检测到24L01
}  

/************************************************************************
 *@brief	if interruption happen then run the radiolinkTask functions 
 *@param	None
 *@retval	None
 */
static void interruptCallback(void)
{
  portBASE_TYPE  xHigherPriorityTaskWoken = pdFALSE;

  //To unlock RadioTask
  xSemaphoreGiveFromISR(dataRdy, &xHigherPriorityTaskWoken);
	//LedseqRun(LEDR, seq_testPassed);
  if(xHigherPriorityTaskWoken)
    vPortYield();
}

/*************************************************************************
 *@brief  initialize the radio 
 *@param  None
 *@retval None
 *************************************************************************/
void radiolinkInit(void)
{
  //if(isInit)
  //  return;
	//hardware initiallise
  nrfSpiInit();
  //check the hardware whether existence
  isInit = nrf24l01Check();
	//if(isInit)
	//{LedseqRun(LEDR, seq_testPassed);}
 
	//set the interrupt function
  nrfSetInterruptCallback(interruptCallback);
	//
  //vTaskSetApplicationTaskTag(0, (pdTASK_HOOK_CODE)TASK_RADIO_ID_NBR);

  // Initialise the semaphores 
  vSemaphoreCreateBinary(dataRdy);
  // Queue init 
  rxQueue = xQueueCreate(3, sizeof(RadioPacket));
  txQueue = xQueueCreate(3, sizeof(RadioPacket));
  //init the nrf24l01
  radiolinkInitNRF24L01P(TX_1);
	
    // Launch the Radio link task 
  xTaskCreate(radiolinkTask,(const signed portCHAR* )"RadioLink",configMINIMAL_STACK_SIZE,NULL,2, NULL);

  isInit = true;
}

/********************************************************************************
 *@brief	reinitialise the radio
 *@param	None
 *@retval	None
 ********************************************************************************/
void radiolinkReInit(void)
{
  if (!isInit)
  return;
  radiolinkInitNRF24L01P(TX_1);
}

struct crtpLinkOperations * radiolinkGetLink(void)
{
  return &radioOp;
}

