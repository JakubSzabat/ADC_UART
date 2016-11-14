#include "uart_init.h"
#include "adc_init.h"

uint8_t value_tab[13] = {0x0D,0,0,0,0,0,0,0,0,0,0,0,0x0A};	//variables declaration
uint8_t volatile send = 0;
int32_t tempAvr = 0;
int32_t tempMax = INT32_MIN;
int32_t tempMin = INT32_MAX;

void UART_Init(void);	//function declaration
void GPIO_Init(void);
void DEC_Place(void);
void AVG_VAL(void);
void MaxMin_VAL(void);


__INLINE void GPIO_Init(void){
	RCC->AHBENR  |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOCEN;		//GPIOA clock enable
	GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODER9 | GPIO_MODER_MODER10))
								 | GPIO_MODER_MODER9_1 | GPIO_MODER_MODER10_1;	//PA9, PA10 as an alternate function
  GPIOA->AFR[1] = (GPIOA->AFR[1] &~ (GPIO_AFRH_AFRH1)) | (1<<((9-8)*4));	//alternate function connected to uart PA9,10
  GPIOA->AFR[1] = (GPIOA->AFR[1] &~ (GPIO_AFRH_AFRH2)) | (1<<((10-8)*4));	//

	GPIOC->MODER = (GPIOA->MODER & ~GPIO_MODER_MODER3)| GPIO_MODER_MODER3_1;	//PC3 as alternate function
	GPIOC->AFR[0] = (GPIOA->AFR[0] &~ (GPIO_AFRL_AFRL3)) | GPIO_AFRL_AFRL3;
}

__INLINE void UART_Init(void){

	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;	//UART clock enable

	USART1->BRR = SystemCoreClock / DEF_BR; //set default BR
	USART1->CR1 = USART_CR1_TE | USART_CR1_RXNEIE | USART_CR1_RE | USART_CR1_UE | USART_CR1_TCIE;; //enable uart rx,tx,interrupts

	USART1->CR2 &= (uint32_t)~((uint32_t)USART_CR2_ABRMODE); //ABR config
  //USART1->CR2 |= USART_CR2_ABRMODE_0;
	USART1->CR2 |= USART_CR2_ABREN; //ABR enable

	while((USART1->ISR & USART_ISR_REACK) == (uint16_t)RESET){}	//wait for ACK flag from RX and TX
	while((USART1->ISR & USART_ISR_TEACK) == (uint16_t)RESET){}	//
	while((USART1->ISR & USART_ISR_ABRF)  == (uint16_t)RESET){}	//wait for ABR complete
	if((USART1->ISR & USART_ISR_ABRE)  != (uint16_t)RESET){			//if error then exit
		return;
	}

  NVIC_SetPriority(USART1_IRQn, 0);	//NVIC enable for uart
  NVIC_EnableIRQ(USART1_IRQn);			//
}

void USART1_IRQHandler(void){

  if((USART1->ISR & USART_ISR_TC) == USART_ISR_TC)
  {
		if(send == sizeof(value_tab))
    {
      send=0;
      USART1->ICR |= USART_ICR_TCCF;	//clear index and transfer complete flag
    }
    else
    {
      USART1->TDR = value_tab[send++];	// clear transfer complete flag and fill TDR with a new char to send
    }
  }
  else if((USART1->ISR & USART_ISR_RXNE) == USART_ISR_RXNE)
  {
    //chartoreceive = (uint8_t)(USART1->RDR);/* Receive data, clear flag */
		USART1->RDR;	//clear flag by reading RDR
		ADC_Start();
  }
  else
  {
    NVIC_DisableIRQ(USART1_IRQn);	//disable UART in NVIC if error
  }
}

__INLINE void DEC_Place(){											 //format VALUES into char ASCII table

	value_tab[0]  =  0x0D;					//send return
	value_tab[1]  =  temp/10;
	value_tab[2]  = (temp-value_tab[1]*10);
	value_tab[3]  =	 0x0A;
	value_tab[4]  =	 tempAvr/10;
	value_tab[5]  = (tempAvr-value_tab[4]*10);
	value_tab[6]  =  0x0A;
	value_tab[7]  =  tempMax/10;
	value_tab[8]  = (tempMax-value_tab[7]*10);
	value_tab[9]  =  0x0A;
	value_tab[10] =	 tempMin/10;
	value_tab[11] =	(tempMin-value_tab[10]*10);
	value_tab[12] =  0x0A;					//send new-line

	for(int i = 1; i<12; ++i){
			value_tab[i] += 0x30;
	}
}

__INLINE void AVG_VAL(void){
	uint32_t static count = 1;
	int32_t static total = 0;
	total += temp;
	tempAvr = total / (count++);
}

__INLINE void MaxMin_VAL(void){
	if (temp > tempMax) tempMax = temp;
	if (temp < tempMin) tempMin = temp;
}

