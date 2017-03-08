#ifndef INTERFACE_H
#define INTERFACE_H

#include <string.h>

#include "OLED.h"
#include "Key.h"
#include "crtp.h"	
/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

void FlashOpen(void);
void ReviseInterface(void);
void THY_ReviseInterface(u8 SelectFlag,u16 max,u16 min);
void Homing_ReviseInterface(u16 thy,u16 yaw,u16 roll,u16 pitch);

void PlaneInterface(void);
void State_PlaneInterface(void);
void State1_PlaneInterface(void);
void InterfaceReceiveCRTPPacket(CRTPPacket *p);

void PowerInterface(void);

void Qdcpt_InterfaceRefresh_Task( unsigned portBASE_TYPE uxPriority );
#endif