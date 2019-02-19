#include "usart.h"

uint8_t UART1_RXBUFFER[USART_REC_LEN];


void USART1_GPIO_Init(void)
{
   GPIO_InitTypeDef   GPIO_Initstructure;

   RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE); 
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    /*Connect pin to Periph */
   GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1);   // 注意这里是GPIO_PinSource9
   GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);

   GPIO_Initstructure.GPIO_Pin = GPIO_Pin_9;
   GPIO_Initstructure.GPIO_Mode=GPIO_Mode_AF;
   GPIO_Initstructure.GPIO_OType=GPIO_OType_PP;  // 推挽输出
   GPIO_Initstructure.GPIO_PuPd=GPIO_PuPd_UP;
   GPIO_Initstructure.GPIO_Speed=GPIO_Speed_50MHz;   
   GPIO_Init(GPIOA,&GPIO_Initstructure);

   GPIO_Initstructure.GPIO_Pin = GPIO_Pin_9;  // 浮空输入  
   GPIO_Init(GPIOA, &GPIO_Initstructure);

}


/*******************************************************************************
  * @brief  DMA1中断配置
  * @param  None
  * @retval None
*******************************************************************************/
void DMA1_NVIC_Init(void)
{
    NVIC_InitTypeDef    NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 2;

    NVIC_Init(&NVIC_InitStructure);
}

void DMA1_Init(void)
{
   DMA_InitTypeDef DMA_InitStructure;
   RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

   DMA_InitStructure.DMA_BufferSize = USART_REC_LEN; // 缓存大小
   DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;    // 内存到内存关闭
   DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;   // 普通模式
   DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  // 外设到内存
   DMA_InitStructure.DMA_Priority = DMA_Priority_High; // DMA通道优先级
   DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;// 内存地址递增
   DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t)&USART1->RDR;   // 外设地址   
   DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;// 外设地址不变
   DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; // 内存数据长度
   DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)UART1_RXBUFFER;   // 定义内存基地址
   DMA_InitStructure.DMA_PeripheralDataSize =DMA_PeripheralDataSize_Byte;//外设数据长度

   DMA_Init(DMA1_Channel3, &DMA_InitStructure);
   DMA_ClearITPendingBit(DMA1_IT_TC3); // 清除一次DMA中断标志
   DMA_ClearFlag(DMA1_FLAG_GL3); 
   DMA_ITConfig(DMA1_Channel3, DMA_IT_TC,ENABLE);// 使能DMA传输完成中断
   DMA1_NVIC_Init(); // 调用了上面的中断配置，故主函数里调用此函数即可
   DMA_Cmd(DMA1_Channel3, ENABLE);
}

void USART1_Init(unsigned long BaudRate)
{
    USART_InitTypeDef   USART_Initstructure;

    USART1_GPIO_Init(); // 调用了上面的端口初始化，故主函数里调用此函数即可。

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    USART_Initstructure.USART_BaudRate = BaudRate;
    USART_Initstructure.USART_Parity  =USART_Parity_No;
    USART_Initstructure.USART_WordLength =USART_WordLength_8b;  
    USART_Initstructure.USART_StopBits =USART_StopBits_1;
    USART_Initstructure.USART_Mode    = USART_Mode_Rx|USART_Mode_Tx;
    USART_Initstructure.USART_HardwareFlowControl =USART_HardwareFlowControl_None;
    USART_Init(USART1, &USART_Initstructure);

    USART_ClearFlag(USART1, USART_FLAG_TC);
    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//开启串口接受中断
    USART_Cmd(USART1, ENABLE);      // 使能串口

    DMA1_NVIC_Init();
    DMA1_Init();
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

void USART1DmaClr(void)
{
    DMA_Cmd(DMA1_Channel3, DISABLE);
    DMA_SetCurrDataCounter(DMA1_Channel3, USART_REC_LEN);
    DMA_ClearFlag(DMA1_FLAG_GL3);
    DMA_Cmd(DMA1_Channel3, ENABLE);
}

void USART1_IRQHandler(void) //中断处理函数；
{    
    int len = 0;

    if(USART_GetITStatus(USART1, USART_IT_IDLE) == SET) //判断是否发生中断；
    {
        USART_ReceiveData(USART1);
        len = USART_REC_LEN - DMA_GetCurrDataCounter(DMA1_Channel5); //算出接本帧数据长度
        //printf("usart1 receive data by dma len:%u\r\n", g_ucUsart1ReceiveDataLen);
        USART1_Send_Bytes(UART1_RXBUFFER, len);

        USART_ClearITPendingBit(USART1, USART_IT_IDLE);         //清除中断标志
        USART1DmaClr();                   //恢复DMA指针，等待下一次的接收
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


