#include "stm32f4xx.h"
#include "delay.h"
#include "motors.h"

void assignment1(void)
{
    initDelay();
    setupMotors();
    forward(180);
    delayMs(3000); // Wait 3 seconds
    forward(250);
    delayMs(3000); // Wait 3 seconds

    // Decrease duty cycle by 1 until 0
    for (int i = 250; i >= 0; i--)
    {
        forward(i);
        delayMs(6); // Wait 6 ms
    }
}
