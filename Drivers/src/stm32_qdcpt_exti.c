 /**
 ******************************************************************************
 * @file    stm32_qdcpt.h
 * @author  Zhu Guohua
 * @version V1.0
 * @date    07-February-2015
 * @brief   This file provides
 *            - over the printf function
 ******************************************************************************
 * @attention
 * Quadcopter = qdcpt
 * COPYRIGHT 2015 
 ******************************************************************************  
 */
 #include <stdbool.h>
 
 #include "stm32_qdcpt.h"
 #include "stm32f10x_conf.h"
 #include "stm32f10x_exti.h"
 #include "nvic_configure.h"
 #include "radiolink.h"
 #include "ledseq.h"
 static void (*interruptCb)(void) = 0;
 
 /* Interruption initialisation */
void extiInit(void)
{
	/*Nested Vectored Interrupt Controller(NVIC)*/
  NVIC_InitTypeDef NVIC_InitStructure;
	
//NVIC_PreemptionPriority:��ռ���ȼ�
//NVIC_SubPriority       :��Ӧ���ȼ�
//NVIC_Channel           :�жϱ��
//NVIC_Group             :�жϷ��� 0~4  
  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;//EXTI0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_RADIO_PRI;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0f;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void nrfIsr(void)
{
  if (interruptCb)
		
    interruptCb();

  return;
}

void nrfSetInterruptCallback(void (*cb)(void))
{
  interruptCb = cb;
}

void extiInterruptHandler(void)
{
  if (EXTI_GetITStatus(QDCPT_SPI_IRQ_LINE)==SET)
  {
    nrfIsr();
		//LedseqRun(LEDR, seq_testPassed);
    EXTI_ClearITPendingBit(QDCPT_SPI_IRQ_LINE);
  }
	
}

 /* Interrupt service routine, call the interrupt callback
 */


