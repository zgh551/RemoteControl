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
 #ifndef __NRF24L01REG_H
 #define __NRF24L01REG_H
 
 #ifdef __cplusplus
	extern C{
 #endif
		
/* Registers address definition */
#define REG_CONFIG              0x00 // �����շ�״̬��CRCУ��ģʽ�Լ��շ�״̬��Ӧ��ʽ
#define REG_EN_AA               0x01 // �Զ�Ӧ��������
#define REG_EN_RXADDR           0x02 // �����ŵ�����
#define REG_SETUP_AW            0x03 // �շ���ַ�������
#define REG_SETUP_RETR          0x04 // �Զ��ط���������
#define REG_RF_CH               0x05 // ����Ƶ������ Fo=(2400+RF_CH)MHz
#define REG_RF_SETUP            0x06 // �������ʡ����Ĺ�������
#define REG_STATUS              0x07 // ״̬�Ĵ���
#define REG_OBSERVE_TX          0x08 // ���ͼ�⹦��
#define REG_RPD                 0x09 // ��ַ���   
#define REG_RX_ADDR_P0          0x0A // Ƶ��0�������ݵ�ַ
#define REG_RX_ADDR_P1          0x0B // Ƶ��1�������ݵ�ַ
#define REG_RX_ADDR_P2          0x0C // Ƶ��2�������ݵ�ַ
#define REG_RX_ADDR_P3          0x0D // Ƶ��3�������ݵ�ַ
#define REG_RX_ADDR_P4          0x0E // Ƶ��4�������ݵ�ַ
#define REG_RX_ADDR_P5          0x0F // Ƶ��5�������ݵ�ַ
#define REG_TX_ADDR             0x10 // ���͵�ַ�Ĵ���
#define REG_RX_PW_P0            0x11 // ����Ƶ��0�������ݳ���
#define REG_RX_PW_P1            0x12 // ����Ƶ��0�������ݳ���
#define REG_RX_PW_P2            0x13 // ����Ƶ��0�������ݳ���
#define REG_RX_PW_P3            0x14 // ����Ƶ��0�������ݳ���
#define REG_RX_PW_P4            0x15 // ����Ƶ��0�������ݳ���
#define REG_RX_PW_P5            0x16 // ����Ƶ��0�������ݳ���
#define REG_FIFO_STATUS         0x17 // FIFOջ��ջ��״̬�Ĵ�������
#define REG_DYNPD               0x1C // ʹ�ܶ�̬���س���
#define REG_FEATURE             0x1D // �����Ĵ���
//RF Setup Regist 
#define VAL_RF_SETUP_1M         0x07 //
#define VAL_RF_SETUP_2M         0x0F //
//setup of Address Widths
#define VAL_SETUP_AW_3B         1    // 3 byte
#define VAL_SETUP_AW_4B         2    // 4 byte
#define VAL_SETUP_AW_5B         3    // 5 byte
		
 #ifdef __cplusplus
	}
 #endif
 #endif
