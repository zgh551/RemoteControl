#ifndef STM32_QDCPT_ADC_H
#define STM32_QDCPT_ADC_H

#include "crtp.h"
/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

void ADC_Configuration(void);
void show_value();
u16 Get_THY(void);
u16 Get_YAW(void);
u16 Get_ROLL(void);
u16 Get_PITCH(void);


void Qdcpt_Rock_Task( unsigned portBASE_TYPE uxPriority );
int ADC_RockReceiveCRTPPacket(CRTPPacket *p);
void SetModel(bool tf);
#endif