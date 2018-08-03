#ifndef MEASUREMENT_H
#define MEASUREMENT_H

/*
 * Algorithm of measure is as follows:
 *
 * Every measurement is executed after TIM1 interrupt (max distance + something)
 * and must end max on TIM3 interrupt
 *
 * 1. set high output to init sonar
 * 3. start TIM2
 * 3. after 10us interrupt of TIM2 set low output
 * 4. on interrupt rising edge, start TIM3
 * 5. on interrupt falling edge, stop TIM3 and measure value
 * 6. if TIM3 interrupt occurs, it means that there no visible object
 *
 * used timers:
 * TIM1 - period of single measurement
 * TIM2 - period of high state to start sonar
 * TIM3 - measure high state on distance output pin
 *
 */


#define NO_SONARS 14
#define BAD_DISTANCE 65535
#define MAX_VALID_CNT 50000

void TIM1Interrput();
void TIM2Interrput();
void TIM3Interrput();
void EXTI2_3Interrupt();
void SetTrigger(uint8_t);
void UpdateSonarId();
uint32_t isRisingEchoEdge();

// stores distance to obstacle in mm
extern volatile uint16_t sonarDistances[NO_SONARS];

#endif
