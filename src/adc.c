/**
 * Student name: Mark Armdan
 * Student number: 5489776
 *
 */
#include "stm32f4xx.h"
#include "delay.h"

#define IRINPUTLEFT 0
#define IRINPUTRIGHT 1

uint16_t data[2];

void initADC1()
{
    RCC->APB2ENR |= 1 << 8; // Enable clock for ADC1
    RCC->AHB1ENR |= 1 << 0; // Enable clock for GPIOA
    ADC->CCR |= 2 << 16;    // Set prescaler to 6

    ADC1->CR1 |= (1 << 8);   // Disable scan mode
    ADC1->CR1 &= ~(3 << 24); // Set resolution to 12 bits
    ADC1->CR2 |= 1 << 1;     // Enable continuous conversion
    ADC1->CR2 |= 1 << 10;    // Enable end of conversion after every conversion
    ADC1->CR2 &= ~(1 << 11); // Enable DA right alignment
    ADC1->CR2 |= 1 << 8;     // Enable DMA
    ADC1->CR2 |= 1 << 9;     // Enable DDS

    ADC1->SMPR2 &= ~(7 << (IRINPUTLEFT * 3) | 7 << (IRINPUTRIGHT * 3)); // Set sample time to 3 cycles
    ADC1->SQR1 |= (1 << 20);                                            // Set number of conversions to 1
    ADC1->SQR3 |= IRINPUTLEFT << 0;                                     // Set channel to be converted first
    ADC1->SQR3 |= IRINPUTRIGHT << 5;                                    // Set channel to be converted second

    GPIOA->MODER |= 3 << (IRINPUTLEFT * 2);  // Set pin to analog mode
    GPIOA->MODER |= 3 << (IRINPUTRIGHT * 2); // Set pin to analog mode

    ADC1->CR2 |= 1 << 0; // Enable ADC1
    delayMs(1);          // Wait for ADC to stabilize
}

void startADC1()
{
    ADC1->SR = 0;         // Clear status register
    ADC1->CR2 |= 1 << 30; // Start conversion
}

void initDMA2()
{
    RCC->AHB1ENR |= 1 << 22; // Enable clock for DMA2

    DMA2_Stream0->CR &= ~(3 << 6);           // Peripheral to memory
    DMA2_Stream0->CR |= 1 << 8;              // Circular mode
    DMA2_Stream0->CR |= 1 << 10;             // Increment memory pointer
    DMA2_Stream0->CR |= (1 << 11 | 1 << 13); // Data size 16 bits

    DMA2_Stream0->CR &= ~(7 << 25); // Channel 0

    DMA2_Stream0->NDTR = 2; // Number of data items to transfer
    DMA2_Stream0->PAR = (uint32_t)&ADC1->DR;
    DMA2_Stream0->M0AR = (uint32_t)data;

    DMA2_Stream0->CR |= 1 << 0; // Enable DMA2
}

void initADC() {
    initDelay();
    initADC1();
    initDMA2();
    startADC1();
}