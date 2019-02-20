#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f0xx.h"
//#include "sys.h" 

#define USART_REC_LEN  			256  	//定义最大接收字节数 256

#define UART_START_BYTE 0x55

#define UART_STOP_BYTE_ONE 0xd
#define UART_STOP_BYTE_TWO 0xa

typedef enum
{
    E_USART1_MSG_HANDLE_STATE_IDLE = 0,
    E_USART1_MSG_HANDLE_STATE_HANDLING = 1,
    E_USART1_MSG_HANDLE_STATE_COMPLETE = 2,
    E_USART1_MSG_HANDLE_STATE_BUTT
}USART1_MSG_HANDLE_STATE_E;

typedef struct 
{
    unsigned char num;
    unsigned char color[1]; //rgb 排列，大小时 num * 3
    unsigned char chksum;
}SMART_EYE_LED_S;


//如果想串口中断接收，请不要注释以下宏定义
void USART1_DMA_Config(void);
void USART1_MsgHandle(void);


#endif


