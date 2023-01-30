/**
 * Student name: Mark Armdan
 * Student number: 5489776
 *
 */
#include "stm32f4xx.h"
#include "debugled.h"

#define AF01 0x01
#define BIAMotor 14
#define BIBMotor 15
#define AIAMotor 3
#define AIBMotor 8

double kP = 7;
double kI = 0.0015;
double kD = 5;
volatile double i = 0;
volatile double lastError = 0;

void initTIM1()
{
    RCC->APB2ENR |= 1 << 0; // Enable clock for TIM1

    TIM1->PSC = 0;   // Set prescaler to 0
    TIM1->ARR = 255; // Set auto reload register to 255
    TIM1->CNT = 0;   // Reset counter

    TIM1->CCMR1 |= (1 << 5) | (1 << 6);   // Set channel 1 to pwm mode 1
    TIM1->CCMR1 |= (1 << 13) | (1 << 14); // Set channel 2 to pwm mode 1
    TIM1->CCMR2 |= (1 << 5) | (1 << 6);   // Set channel 3 to pwm mode 1
    TIM1->CCER |= 1 << 0;                 // Enable channel 1
    TIM1->CCER |= 1 << 6;                 // Enable channel 2N
    TIM1->CCER |= 1 << 10;                // Enable channel 3
    TIM1->CR1 |= 1 << 0;                  // Counter enable for TIM1
    TIM1->BDTR |= 1 << 15;                // Enable main output
}
void initTIM2()
{
    RCC->APB1ENR |= 1 << 0; // Enable clock for TIM2

    TIM2->PSC = 0;   // Set prescaler to 0
    TIM2->ARR = 255; // Set auto reload register to 255
    TIM2->CNT = 0;   // Reset counter

    TIM2->CCMR1 |= (1 << 13) | (1 << 14); // Set channel 2 to pwm mode 1
    TIM2->CCER |= 1 << 4;                 // Enable channel 2
    TIM2->CR1 |= 1 << 0;                  // Counter enable for TIM2
}

void forward(int speed)
{
    TIM1->CCR1 = speed*0.8;
    TIM1->CCR2 = 0;
    TIM2->CCR2 = 0;
    TIM1->CCR3 = speed;
}

void drive(int speed, int turn_rate)
{
    // If the turn rate is below 20 and above -20, drive straight
    if (turn_rate < 20 && turn_rate > -20)
    {
        TIM1->CCR1 = speed * 0.8;
        TIM1->CCR2 = 0;
        TIM2->CCR2 = 0;
        TIM1->CCR3 = speed;
        return;
    }
    if (turn_rate > 0)
    {
        TIM1->CCR1 = (speed)*0.8;
        TIM1->CCR2 = speed > turn_rate ? speed : turn_rate;
        TIM2->CCR2 = 0;
        TIM1->CCR3 = 0;
    }
    else if (turn_rate < 0)
    {
        TIM1->CCR1 = 0;
        TIM1->CCR2 = 0;
        TIM2->CCR2 = speed * 0.8 > -turn_rate ? speed * 0.8 : -turn_rate;
        TIM1->CCR3 = speed;
    }
}

// A line follower that uses a PID controller to follow a line, the desired outcome if for both sensors to read the same value
void pidLineFollower(uint16_t data[2])
{
    // Turn on the fun LED's when the sensors read a value above 1024
    if(data[0] > 1024) {
        setLEDLeft(0xffff-1);
    }
    else {
        setLEDLeft(0);
    }
    if(data[1] > 1024) {
        setLEDRight(0xffff-1);
    }
    else {
        setLEDRight(0);
    }
    double error = ((double)data[0] * 100) / 4095.0 - ((double)data[1] * 100) / 4095.0; // Calculate the difference between the two sensors, this is the error
    double p = kP * error;                                                              // Calculate the proportional term (compensates for error)
    i += error;                                                                         // Increment the integral term 
    double tempI = kI * i;                                                              // Calculate the integral term (overcompensates for accumulated error)
    double d = kD * (error - lastError);                                                // Calculate the derivative term (compensates for sudden changes in error)

    double output = p + tempI + d; // Calculate the output
    drive(185, output);

    lastError = error; // Set the last error to the current error
}

void stop()
{
    TIM1->CCR1 = 0;
    TIM1->CCR2 = 0;
    TIM2->CCR2 = 0;
    TIM1->CCR3 = 0;
}

void setLeftMotor(int speed)
{
    TIM1->CCR3 = speed;
}

void setRightMotor(int speed)
{
    TIM1->CCR1 = speed;
}

void setupMotors()
{
    initTIM1();
    initTIM2();
    initLED();

    RCC->AHB1ENR |= 1 << 1; // Enable clock for GPIOB
    RCC->AHB1ENR |= 1 << 0; // Enable clock for GPIOA

    GPIOB->MODER |= 1 << (BIBMotor * 2 + 1); // Set pin B15 to alternate function
    GPIOB->MODER &= ~(1 << (BIBMotor * 2));

    GPIOB->MODER |= 1 << (BIAMotor * 2 + 1); // Set pin B14 to alternate function
    GPIOB->MODER &= ~(1 << (BIAMotor * 2));

    GPIOB->MODER |= 1 << (AIAMotor * 2 + 1); // Set pin B3 to alternate function
    GPIOB->MODER &= ~(1 << (AIAMotor * 2));

    GPIOA->MODER |= 1 << (AIBMotor * 2 + 1); // Set pin A8 to alternate function
    GPIOA->MODER &= ~(1 << (AIBMotor * 2));

    GPIOB->AFR[1] |= AF01 << 24; // Set the alternate function register for pin B14 to 0001
    GPIOB->AFR[1] |= AF01 << 28; // Pin B15
    GPIOB->AFR[0] |= AF01 << 12; // Pin B3
    GPIOA->AFR[1] |= AF01 << 0;  // Pin A8

    TIM1->CCR1 = 0; // AIB Right wheel forward
    TIM1->CCR2 = 0; // BIA
    TIM1->CCR3 = 0; // BIB Left wheel forward
    TIM2->CCR2 = 0; // AIA
}