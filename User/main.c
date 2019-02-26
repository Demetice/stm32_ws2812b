#include "ALL_Includes.h"//包含所需的头文件

void ALL_Config(void);

int main(void)
{    
    int i = 0;
    uint8_t rgb_data[9];
    
    ALL_Config();
    USART1_DMA_Config();
    ws281x_init();

    delay_ms(10);
    ws281x_Off(3);
    delay_ms(1);
    ws281x_Off(3);
    
    while(1) 
    {
        USART1_MsgHandle();
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


