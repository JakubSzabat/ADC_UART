#include "stm32f0xx.h"
#include "uart_init.h"
#include "adc_init.h"

int main(void){		//main function

	SystemCoreClockUpdate();
	GPIO_Init();
	ADC_Init();
	UART_Init();

	while(1){
		__WFI();	//suspend till interrupt
	}
}
