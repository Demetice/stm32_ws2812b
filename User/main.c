#include "ALL_Includes.h"//���������ͷ�ļ�

void ALL_Config(void);

int main(void)
{
    uint8_t i;    
    uint32_t color[] = {0x0000bb};
    uint8_t buf[32] = "Hello world.\n";
    uint8_t rgb[][3] = {{0,0,45},{45,0,0}};
    
    ALL_Config();
    USART1_DMA_Config();
    
    Timer2_init();
    //Tim1_init();
    //DMA_Configuration();


    while(1) 
    {
        LED_ON();
        delay_ms(400);
        LED_OFF();
        delay_ms(600);

        //printf("I'm printf \n");

        //WS2812_send(color, 1);
        WS2812_send_internal(rgb, 1);
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

