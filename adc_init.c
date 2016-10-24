#include "adc_init.h"
#include "uart_init.h"

void ADC_Init(void){
	/* This code selects the HSI14 as clock source. */
	/* (1) Enable the peripheral clock of the ADC */
	/* (2) Start HSI14 RC oscillator */
	/* (3) Wait HSI14 is ready */
	/* (4) Select HSI14 by writing 00 in CKMODE (reset value) */
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; /* (1) */
	RCC->CR2 |= RCC_CR2_HSI14ON; /* (2) */
	while ((RCC->CR2 & RCC_CR2_HSI14RDY) == 0) /* (3) */
	{
	}
	ADC1->CFGR2 &= (~ADC_CFGR2_CKMODE); /* (4) */


	////////////////////////////////////
	/* (1) Ensure that ADEN = 0 */
	/* (2) Clear ADEN */
	/* (3) Launch the calibration by setting ADCAL */
	/* (4) Wait until ADCAL=0 */
	if ((ADC1->CR & ADC_CR_ADEN) != 0) /* (1) */
	{
	ADC1->CR &= (uint32_t)(~ADC_CR_ADEN); /* (2) */
	}
	ADC1->CR |= ADC_CR_ADCAL; /* (3) */
	while ((ADC1->CR & ADC_CR_ADCAL) != 0) /* (4) */
	{
	}


	////////////////////////////////////
	/* (1) Enable the ADC */
	/* (2) Wait until ADC ready */
	ADC1->CR |= ADC_CR_ADEN; /* (1) */
	while ((ADC1->ISR & ADC_ISR_ADRDY) == 0) /* (2) */
	{
	}


	////////////////////////////////////
	/* (1) Select CHSEL16 for temperature sensor */
	/* (2) Select a sampling mode of 111 i.e. 239.5 ADC clk to be greater than
	17.1us */
	/* (3) Wake-up the Temperature sensor (only for VBAT, Temp sensor and
	VRefInt) */
	ADC1->CHSELR = ADC_CHSELR_CHSEL16; /* (1) */
	ADC1->SMPR |= ADC_SMPR1_SMPR_0 | ADC_SMPR1_SMPR_1 | ADC_SMPR1_SMPR_2; /* (2) */
	ADC->CCR |= ADC_CCR_TSEN; /* (3) */


	////////////////////////////////////
	/* Configure NVIC for ADC */
	/* (7) Enable Interrupt on ADC */
	/* (8) Set priority for ADC */
	ADC1->IER = ADC_IER_EOCIE;	//wlacze przerwania dla end of convertion
	NVIC_EnableIRQ(ADC1_COMP_IRQn); /* (7) */
	NVIC_SetPriority(ADC1_COMP_IRQn,2); /* (8) */		
}

void ADC_Start(void){
	ADC1->CR |= ADC_CR_ADSTART;
}

void ADC_Stop(void){
	ADC1->CR &= ~ADC_CR_ADSTART;
}

int32_t ADC_Read(void){
	int32_t temperature; /* will contain the temperature in degree Celsius */
	temperature = (((int32_t) ADC1->DR * VDD_APPLI / VDD_CALIB) - (int32_t) *TEMP30_CAL_ADDR );
	temperature = temperature * (int32_t)(110 - 30);
	temperature = temperature / (int32_t)(*TEMP110_CAL_ADDR - *TEMP30_CAL_ADDR);
	temperature = temperature + 30;
	return temperature;
}

void ADC1_COMP_IRQHandler(void){
	temp=ADC_Read();
	ADC_Stop();
	DEC_Place();	//form new baud rate in table
	USART1->TDR = baudrate_tab[send++];	//start BR transmision
}
