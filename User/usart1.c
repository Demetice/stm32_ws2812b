#include "usart.h"

#define USART1_TX GPIO_Pin_9
#define USART1_RX GPIO_Pin_10

//串口接收DMA缓存
#define UART_RX_LEN 128


/****************static*****************/
//串口接收DMA缓存
uint8_t Uart_Rx[UART_RX_LEN] = {0};
USART1_MSG_HANDLE_STATE_E g_eUartState = E_USART1_MSG_HANDLE_STATE_IDLE;
uint16_t g_ucUartMsgLen = 0;


/****************extern*****************/
extern void WS2812_send_internal(uint8_t (*color)[3], uint16_t len);

//---------------------串口功能配置---------------------
void USART1_DMA_Config(void) 
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure; //定义串口初始化结构体
    NVIC_InitTypeDef NVIC_InitStruct;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE); //使能GPIOA的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);//使能USART的时钟
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1);//配置PA9成第二功能引脚??TX
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);//配置PA10成第二功能引脚 RX?

    /*USART1_TX ->PA9 USART1_RX ->PA10*/
    GPIO_InitStructure.GPIO_Pin = USART1_TX|USART1_RX;//选中串口默认输出管脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //定义输出最大速率
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//定义管脚9的模式
    GPIO_Init(GPIOA, &GPIO_InitStructure); //调用函数，把结构体参数输入进行初始化????????

    //DMA_config();
    /*串口通讯参数设置*/
    USART_InitStructure.USART_BaudRate = 115200;//9600; //波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //数据位8位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//停止位1位
    USART_InitStructure.USART_Parity = USART_Parity_No;//校验位 无
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//使能接收和发送引脚

    USART_Init(USART1, &USART_InitStructure);

    //TXE发送中断,TC传输完成中断,RXNE接收中断,PE奇偶错误中断,可以是多个
    USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);           //使能接收中断
    //采用DMA方式发送
    //USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);

    USART_Cmd(USART1, ENABLE);

    /* USART1的NVIC中断配置 */
    NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 0x02;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}

void USART1_Send_Byte(unsigned char Data) //发送一个字节；
{
    USART_SendData(USART1,Data);
    while( USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET );
}

void USART1_Send_Bytes(unsigned char *Data, int len) //发送字符串；
{
    int i;

    for(i = 0; i < len; i++)
    {
        USART1_Send_Byte(Data[i]);
    }
}

//串口1接收中断
void USART1_IRQHandler(void)
{
    static uint8_t data = 0, data2 = 0;
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        data = (uint8_t)USART_ReceiveData(USART1);
        if (g_eUartState == E_USART1_MSG_HANDLE_STATE_HANDLING)
        {
            if (data2 == UART_STOP_BYTE_ONE 
                && data == UART_STOP_BYTE_TWO)
            {
                g_eUartState = E_USART1_MSG_HANDLE_STATE_COMPLETE;
            }
            else
            {
                Uart_Rx[g_ucUartMsgLen++] = data;
                data2 = data;
            }
                        
        }
        else if(g_eUartState == E_USART1_MSG_HANDLE_STATE_IDLE
            && data == UART_START_BYTE)
        {
            g_eUartState = E_USART1_MSG_HANDLE_STATE_HANDLING;
            g_ucUartMsgLen = 0;
        }
    }
}

void USART1_MsgHandle(void)
{
    SMART_EYE_LED_S *pstVal = (SMART_EYE_LED_S *) Uart_Rx;

    if (pstVal->num > 2) pstVal->num = 2;

    if (g_eUartState == E_USART1_MSG_HANDLE_STATE_COMPLETE)
    {
        USART1_Send_Bytes(Uart_Rx, g_ucUartMsgLen);

        WS2812_send_internal((uint8_t (*)[3])pstVal->color, pstVal->num);
        
        g_eUartState = E_USART1_MSG_HANDLE_STATE_IDLE;
    }
}



//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
    USART1_Send_Byte((unsigned char)ch);
    return ch;
}
#endif 


