#include "ALL_Includes.h"//���������ͷ�ļ�

void ALL_Config(void);


int main(void)
{
    uint8_t i;    
    uint8_t color[][3] = {{222, 0, 0}};
    uint8_t buf[32] = "Hello world.\n";
    
	ALL_Config();
    USART1_Init(115200);
    
//	DMX512_Init();

    //Timer2_init();

	while(1) 
    {
        LED_ON();
        delay_ms(400);
        LED_OFF();
        delay_ms(600);

        printf("I'm printf \n");

//        WS2812_send(color, 1);
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

