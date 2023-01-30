/**
 * Student name: Mark Armdan
 * Student number: 5489776
 *
 */
#include "stm32f4xx.h"
#include "motors.h"
#include "delay.h"
#include "ultrasound.h"
#include "debugled.h"
#include "adc.h"

volatile int distance_front = 0;
volatile int distance_side = 0;
volatile int obstacle_flag = 0;
volatile int side_obstacle_flag = 0;
volatile int temp_distance = 0;
volatile int time_stamp = 0;

void callbackImplForward(uint32_t pulse_width)
{
    distance_front = pulse_width / 58;
}

void callbackImplSide(uint32_t pulse_width)
{
    distance_side = pulse_width / 58;
}

void assignment4(void)
{
    initADC();
    initDelay();
    initLED();
    setupMotors();
    initUltraSoundSensors(callbackImplForward, callbackImplSide);
    obstacle_flag = 0;
    side_obstacle_flag = 0;
    setLEDLeft(0);
    setLEDRight(0);

    delayMs(2000);

    while (1)
    {
        // If the side obstacle flag is 0, check if there is an obstacle in front of the robot
        if (side_obstacle_flag == 0)
        {
            // If there is no obstacle in front, follow the line
            if (obstacle_flag == 0)
            {
                if (distance_front > 15)
                {
                    time_stamp = 0;
                    pidLineFollower(data);
                }
                else
                {
                    obstacle_flag = 1;
                    stop();
                }
            }
            // If there is an obstacle in front, remember the distance, turn until the side obstacle flag is 1
            else
            {
                if (distance_side < 25)
                {
                    obstacle_flag = 0;
                    side_obstacle_flag = 1;
                    temp_distance = distance_side;
                    stop();
                }
                else
                {
                    time_stamp++;
                    drive(185, -30);
                }
            }
        }
        // If the side obstacle flag is 1, go around the obstacle
        else
        {
            // If the distance is less than 7 times the remembered distance, drive until the side sensor cant see the obstacle
            if (distance_side < temp_distance * 7)
            {
                drive(185, 0);
                if (data[0] > 0x0400 || data[1] > 0x0400)
                {
                    setLEDLeft(0xffff - 1);
                    setLEDRight(0xffff - 1);
                    side_obstacle_flag = 0;
                }
            }
            // If the distance is more than 7 times the remembered distance, turn and drive until the obstacle is spotted again
            else
            {
                drive(185, 0);
                for (int i = 0; i < time_stamp * 1.2; i++)
                {
                    if (data[0] > 0x0400 || data[1] > 0x0400)
                    {
                        setLEDLeft(0xffff - 1);
                        setLEDRight(0xffff - 1);
                        side_obstacle_flag = 0;
                    }
                }
                drive(185, 30);
                for (int i = 0; i < time_stamp * 1.35; i++)
                {
                    if (data[0] > 0x0400 || data[1] > 0x0400)
                    {
                        setLEDLeft(0xffff - 1);
                        setLEDRight(0xffff - 1);
                        side_obstacle_flag = 0;
                    }
                }
                drive(185, 0);
                while (distance_side > temp_distance * 4)
                {
                    if (data[0] > 0x0400 || data[1] > 0x0400)
                    {
                        setLEDLeft(0xffff - 1);
                        setLEDRight(0xffff - 1);
                        side_obstacle_flag = 0;
                    }
                    delayMs(20);
                }
            }
        }
    }
}