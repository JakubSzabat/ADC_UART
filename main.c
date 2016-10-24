#include "stm32f0xx.h"
#include "uart_init.h"


extern volatile int32_t temp;

int main(void){		//main function
	
	SystemCoreClockUpdate();
	GPIO_Init();
	UART_Init();
	ADC_Init();
	
	while(1){
		__WFI();	//suspend till interrupt
	}
}
