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
/*Application Include Header File*/
#include "stm32_qdcpt_led.h"

void Stm32QdcptInitLed(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//Start the GPIO clk
	RCC_APB2PeriphClockCmd(LED_GPIO_CLK,ENABLE);

	//select the mode
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	
	//select the control pin
	GPIO_InitStructure.GPIO_Pin=LEDL_PIN;
	//init the gpio
	GPIO_Init(LEDL_GPIO_PORT,&GPIO_InitStructure);
	
	//select the control pin
	GPIO_InitStructure.GPIO_Pin=LEDR_PIN;
	//init the gpio
	GPIO_Init(LEDR_GPIO_PORT,&GPIO_InitStructure);
	
	//close the led
	GPIO_SetBits(LEDL_GPIO_PORT,LEDL_PIN);
	//close the led
	GPIO_SetBits(LEDR_GPIO_PORT,LEDR_PIN);
}

/************************************************************
 *@brief     set the led state
 *@param[in] led:select which led
 *@param[in] value:set the led is true or false
 *@retval    None
 ************************************************************/
void Stm32QdcptLedSet(Led_TypeDef led, uint8_t value)
{
    uint16_t      led_pin  = QDCPT_INVALID_LED;
    GPIO_TypeDef *led_port ;

    /* ����Ϊ LED0 pin */
    if(led == LEDL)
    {
        led_pin  = LEDL_PIN;
        led_port = LEDL_GPIO_PORT;
    }
    /* ����Ϊ LED1 pin */
    else if(led == LEDR)
    {
        led_pin  = LEDR_PIN;
        led_port = LEDR_GPIO_PORT;
    }

    /* ���Ϊ��Ч LED */
    if (led != QDCPT_INVALID_LED)
    {
        if (value == 1)
        {
            /* Open LED */
            GPIO_ResetBits(led_port, led_pin);
        }
        else if (value == 0)
        {
            /* Close LED */
            GPIO_SetBits(led_port, led_pin);
        }
    }
}

void Stm32QdcptLedToggle(Led_TypeDef led)
{
    uint16_t      led_pin  = QDCPT_INVALID_LED;
    GPIO_TypeDef *led_port;

    /*Set as the LEDL */
    if(led == LEDL)
    {
        led_pin  = LEDL_PIN;
        led_port = LEDL_GPIO_PORT;
    }
    /* Set as the LEDR */
    else if(led == LEDR)
    {
        led_pin  = LEDR_PIN;
        led_port = LEDR_GPIO_PORT;
    }
    
    /* if the Led is effective */
    if(led != QDCPT_INVALID_LED)
    {
        /* �л� LED ����״̬ */
        led_port->ODR ^= led_pin;
    }
}



