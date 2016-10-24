#include "uart_init.h"

extern volatile int32_t temp;

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
		if(send == sizeof(baudrate_tab))
    {
      send=0;
      USART1->ICR |= USART_ICR_TCCF;	//clear index and transfer complete flag
    }
    else
    {
      USART1->TDR = baudrate_tab[send++];	// clear transfer complete flag and fill TDR with a new char to send
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

__INLINE void DEC_Place(void){											 //format BR into char ASCII table
	
	uint32_t baudrate = temp;
	baudrate_tab[0] =  0x0D;					//send return
	baudrate_tab[1] =  baudrate/100000;
	baudrate_tab[2] = (baudrate-baudrate_tab[1]*100000)/10000;
	baudrate_tab[3] = (baudrate-baudrate_tab[1]*100000-baudrate_tab[2]*10000)/1000;
	baudrate_tab[4] = (baudrate-baudrate_tab[1]*100000-baudrate_tab[2]*10000-baudrate_tab[3]*1000)/100;
	baudrate_tab[5] = (baudrate-baudrate_tab[1]*100000-baudrate_tab[2]*10000-baudrate_tab[3]*1000-baudrate_tab[4]*100)/10;
	baudrate_tab[6] = (baudrate-baudrate_tab[1]*100000-baudrate_tab[2]*10000-baudrate_tab[3]*1000-baudrate_tab[4]*100-baudrate_tab[5]*10);
	baudrate_tab[7] =  0x0A;					//send new-line
	
	baudrate_tab[1] += 0x30;	//turn into ascii
	baudrate_tab[2] += 0x30;
	baudrate_tab[3] += 0x30;
	baudrate_tab[4] += 0x30;
	baudrate_tab[5] += 0x30;
	baudrate_tab[6] += 0x30;
}
