#include "ALL_Includes.h"//包含所需的头文件

void ALL_Config(void);


int main(void)
{	
	ALL_Config();
	
	while(1)
	{
			LED_ON();
			delay_ms(300);
			LED_OFF();
			delay_ms(300);
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






