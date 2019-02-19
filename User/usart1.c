#include "usart.h"

uint8_t UART1_RXBUFFER[USART_REC_LEN];


void USART1_GPIO_Init(void)
{
   GPIO_InitTypeDef   GPIO_Initstructure;

   RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE); 
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    /*Connect pin to Periph */
   GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1);   // ע��������GPIO_PinSource9
   GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);

   GPIO_Initstructure.GPIO_Pin = GPIO_Pin_9;
   GPIO_Initstructure.GPIO_Mode=GPIO_Mode_AF;
   GPIO_Initstructure.GPIO_OType=GPIO_OType_PP;  // �������
   GPIO_Initstructure.GPIO_PuPd=GPIO_PuPd_UP;
   GPIO_Initstructure.GPIO_Speed=GPIO_Speed_50MHz;   
   GPIO_Init(GPIOA,&GPIO_Initstructure);

   GPIO_Initstructure.GPIO_Pin = GPIO_Pin_9;  // ��������  
   GPIO_Init(GPIOA, &GPIO_Initstructure);

}


/*******************************************************************************
  * @brief  DMA1�ж�����
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

   DMA_InitStructure.DMA_BufferSize = USART_REC_LEN; // �����С
   DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;    // �ڴ浽�ڴ�ر�
   DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;   // ��ͨģʽ
   DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  // ���赽�ڴ�
   DMA_InitStructure.DMA_Priority = DMA_Priority_High; // DMAͨ�����ȼ�
   DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;// �ڴ��ַ����
   DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t)&USART1->RDR;   // �����ַ   
   DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;// �����ַ����
   DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; // �ڴ����ݳ���
   DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)UART1_RXBUFFER;   // �����ڴ����ַ
   DMA_InitStructure.DMA_PeripheralDataSize =DMA_PeripheralDataSize_Byte;//�������ݳ���

   DMA_Init(DMA1_Channel3, &DMA_InitStructure);
   DMA_ClearITPendingBit(DMA1_IT_TC3); // ���һ��DMA�жϱ�־
   DMA_ClearFlag(DMA1_FLAG_GL3); 
   DMA_ITConfig(DMA1_Channel3, DMA_IT_TC,ENABLE);// ʹ��DMA��������ж�
   DMA1_NVIC_Init(); // ������������ж����ã�������������ô˺�������
   DMA_Cmd(DMA1_Channel3, ENABLE);
}

void USART1_Init(unsigned long BaudRate)
{
    USART_InitTypeDef   USART_Initstructure;

    USART1_GPIO_Init(); // ����������Ķ˿ڳ�ʼ����������������ô˺������ɡ�

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
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//�������ڽ����ж�
    USART_Cmd(USART1, ENABLE);      // ʹ�ܴ���

    DMA1_NVIC_Init();
    DMA1_Init();
}


void USART1_Send_Byte(unsigned char Data) //����һ���ֽڣ�
{
    USART_SendData(USART1,Data);
    while( USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET );
}

void USART1_Send_Bytes(unsigned char *Data, int len) //�����ַ�����
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

void USART1_IRQHandler(void) //�жϴ�������
{    
    int len = 0;

    if(USART_GetITStatus(USART1, USART_IT_IDLE) == SET) //�ж��Ƿ����жϣ�
    {
        USART_ReceiveData(USART1);
        len = USART_REC_LEN - DMA_GetCurrDataCounter(DMA1_Channel5); //����ӱ�֡���ݳ���
        //printf("usart1 receive data by dma len:%u\r\n", g_ucUsart1ReceiveDataLen);
        USART1_Send_Bytes(UART1_RXBUFFER, len);

        USART_ClearITPendingBit(USART1, USART_IT_IDLE);         //����жϱ�־
        USART1DmaClr();                   //�ָ�DMAָ�룬�ȴ���һ�εĽ���
    }  
} 


//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
    USART1_Send_Byte((unsigned char)ch);
    return ch;
}
#endif 


