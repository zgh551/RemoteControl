#include "power.h"

//PB5-> EN1
//PB6-> EN2
void Power_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
		/* Configure KEY in intput pushpull mode */
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//…Ë÷√Œ™ ‰≥ˆ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_WriteBit(GPIOB,GPIO_Pin_5,Bit_RESET);
	GPIO_WriteBit(GPIOB,GPIO_Pin_6,Bit_SET);
}

