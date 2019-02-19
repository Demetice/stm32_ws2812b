#include "usart.h"

uint8_t UART1_RXBUFFER[USART_REC_LEN];
#define USART1_TX GPIO_Pin_9
#define USART1_RX GPIO_Pin_10

//串口接收DMA缓存
#define UART_RX_LEN 128


/****************static*****************/
//串口接收DMA缓存
static uint8_t Uart_Rx[UART_RX_LEN] = {0};

/****************extern*****************/

static void DMA_config()
{
    DMA_InitTypeDef DMA_InitStructure;
    //串口收DMA配置
    //启动DMA时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    //DMA1通道5配置
    DMA_DeInit(DMA1_Channel3);
    //外设地址
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->RDR);
    //内存地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Uart_Rx;
    //dma传输方向单向
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    //设置DMA在传输时缓冲区的长度
    DMA_InitStructure.DMA_BufferSize = UART_RX_LEN;
    //设置DMA的外设递增模式，一个外设
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    //设置DMA的内存递增模式
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    //外设数据字长
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    //内存数据字长
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    //设置DMA的传输模式
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    //设置DMA的优先级别
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    //设置DMA的2个memory中的变量互相访问
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel3,&DMA_InitStructure);
    //使能通道5
    DMA_Cmd(DMA1_Channel3,ENABLE);
}

//---------------------串口功能配置---------------------
void USART1_DMA_Config() 
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

    DMA_config();
    /*串口通讯参数设置*/
    USART_InitStructure.USART_BaudRate = 115200;//9600; //波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //数据位8位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//停止位1位
    USART_InitStructure.USART_Parity = USART_Parity_No;//校验位 无
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//使能接收和发送引脚

    USART_Init(USART1, &USART_InitStructure);

    //TXE发送中断,TC传输完成中断,RXNE接收中断,PE奇偶错误中断,可以是多个
    USART_ITConfig(USART1,USART_IT_TC,DISABLE);
    USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);
    USART1->ICR |= 1<<4; //必须先清除IDLE中断，否则会一直进IDLE中断
    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
    //采用DMA方式发送
    //USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);
    //采用DMA方式接收
    USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);
    USART_Cmd(USART1, ENABLE);

    /* USART1的NVIC中断配置 */
    NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 0x02;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}


//串口1接收中断
void USART1_IRQHandler(void)
{
    uint16_t Len = 0;
    if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
    {
        USART1->ICR |= 1<<4; //清除中断
        DMA_Cmd(DMA1_Channel3,DISABLE);
        Len = UART_RX_LEN - DMA_GetCurrDataCounter(DMA1_Channel3);
            printf("%s", Uart_Rx);
        //设置传输数据长度
        DMA_SetCurrDataCounter(DMA1_Channel3,UART_RX_LEN);
        //打开DMA
        DMA_Cmd(DMA1_Channel3,ENABLE);
    }
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


