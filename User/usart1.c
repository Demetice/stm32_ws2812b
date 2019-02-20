#include "usart.h"

#define USART1_TX GPIO_Pin_9
#define USART1_RX GPIO_Pin_10

//���ڽ���DMA����
#define UART_RX_LEN 128


/****************static*****************/
//���ڽ���DMA����
uint8_t Uart_Rx[UART_RX_LEN] = {0};
USART1_MSG_HANDLE_STATE_E g_eUartState = E_USART1_MSG_HANDLE_STATE_IDLE;
uint16_t g_ucUartMsgLen = 0;


/****************extern*****************/
extern void WS2812_send_internal(uint8_t (*color)[3], uint16_t len);

//---------------------���ڹ�������---------------------
void USART1_DMA_Config(void) 
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

    //DMA_config();
    /*����ͨѶ��������*/
    USART_InitStructure.USART_BaudRate = 115200;//9600; //������
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //����λ8λ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//ֹͣλ1λ
    USART_InitStructure.USART_Parity = USART_Parity_No;//У��λ ��
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//ʹ�ܽ��պͷ�������

    USART_Init(USART1, &USART_InitStructure);

    //TXE�����ж�,TC��������ж�,RXNE�����ж�,PE��ż�����ж�,�����Ƕ��
    USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);           //ʹ�ܽ����ж�
    //����DMA��ʽ����
    //USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);

    USART_Cmd(USART1, ENABLE);

    /* USART1��NVIC�ж����� */
    NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 0x02;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
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

//����1�����ж�
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


