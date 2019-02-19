#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f0xx.h"
//#include "sys.h" 

#define USART_REC_LEN  			256  	//定义最大接收字节数 256

//如果想串口中断接收，请不要注释以下宏定义
void USART1_DMA_Config();


#endif


