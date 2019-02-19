#include "ALL_Includes.h"//包含所需的头文件

void ALL_Config(void);

int main(void)
{
    uint8_t i;    
    uint32_t color[] = {0x0000bb};
    uint8_t buf[32] = "Hello world.\n";
    
    ALL_Config();
    USART1_DMA_Config();
    
    //Timer2_init();
    //Tim1_init();
    //DMA_Configuration();


    while(1) 
    {
        LED_ON();
        delay_ms(400);
        LED_OFF();
        delay_ms(600);

        printf("I'm printf \n");

        //WS2812_send(color, 1);
    }
}

/************************
函数功能：总初始化
输入参数：无
输出参数：无
备    注：无
************************/
void ALL_Config(void)
{
    Delay_Init(48);
		LED_Init();
}

