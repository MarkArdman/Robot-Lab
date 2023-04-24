#include "stm32f4xx.h"
#include "motors.h"
#include "delay.h"
#include "ultrasound.h"

void callbackImpl(uint32_t pulse_width)
{
    // If the pulse width is less than 15 cm, stop the robot, otherwise move forward
    if (pulse_width / 58 < 15)
    {
        stop();
    }
    else
    {
        forward(250);
    }
}

void assignment2(void)
{
    initDelay();
    setupMotors();
    initUltraSoundSensors(callbackImpl, ((void *)0));

    forward(250);

    // To prevent the program from exiting
    while (1)
        ;
}