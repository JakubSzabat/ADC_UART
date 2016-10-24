#include "stm32f0xx.h"

#define TEMP110_CAL_ADDR ((uint16_t*) ((uint32_t) 0x1FFFF7C2))	//wartosci zadane przez producenta do skalibrowania miernika
#define TEMP30_CAL_ADDR ((uint16_t*) ((uint32_t) 0x1FFFF7B8))
#define VDD_CALIB ((uint16_t) (330))
#define VDD_APPLI ((uint16_t) (300))

volatile int32_t temp;

void ADC_Init(void);
void ADC_Start(void);
void ADC_Stop(void);
int32_t ADC_Read(void);
