/**
 * Student name: Mark Armdan
 * Student number: 5489776
 *
 */
#include "stm32f4xx.h"

#define AF03 0x03
#define LEDLeft 2
#define LEDRight 3

void initTIM9()
{
    RCC->APB2ENR |= 1 << 16; // Enable clock for TIM9

    TIM9->PSC = 0;          // Set prescaler to 0
    TIM9->ARR = 0xffff - 1; // Set auto reload register to 0xffffC
    TIM9->CNT = 0;          // Reset counter

    TIM9->CCMR1 |= 6 << 4;  // Set channel 1 to PWM mode 1
    TIM9->CCMR1 |= 6 << 12; // Set channel 2 to PWM mode 1

    TIM9->CCER |= 1 << 0; // Enable channel 1
    TIM9->CCER |= 1 << 4; // Enable channel 2

    TIM9->CR1 |= 1 << 0; // Enable counter
}

void setLEDLeft(int brightness)
{
    TIM9->CCR1 = brightness % (0xffff);
}

void setLEDRight(int brightness)
{
    TIM9->CCR2 = brightness % (0xffff);
}

void initLED()
{
    initTIM9();

    RCC->AHB1ENR |= 1 << 0; // Enable clock for GPIOA

    GPIOA->MODER |= 2 << (LEDLeft * 2);  // Set PA2 to AF mode
    GPIOA->MODER |= 2 << (LEDRight * 2); // Set PA3 to AF mode

    GPIOA->AFR[0] |= AF03 << (LEDLeft * 4);  // Set PA2 to AF03
    GPIOA->AFR[0] |= AF03 << (LEDRight * 4); // Set PA3 to AF03

    TIM9->CCR1 = 0;
    TIM9->CCR2 = 0;
}