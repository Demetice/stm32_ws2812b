#include "ALL_Includes.h"//���������ͷ�ļ�

void ALL_Config(void);

int main(void)
{    
    ALL_Config();
    USART1_DMA_Config();
    Timer2_init();

    while(1) 
    {
        USART1_MsgHandle();
    }
}

/************************
�������ܣ��ܳ�ʼ��
�����������
�����������
��    ע����
************************/
void ALL_Config(void)
{
    Delay_Init(48);
    LED_Init();
}

