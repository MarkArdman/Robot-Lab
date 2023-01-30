/**
 * Student name: Mark Armdan
 * Student number: 5489776
 *
 */
#include "stm32f4xx.h"
#include "motors.h"
#include "delay.h"
#include "debugled.h"
#include "adc.h"

#define RotationSensorLeft 0
#define RotationSensorRight 11

uint32_t volatile left_tick = 0;
uint32_t volatile right_tick = 0;

void TIM3_IRQHandler(void)
{
    if(TIM1->CCR1 > 0)
    {
        right_tick++; // Increment tick on the right wheel if it's going forward
    }
    if(TIM2->CCR2 > 0)
    {
        right_tick--; // Decrement tick on the right wheel if it's going backwards
    }
    TIM3->SR &= ~(1 << 3); // Clear interrupt flag
}

void TIM1_CC_IRQHandler(void)
{
    if(TIM1->CCR3 > 0)
    {
        left_tick++; // Increment tick on the left wheel if it's going forward
    }
    if(TIM1->CCR2 > 0)
    {
        left_tick--; // Decrement tick on the right wheel if it's going backwards
    }
    TIM1->SR &= ~(1 << 4); // Clear interrupt flag
}

void addInitTIM3()
{
    RCC->APB1ENR |= 1 << 1; // Enable clock for TIM3

    TIM3->PSC = 15;        // Set prescaler to 16
    TIM3->CCMR2 |= 1 << 0; // Set channel 3 to capture/compare mode
    TIM3->CCER |= 1 << 8;  // Enable capture/compare channel 3

    NVIC->IP[29] = 3;                        // Set priority for TIM3 interrupt
    NVIC->ISER[29 >> 5] |= (1 << (29 % 32)); // Enable interrupt for TIM3

    TIM3->DIER |= 1 << 3; // Enable interrupt for capture/compare 3
    TIM3->CR1 |= 1 << 0;  // Enable timer
    TIM3->CNT = 0;        // Reset counter
    TIM3->EGR |= 1 << 0;  // Generate update event
}

void addInitTIM1()
{
    RCC->APB2ENR |= 1 << 0; // Enable clock for TIM1

    TIM1->CCMR2 |= 1 << 8; // Set channel 4 to capture/compare mode
    TIM1->CCER |= 1 << 12; // Enable capture/compare channel 4

    NVIC->IP[27] = 3;                        // Set priority for TIM1 interrupt
    NVIC->ISER[27 >> 5] |= (1 << (27 % 32)); // Enable interrupt for TIM1

    TIM1->DIER |= 1 << 4; // Enable interrupt for capture/compare 4
    TIM1->CR1 |= 1 << 0;  // Enable timer
    TIM1->CNT = 0;        // Reset counter
    TIM1->EGR |= 1 << 0;  // Generate update event
}

void assignment5()
{
    int distance_to_travel = 124;
    int ticks_to_travel = distance_to_travel * 45;

    setupMotors();
    initDelay();
    initADC();
    initLED();

    addInitTIM3();
    addInitTIM1();

    RCC->AHB1ENR |= 1 << 1; // Enable clock for GPIOB
    RCC->AHB1ENR |= 1 << 0; // Enable clock for GPIOA

    GPIOB->MODER |= (1 << (RotationSensorLeft * 2 + 1)); // Set pin B0 to alternate function
    GPIOB->MODER &= ~(1 << (RotationSensorLeft * 2));    // Set pin B0 to alternate function

    GPIOA->MODER |= (1 << (RotationSensorRight * 2 + 1)); // Set pin A11 to alternate function
    GPIOA->MODER &= ~(1 << (RotationSensorRight * 2));    // Set pin A11 to alternate function

    GPIOB->AFR[0] |= 0x0002 << 0;  // Set the alternate function register for pin B0 to 0010
    GPIOA->AFR[1] |= 0x0001 << 12; // Set the alternate function register for pin A11 to 0010

    delayMs(1000);

    // Follow the line until the robot has travelled the desired distance
    while (left_tick + right_tick < ticks_to_travel)
    {
        pidLineFollower(data);
    }
    stop();
}