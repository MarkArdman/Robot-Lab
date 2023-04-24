#include "stm32f4xx.h"

void setupMotors();
void forward(int speed);
void stop();
void setLeftMotor(int speed);
void setRightMotor(int speed);
void drive(int speed, int turn_rate);
void pidLineFollower(uint16_t data[2]);