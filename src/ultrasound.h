/**
 * Student name: Mark Armdan
 * Student number: 5489776
 * 
*/
#include "stm32f4xx.h"

void initUltraSoundSensors(void (*callback)(uint32_t), void (*callback2)(uint32_t));

void updateForwardCallback(void (*callback)(uint32_t));

void updateSideCallback(void (*callback)(uint32_t));