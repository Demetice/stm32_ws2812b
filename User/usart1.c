#include "usart.h"

uint8_t UART1_RXBUFFER[USART_REC_LEN];
#define USART1_TX GPIO_Pin_9
#define USART1_RX GPIO_Pin_10

//���ڽ���DMA����
#define UART_RX_LEN 128


/****************static*****************/
//���ڽ���DMA����
static uint8_t Uart_Rx[UART_RX_LEN] = {0};

/****************extern*****************/

static void DMA_config()
{
    DMA_InitTypeDef DMA_InitStructure;
    //������DMA����
    //����DMAʱ��
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    //DMA1ͨ��5����
    DMA_DeInit(DMA1_Channel3);
    //�����ַ
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->RDR);
    //�ڴ��ַ
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Uart_Rx;
    //dma���䷽����
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    //����DMA�ڴ���ʱ�������ĳ���
    DMA_InitStructure.DMA_BufferSize = UART_RX_LEN;
    //����DMA���������ģʽ��һ������
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    //����DMA���ڴ����ģʽ
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    //���������ֳ�
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    //�ڴ������ֳ�
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    //����DMA�Ĵ���ģʽ
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    //����DMA�����ȼ���
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    //����DMA��2��memory�еı����������
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel3,&DMA_InitStructure);
    //ʹ��ͨ��5
    DMA_Cmd(DMA1_Channel3,ENABLE);
}

//---------------------���ڹ�������---------------------
void USART1_DMA_Config() 
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure; //���崮�ڳ�ʼ���ṹ��
    NVIC_InitTypeDef NVIC_InitStruct;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE); //ʹ��GPIOA��ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);//ʹ��USART��ʱ��
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1);//����PA9�ɵڶ���������??TX
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);//����PA10�ɵڶ��������� RX?

    /*USART1_TX ->PA9 USART1_RX ->PA10*/
    GPIO_InitStructure.GPIO_Pin = USART1_TX|USART1_RX;//ѡ�д���Ĭ������ܽ�
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //��������������
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//����ܽ�9��ģʽ
    GPIO_Init(GPIOA, &GPIO_InitStructure); //���ú������ѽṹ�����������г�ʼ��????????

    DMA_config();
    /*����ͨѶ��������*/
    USART_InitStructure.USART_BaudRate = 115200;//9600; //������
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //����λ8λ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//ֹͣλ1λ
    USART_InitStructure.USART_Parity = USART_Parity_No;//У��λ ��
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//ʹ�ܽ��պͷ�������

    USART_Init(USART1, &USART_InitStructure);

    //TXE�����ж�,TC��������ж�,RXNE�����ж�,PE��ż�����ж�,�����Ƕ��
    USART_ITConfig(USART1,USART_IT_TC,DISABLE);
    USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);
    USART1->ICR |= 1<<4; //���������IDLE�жϣ������һֱ��IDLE�ж�
    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
    //����DMA��ʽ����
    //USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);
    //����DMA��ʽ����
    USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);
    USART_Cmd(USART1, ENABLE);

    /* USART1��NVIC�ж����� */
    NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 0x02;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}


//����1�����ж�
void USART1_IRQHandler(void)
{
    uint16_t Len = 0;
    if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
    {
        USART1->ICR |= 1<<4; //����ж�
        DMA_Cmd(DMA1_Channel3,DISABLE);
        Len = UART_RX_LEN - DMA_GetCurrDataCounter(DMA1_Channel3);
            printf("%s", Uart_Rx);
        //���ô������ݳ���
        DMA_SetCurrDataCounter(DMA1_Channel3,UART_RX_LEN);
        //��DMA
        DMA_Cmd(DMA1_Channel3,ENABLE);
    }
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


