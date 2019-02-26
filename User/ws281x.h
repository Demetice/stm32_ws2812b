/*
 * ws281x.h - init & comm api for the ws281x PWM RGB LED
 * 04-01-15 E. Brombaugh
 */

#ifndef __ws281x__
#define __ws281x__

#include "stm32f0xx.h"

#define WS_MAX_LEDS 8

void ws281x_init(void);
void ws281x_send(uint8_t rgb_data[], uint8_t num_leds);
void ws281x_hsv2rgb(uint8_t rgb[], uint8_t hsv[]);
void ws281x_Off(uint16_t num);


#endif

