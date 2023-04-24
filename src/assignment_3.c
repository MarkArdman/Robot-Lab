#include "stm32f4xx.h"
#include "delay.h"
#include "debugled.h"
#include "motors.h"
#include "adc.h"
#include "ultrasound.h"

void assignment3(void)
{
    initADC();
    initDelay();
    setupMotors();

    while (1)
    {
        pidLineFollower(data);
    }
}