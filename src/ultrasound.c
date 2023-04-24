#include "stm32f4xx.h"
#include "debugled.h"

#define AF02 0x02
#define AF03 0x03
#define UltraSonicTriggerFront 7
#define UltraSonicTriggerSide 6
#define UltraSonicEchoFront 8
#define UltraSonicEchoSide 9

volatile uint32_t time_stamp1_front = 0;
volatile uint32_t time_stamp2_front = 0;
volatile uint32_t width_front = 0;
volatile unsigned int interrupt_front_flag = 0;
volatile void (*frontCallback)(uint32_t);

volatile uint32_t time_stamp1_side = 0;
volatile uint32_t time_stamp2_side = 0;
volatile uint32_t width_side = 0;
volatile unsigned int interrupt_side_flag = 0;
volatile void (*sideCallback)(uint32_t);

void TIM4_IRQHandler(void)
{
    uint32_t time = TIM4->CCR3;
    // Check if interrupt flag is set
    if (interrupt_front_flag == 0)
    {
        time_stamp1_front = time; // Store time stamp 1
        interrupt_front_flag++;   // Set flag to 1
    }
    else
    {
        time_stamp2_front = time; // Store time stamp 2

        // Calculate width of pulse
        if (time_stamp2_front >= time_stamp1_front)
        {
            width_front = time_stamp2_front - time_stamp1_front;
        }
        else
        {
            width_front = (0xffff - time_stamp1_front) + time_stamp2_front;
        }

        // If callback is not null, call it
        if (frontCallback != ((void *)0))
        {
            frontCallback(width_front);
        }

        TIM4->CNT = 0;          // Reset counter
        interrupt_front_flag--; // Set flag to 0
    }
    TIM4->SR &= ~(1 << 3); // Clear interrupt flag
}

void TIM1_TRG_COM_TIM11_IRQHandler(void)
{

    uint32_t time = TIM11->CCR1;
    // Check if interrupt flag is set
    if (interrupt_side_flag == 0)
    {
        time_stamp1_side = time; // Store time stamp 1
        interrupt_side_flag++;   // Set flag to 1
    }
    else
    {
        time_stamp2_side = time; // Store time stamp 2

        // Calculate width of pulse
        if (time_stamp2_side >= time_stamp1_side)
        {
            width_side = time_stamp2_side - time_stamp1_side;
        }
        else
        {
            width_side = (0xffff - time_stamp1_side) + time_stamp2_side;
        }

        // If callback is not null, call it
        if (sideCallback != ((void *)0))
        {
            sideCallback(width_side);
        }

        TIM11->CNT = 0;        // Reset counter
        interrupt_side_flag--; // Set flag to 0
    }
    TIM11->SR &= ~(1 << 1); // Clear interrupt flag
}

void initTIM4()
{
    RCC->APB1ENR |= 1 << 2; // Enable clock for TIM4

    TIM4->PSC = 15;        // Set prescaler to 15
    TIM4->CCMR2 |= 1 << 0; // Set channel 3 capture/compare to input to TI4
    TIM4->CCER |= 1 << 8;  // Enable capture/compare 3
    TIM4->CCER |= 5 << 9;  // Both edges non-inverted

    NVIC->IP[30] = 1;                        // Set priority for TIM4 interrupt
    NVIC->ISER[30 >> 5] |= (1 << (30 % 32)); // Enable interrupt for TIM4

    TIM4->DIER |= 1 << 3; // Enable interrupt for capture/compare 3
    TIM4->CR1 |= 1 << 0;  // Counter enable for TIM4
    TIM4->CNT = 0;        // Reset counter
    TIM4->EGR |= 1 << 0;  // Generate update event
}

void initTIM11()
{
    RCC->APB2ENR |= 1 << 18; // Enable clock for TIM11

    TIM11->PSC = 15;        // Set prescaler to 16
    TIM11->CCMR1 |= 1 << 0; // Set channel 1 to capture/compare to input to TI1
    TIM11->CCER |= 1 << 0;  // Enable capture/compare 1
    TIM11->CCER |= 5 << 1;  // Both edges non-inverted

    NVIC->IP[26] = 2;                        // Set priority for TIM11 interrupt
    NVIC->ISER[26 >> 5] |= (1 << (26 % 32)); // Enable interrupt for TIM11

    TIM11->DIER |= 1 << 1; // Enable interrupt for capture/compare 1
    TIM11->CR1 |= 1 << 0;  // Counter enable for TIM11
    TIM11->CNT = 0;        // Reset counter
    TIM11->EGR |= 1 << 0;  // Generate update event
}

void initTIM3()
{
    RCC->APB1ENR |= 1 << 1; // Enable clock for TIM3

    TIM3->PSC = 15;    // Set prescaler to 16
    TIM3->ARR = 25000; // Set auto reload register to a high value to avoid echo
    TIM3->CNT = 0;     // Reset counter

    TIM3->CCMR1 |= 6 << 12; // Set channel 2 to pwm mode 1
    TIM3->CCMR1 |= 6 << 4;  // Set channel 1 to pwm mode 1
    TIM3->CCER |= 1 << 4;   // Enable channel 2
    TIM3->CCER |= 1 << 0;   // Enable channel 1
    TIM3->CR1 |= 1 << 0;    // Counter enable for TIM3
    TIM3->EGR |= 1 << 0;    // Generate update event
}

void updateForwardCallback(void (*callbackImpl)(uint32_t))
{
    frontCallback = callbackImpl;
}

void updateSideCallback(void (*callbackImpl)(uint32_t))
{
    sideCallback = callbackImpl;
}

void initUltraSoundSensors(void (*callbackImpl)(uint32_t), void (*callback2Impl)(uint32_t))
{
    frontCallback = callbackImpl;
    sideCallback = callback2Impl;
    initTIM4();
    initTIM3();
    initTIM11();
    initLED();

    RCC->AHB1ENR |= 1 << 0; // Enable clock for GPIOA
    RCC->AHB1ENR |= 1 << 1; // Enable clock for GPIOB

    GPIOA->MODER |= 1 << (UltraSonicTriggerFront * 2 + 1); // Set pin A7 to alternate function
    GPIOA->MODER &= ~(1 << (UltraSonicTriggerFront * 2));  // Set pin A7 to alternate function

    GPIOB->MODER |= (1 << (UltraSonicEchoFront * 2 + 1)); // Set pin B8 to alternate function
    GPIOB->MODER &= ~(1 << (UltraSonicEchoFront * 2));    // Set pin B8 to alternate function

    GPIOA->OSPEEDR |= 1 << UltraSonicTriggerFront; // Set pin A7 to high speed

    GPIOB->AFR[1] |= AF02 << 0;  // Set the alternate function register for pin B8 to 0010
    GPIOA->AFR[0] |= AF02 << 28; // Set the alternate function register for pin A7 to 0010

    TIM3->CCR2 = 15; // Set duty cycle to 15

    GPIOA->MODER |= 1 << (UltraSonicTriggerSide * 2 + 1); // Set pin A6 to alternate function
    GPIOA->MODER &= ~(1 << (UltraSonicTriggerSide * 2));  // Set pin A6 to alternate function

    GPIOB->MODER |= (1 << (UltraSonicEchoSide * 2 + 1)); // Set pin B9 to alternate function
    GPIOB->MODER &= ~(1 << (UltraSonicEchoSide * 2));    // Set pin B9 to alternate function

    GPIOA->OSPEEDR |= 1 << UltraSonicTriggerSide; // Set pin A6 to high speed

    GPIOB->AFR[1] |= AF03 << 4;  // Set the alternate function register for pin B9 to 0011
    GPIOA->AFR[0] |= AF02 << 24; // Set the alternate function register for pin A6 to 0010

    TIM3->CCR1 = 15; // Set duty cycle to 15
}