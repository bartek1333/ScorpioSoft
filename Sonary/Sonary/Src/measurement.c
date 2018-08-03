/*
 * measurement.c
 *
 *  Created on: 30.04.2018
 *      Author: bartek
 */

#include "stm32f0xx.h"
#include "measurement.h"
#include "main.h"

// current sonar
volatile static uint8_t currentSonarId;
// stores distances in mm
// distance equal 65535 is invalid
volatile uint16_t sonarDistances[NO_SONARS];

void TIM1Interrput()
{
	//update sonar number
	UpdateSonarId();
	// generate trigger impulse
	SetTrigger(1);
	TIM2->CR1 |= TIM_CR1_CEN;
	LED1_GPIO_Port->ODR |= LED1_Pin;
}

void TIM2Interrput()
{
	TIM2->CR1 &= ~TIM_CR1_CEN;
	TIM2->CNT = 0;
	TIM2->SR = 0;
	SetTrigger(0);
}

void TIM3Interrput()
{
	TIM3->CR1 &= ~TIM_CR1_CEN;
	TIM3->CNT = 0;
	TIM3->SR = 0;
	// high state too long, set BAD_DISTANCE constant
	sonarDistances[currentSonarId] = BAD_DISTANCE;
}

void EXTI2_3Interrupt()
{
	if(isRisingEchoEdge())
	{
		TIM3->CR1 |= TIM_CR1_CEN;
	}
	else // falling edge
	{
		if(TIM3->CNT > 0 && TIM3->CNT < MAX_VALID_CNT)
		{
			TIM3->CR1 &= ~TIM_CR1_CEN;
			// CNT stores us, so divided it by 1000, gives ms
			double tmpCNT = (double)TIM3->CNT / 1000.0;
			// ms * 340m/s gives distance in milimeters
			sonarDistances[currentSonarId] = (uint16_t)((tmpCNT * 340.0) / 2.0);
			TIM3->CNT = 0;
			TIM3->SR = 0;
		}
	}
}


// set sonar trigger pin low/high
// odr = 1 high  odr = 0 low
void SetTrigger(uint8_t odr)
{
	if(odr)
	{
		switch(currentSonarId)
		{
		case 0:
			TRIG_1_GPIO_Port->ODR |= TRIG_1_Pin;
			break;
		}
	}
	else
	{
		switch(currentSonarId)
		{
		case 0:
			TRIG_1_GPIO_Port->ODR &= ~TRIG_1_Pin;
			break;
		}
	}
}

uint32_t isRisingEchoEdge()
{
	switch(currentSonarId)
	{
	case 0:
		return ECHO_1_GPIO_Port->IDR & ECHO_1_Pin;
	// should never be done
	// only for avoid compiler errors
	default:
		return 0;
	}
}

void UpdateSonarId()
{
	// just to demonstrate
	currentSonarId = 0;
	/*
	if(++currentSonarId == 14)
		currentSonarId = 0;
	*/
}
