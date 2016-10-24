#include "stm32f0xx.h"

#define DEF_BR 9600

uint8_t static baudrate_tab[8] = {0x0D,0,0,0,0,0,0,0x0A};	//variables declaration
uint8_t static send = 0;

void UART_Init(void);	//function declaration
void GPIO_Init(void);
void DEC_Place(void);
