#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f0xx.h"
//#include "sys.h" 

#define USART_REC_LEN  			256  	//�����������ֽ��� 256

typedef struct 
{
    unsigned char type; //0x55
    unsigned char num;
    unsigned char color[1]; //rgb ���У���Сʱ num * 3
    unsigned char chksum;
}SMART_EYE_LED_S;


//����봮���жϽ��գ��벻Ҫע�����º궨��
void USART1_DMA_Config();


#endif


