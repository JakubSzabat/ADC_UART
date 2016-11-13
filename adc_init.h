#ifndef ADC_INIT_H
#define ADC_INIT_H
#include "stm32f0xx.h"

#define TEMP110_CAL_ADDR ((uint16_t*) ((uint32_t) 0x1FFFF7C2))	//temp sensor calibration values
#define TEMP30_CAL_ADDR ((uint16_t*) ((uint32_t) 0x1FFFF7B8))
#define VDD_CALIB ((uint16_t) (330))
#define VDD_APPLI ((uint16_t) (300))

extern volatile int32_t temp;

extern void ADC_Init(void);
extern void ADC_Start(void);
extern void ADC_Stop(void);
extern int32_t ADC_Read(void);

#endif
