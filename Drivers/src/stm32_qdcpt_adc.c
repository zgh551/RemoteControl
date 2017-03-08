#include "stm32f10x.h"
#include "stm32_qdcpt.h"
#include "stm32_qdcpt_adc.h"
#include "OLED.h"
#include "Key.h"
#include <math.h>
static xQueueHandle RockPacketDelivery;//数据传送队列
CRTPPacket RockPacket;
__IO uint32_t ADC_DualConvertedValueTab[16];

void ADC_GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	/* Enable peripheral clocks ------------------------------------------------*/
	/* Enable DMA1 clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	/* Enable ADC1, ADC2 and GPIOC clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_ADC2 | QDCPT_ADC_CLK, ENABLE);
//
	/* Configure PB.00,   PB.01,   PC.00,    PC.01 and PC.02 
	(ADC      Channel8,Channel9,Channel0, Channel1 and Channel2)
	as analog input ----------------------------------------------------------*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = QDCPT_ADC_THR_PIN | QDCPT_ADC_YAW_PIN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = QDCPT_ADC_ROLL_PIN | QDCPT_ADC_PITCH_PIN | QDCPT_ADC_VBAT_PIN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void ADC_DMA_Configuration(void)
{
	DMA_InitTypeDef DMA_InitStructure;
/* DMA1 channel1 configuration ----------------------------------------------*/
   DMA_DeInit(DMA1_Channel1);
   DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;//ADC1_DR_Address;
   DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC_DualConvertedValueTab;
   DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
   DMA_InitStructure.DMA_BufferSize = 3;
   DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址不变
   DMA_InitStructure.DMA_MemoryInc     = DMA_MemoryInc_Enable;//内存地址递增
   DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;//ADC外设数据宽度为32位
   DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;//内存数据宽度为32位
   DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//普通模式，一次循环停止
   DMA_InitStructure.DMA_Priority = DMA_Priority_High;
   DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
   DMA_Init(DMA1_Channel1, &DMA_InitStructure);
   /* Enable DMA1 Channel1 */
   DMA_Cmd(DMA1_Channel1, ENABLE);
}

void ADC_Configuration(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	
	ADC_GPIO_Configuration();
	ADC_DMA_Configuration();
	//创建遥控器数据传送队列
	RockPacketDelivery = xQueueCreate(2, sizeof(CRTPPacket));
	
	ADC_DeInit(ADC1);
	ADC_DeInit(ADC2);//设置为缺省值
	/* ADC1 configuration ------------------------------------------------------*/
  ADC_InitStructure.ADC_Mode = ADC_Mode_RegSimult;//同步规则模式 ADC_Mode_Independent;//
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;//扫描模式
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//连续模式
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//软件触发
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//数据右对齐
  ADC_InitStructure.ADC_NbrOfChannel = 3;//设置3个通道连续采样
  ADC_Init(ADC1, &ADC_InitStructure);
  /* ADC1 regular channels configuration */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_8,  1, ADC_SampleTime_239Cycles5); //THR 油门  
  ADC_RegularChannelConfig(ADC1, ADC_Channel_9,  2, ADC_SampleTime_239Cycles5); //YAW 偏航
	ADC_RegularChannelConfig(ADC1, ADC_Channel_17, 3, ADC_SampleTime_239Cycles5);//芯片内部温度测量
  /* Enable ADC1 DMA */
  ADC_DMACmd(ADC1, ENABLE);

  /* ADC2 configuration ------------------------------------------------------*/
  ADC_InitStructure.ADC_Mode = ADC_Mode_RegSimult;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 3;
  ADC_Init(ADC2, &ADC_InitStructure);
  /* ADC2 regular channels configuration */ 
	ADC_RegularChannelConfig(ADC2, ADC_Channel_10, 1, ADC_SampleTime_239Cycles5);//ROLL	 翻滚	
  ADC_RegularChannelConfig(ADC2, ADC_Channel_11, 2, ADC_SampleTime_239Cycles5);//PITCH 俯仰
  ADC_RegularChannelConfig(ADC2, ADC_Channel_12, 3, ADC_SampleTime_239Cycles5);//电池电压测量
  /* Enable ADC2 external trigger conversion */
  ADC_ExternalTrigConvCmd(ADC2, ENABLE);
	
  /* Enable Vrefint channel_16 */
  ADC_TempSensorVrefintCmd(ENABLE);
	
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);
  /* Enable ADC1 reset calibration register */   
  ADC_ResetCalibration(ADC1);
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));
  /* Start ADC1 calibration */
  ADC_StartCalibration(ADC1);
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));

  /* Enable ADC2 */
  ADC_Cmd(ADC2, ENABLE);
  /* Enable ADC2 reset calibration register */   
  ADC_ResetCalibration(ADC2);
  /* Check the end of ADC2 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC2));
  /* Start ADC2 calibration */
  ADC_StartCalibration(ADC2);
  /* Check the end of ADC2 calibration */
  while(ADC_GetCalibrationStatus(ADC2));

  /* Start ADC1 Software Conversion */ 
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);

  /* Test on DMA1 channel1 transfer complete flag */
  while(!DMA_GetFlagStatus(DMA1_FLAG_TC1));
  /* Clear DMA1 channel1 transfer complete flag */
  DMA_ClearFlag(DMA1_FLAG_TC1);
}

void show_value()
{
	OLED_ShowNum(0,28,(uint16_t)ADC_DualConvertedValueTab[0],5,12,1);//油门
	OLED_ShowNum(0,40,(uint16_t)ADC_DualConvertedValueTab[1],5,12,1);//偏航
	OLED_ShowNum(0,52,(uint16_t)ADC_DualConvertedValueTab[2],5,12,1);//芯片温度
	
	OLED_ShowNum(36,28,(uint16_t)(ADC_DualConvertedValueTab[0]>>16),5,12,1);//翻滚角
	OLED_ShowNum(36,40,(uint16_t)(ADC_DualConvertedValueTab[1]>>16),5,12,1);//俯仰角
	OLED_ShowNum(36,52,(uint16_t)(ADC_DualConvertedValueTab[2]>>16),5,12,1);//电池电压
	OLED_Refresh_Gram();//更新显示到OLED	
}

u16 Get_THY(void)
{
	return (uint16_t)ADC_DualConvertedValueTab[0];
}

u16 Get_YAW(void)
{
	return (uint16_t)ADC_DualConvertedValueTab[1];
}

u16 Get_ROLL(void)
{
	return (uint16_t)(ADC_DualConvertedValueTab[0]>>16);
}

u16 Get_PITCH(void)
{
	return (uint16_t)(ADC_DualConvertedValueTab[1]>>16);
}

//void RockValueHandle()
static portTASK_FUNCTION( QdcptRockData, pvParameters )
{
	u16 thrust=0;
	s16 yaw=0,roll=0,pitch=0;
	uint32_t lastWakeTime;
	lastWakeTime = xTaskGetTickCount();
	for(;;)
	{	
		vTaskDelayUntil((portTickType *)&lastWakeTime,30/portTICK_RATE_MS);//// 500Hz 
//		if(PlaneShowFlag == 0x0f)
//		{
			thrust = Get_THY();
			thrust = 2000 - thrust;
			if(AltHoldFlag)
			{
				thrust = (65000*thrust + 1000)/2000;
				if(thrust >30000 &&  thrust <40000)
				{
					thrust = 32767;
				}
			}
			else
			{
				thrust =(u16)(0.01625*pow(thrust,2));
				if(thrust<1000)
				{
				thrust =0;
				}
			}		
			yaw = Get_YAW()-YAW_Center;
			yaw = yaw*30/1200;
			
			roll = ROll_Center - Get_ROLL();
			roll = roll*30/1200;
			
			pitch = Get_PITCH() - PITH_Center;
			pitch = pitch*30/1200;
			
			RockPacket.header=0x30;
			RockPacket.size=5;
			RockPacket.data[0]=(u8)thrust;//油门值低位
			RockPacket.data[1]=(u8)(thrust>>8);//油门值高位
			RockPacket.data[2]=pitch;//偏航角
			RockPacket.data[3]=roll;//翻滚角
			RockPacket.data[4]=yaw;//俯仰角
			xQueueSend(RockPacketDelivery, &RockPacket, 0);
//		}
	}
}

void SetModel(bool tf)
{
			RockPacket.header=0x20;
			RockPacket.size=1;
			RockPacket.data[0]= tf;
			xQueueSend(RockPacketDelivery, &RockPacket, 0);
}

int ADC_RockReceiveCRTPPacket(CRTPPacket *p)
{
  if (xQueueReceive(RockPacketDelivery, p, 0))
  {
    return 0;
  }
	return -1;
}

void Qdcpt_Rock_Task( unsigned portBASE_TYPE uxPriority )
{
	xTaskCreate( QdcptRockData, ( signed char * ) "Rock", configMINIMAL_STACK_SIZE, NULL, uxPriority , ( xTaskHandle * ) NULL );
}
	