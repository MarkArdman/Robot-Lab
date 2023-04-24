#include "stm32f4xx.h"

void delayMs(int ms)
{
    TIM5->CNT = 0;                  // Reset counter
    TIM5->CR1 |= 1 << 0;            // Enable timer
    while (TIM5->CNT < (1000 * ms)) // Wait for counter to reach ms
        ;
    TIM5->CR1 &= ~(1 << 0); // Disable timer
}

void delayUs(int us)
{
    TIM5->CNT = 0;         // Reset counter
    TIM5->CR1 |= 1 << 0;   // Enable timer
    while (TIM5->CNT < us) // Wait for counter to reach us
        ;
    TIM5->CR1 &= ~(1 << 0); // Disable timer
}

uint32_t getCurrentTimestamp()
{
    return TIM5->CNT;
}

void initTIM5()
{
    RCC->APB1ENR |= 1 << 3; // Enable clock for TIM5

    TIM5->PSC = 15;             // Set prescaler to 16
    TIM5->ARR = 0xffffffff - 1; // Set auto reload register to a very large number
    TIM5->CNT = 0;              // Reset counter
    TIM5->EGR |= 1 << 0;        // Generate update event
}

void initDelay()
{
    initTIM5();
}