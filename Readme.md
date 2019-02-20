# stm32f030f4p6 驱动ws2812b

> stm32f030f4p6 驱动ws2812b灯珠，可以通过串口来修改灯的颜色

## 硬件端口对应

| PA6  | WS2812B 控制口 |
| ---- | -------------- |
| PA9  | USART1 TX      |
| PA10 | USART1 RX      |
| PA4  | LED            |

## 说明

采用TIM3 PWM通道1 也就是PA6输出PWM波形， 用DMA 通道4来更新TIM3->CCR1.

采用了内部晶振，外设48Mhz

## 串口修改灯颜色

```c
hdr = 0x55;
num = n; //需要驱动的灯的个数
for(n)
{
    r
    g
    b
}
tail1 = 0x0d;
tail2 = 0x0a;
```

