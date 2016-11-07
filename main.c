#include "stm32f0xx.h"

extern volatile int32_t temp;
extern void ADC_Init(void);
extern void	UART_Init(void);
extern void	GPIO_Init(void);

int main(void){		//main function
	
	SystemCoreClockUpdate();
	GPIO_Init();
	ADC_Init();
	UART_Init();
	
	while(1){
		__WFI();	//suspend till interrupt
	}
}
