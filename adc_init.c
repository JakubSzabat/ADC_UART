#include "adc_init.h"
#include "uart_init.h"

volatile int32_t temp = 0;

void ADC_Init(void);
void ADC_Start(void);
void ADC_Stop(void);
int32_t ADC_Read(void);

void ADC_Init(void){
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; // Enable the peripheral clock of the ADC
	RCC->CR2 |= RCC_CR2_HSI14ON; // Start HSI14 RC oscillator
	while ((RCC->CR2 & RCC_CR2_HSI14RDY) == 0) // Wait HSI14 is ready
	{
	}
	ADC1->CFGR2 &= (~ADC_CFGR2_CKMODE); //Select HSI14 by writing 00 in CKMODE (reset value)

	if ((ADC1->CR & ADC_CR_ADEN) != 0) //Ensure that ADEN = 0
	{
	ADC1->CR &= (uint32_t)(~ADC_CR_ADEN); //Clear ADEN 
	}
	ADC1->CR |= ADC_CR_ADCAL; //Launch the calibration by setting ADCAL
	while ((ADC1->CR & ADC_CR_ADCAL) != 0) //Wait until ADCAL=0
	{
	}

	ADC1->CR |= ADC_CR_ADEN; //Enable the ADC
	while ((ADC1->ISR & ADC_ISR_ADRDY) == 0) //Wait until ADC ready
	{
	}

	ADC1->CHSELR = ADC_CHSELR_CHSEL16; //Select CHSEL16 for temperature sensor
	ADC1->SMPR |= ADC_SMPR_SMP_0 | ADC_SMPR_SMP_1 | ADC_SMPR_SMP_2; //Select a sampling mode of 111 i.e.
																																				//239.5 ADC clk to be greater than 4us -- ~30us
	ADC->CCR |= ADC_CCR_TSEN; //Wake-up the Temperature sensor

	ADC1->IER = ADC_IER_EOCIE;	//Enable Interrupt on ADC end of convertion
	NVIC_EnableIRQ(ADC1_COMP_IRQn);
	NVIC_SetPriority(ADC1_COMP_IRQn,2);
}

void ADC_Start(void){
	ADC1->CR |= ADC_CR_ADSTART;	//start convertion
}

void ADC_Stop(void){
	ADC1->CR |= ADC_CR_ADSTP;	//stop convertion (ensure it)
}

int32_t ADC_Read(void){
	int32_t temperature; // will contain the temperature in degree Celsius
	temperature = (((int32_t) ADC1->DR * VDD_APPLI / VDD_CALIB) - (int32_t) *TEMP30_CAL_ADDR );
	temperature = temperature * (int32_t)(110 - 30);
	temperature = temperature / (int32_t)(*TEMP110_CAL_ADDR - *TEMP30_CAL_ADDR);
	temperature = temperature + 30;
	return temperature;
}

void ADC1_COMP_IRQHandler(void){
	temp=ADC_Read();
	AVG_VAL();
	MaxMin_VAL();
	ADC_Stop();
	DEC_Place();	//form new val in table
	USART1->TDR = value_tab[send++];	//start BR transmision
}
