#ifndef UART_INIT_H
#define UART_INIT_H
#include "stm32f0xx.h"

#define DEF_BR 9600 //default baudrate

extern uint8_t value_tab[13];	//variables declaration
extern uint8_t volatile send;
extern int32_t tempAvr;
extern int32_t tempMax;
extern int32_t tempMin;

extern void UART_Init(void);	//function declaration
extern void GPIO_Init(void);
extern void DEC_Place(void);
extern void AVG_VAL(void);
extern void MaxMin_VAL(void);

#endif
