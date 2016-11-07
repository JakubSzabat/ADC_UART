#ifndef UART_INIT_H
#define UART_INIT_H
#include "stm32f0xx.h"

#define DEF_BR 9600

uint8_t baudrate_tab[13] = {0x0D,0,0,0,0,0,0,0,0,0,0,0,0x0A};	//variables declaration
uint8_t send = 0;
int32_t tempAvr = 0;
int32_t tempMax = 0;
int32_t tempMin = 100;

void UART_Init(void);	//function declaration
void GPIO_Init(void);
void DEC_Place(void);
void AVG_VAL(void);
void MaxMin_VAL(void);

#endif
