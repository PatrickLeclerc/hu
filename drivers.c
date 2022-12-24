#include "drivers.h"

void startup(uint32_t hclk,TIM_t* tim,GPIO_t* gpio,DMA_t* dma){
	/*General Initialisation*/
	initClock(hclk);
	initGPIO(gpio);
	initDMA(dma);
	initTIM(tim);
}
void initClock(uint32_t hclk){
	/*APB1 45MHz Max*/
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;
	/*APB2 90MHz Max*/
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;
	/*HSE*/
	RCC->CR = (RCC->CR & ~RCC_CR_HSEBYP) | RCC_CR_HSEON;
	while(!(RCC->CR & RCC_CR_HSERDY)){}
	/*PLLSRC*/
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE;
	/*PLLM*/
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM_Msk;
	RCC->PLLCFGR |= (4U<<RCC_PLLCFGR_PLLM_Pos);
	/*PLLN*/
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN_Msk;
	RCC->PLLCFGR |= (hclk<<RCC_PLLCFGR_PLLN_Pos);
	/*PLLP*/
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLP_Msk;
	/*FLASHMEM*/
	FLASH->ACR |= FLASH_ACR_PRFTEN|FLASH_ACR_LATENCY_5WS;
	/*Activate PLL*/
	RCC->CR |= RCC_CR_PLLON;
	while(!(RCC->CR & RCC_CR_PLLRDY)){}
	/*Switch clock source to PLL*/
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	SystemCoreClockUpdate();
}
void initTIM(TIM_t* tim){
	/* RCC */
	uint32_t TIM_IRQn;
	switch(tim->n){
		//APB1
		case 2:{
			RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
			tim->tim_ptr = TIM2;
			TIM_IRQn = TIM2_IRQn;
			break;
		}
		case 3:{
			RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
			tim->tim_ptr = TIM3;
			TIM_IRQn = TIM3_IRQn;
			break;
		}
		case 4:{
			RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
			tim->tim_ptr = TIM4;
			TIM_IRQn = TIM4_IRQn;
			break;
		}
		case 5:{
			RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
			tim->tim_ptr = TIM5;
			TIM_IRQn = TIM5_IRQn;
			break;
		}
		case 6:{
			RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
			tim->tim_ptr = TIM6;
			TIM_IRQn = TIM6_DAC_IRQn;
			break;
		}
		case 7:{
			RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
			tim->tim_ptr = TIM7;
			TIM_IRQn = TIM7_IRQn;
			break;
		}
		//APB2
		case 1:{
			RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
			tim->tim_ptr = TIM1;
			TIM_IRQn = TIM1_UP_TIM10_IRQn;
			break;
		}
		case 8:{
			RCC->APB2ENR |= RCC_APB2ENR_TIM8EN;
			tim->tim_ptr = TIM8;
			TIM_IRQn = TIM8_UP_TIM13_IRQn;
			break;
		}
		default: return;
	}

	/* TIM */
	tim->tim_ptr->CR1 = TIM_CR1_ARPE | TIM_CR1_URS;
	tim->tim_ptr->PSC = tim->psc;
	tim->tim_ptr->ARR = tim->arr;
	tim->tim_ptr->CNT = 0U;
	//tim->tim_ptr->CR2 = TIM_CR2_MMS_1;
	
	/* NVIC and DMA */
	if(tim->tcie){
		tim->tim_ptr->DIER |= TIM_DIER_UIE;// | TIM_DIER_UDE;
		NVIC_EnableIRQ(TIM_IRQn);
	}
	
	/*Update registers */
	tim->tim_ptr->EGR |= TIM_EGR_UG;
	
	/* Enable */
	tim->tim_ptr->CR1 |= TIM_CR1_CEN;
	
	/*Update registers */
	tim->tim_ptr->EGR |= TIM_EGR_UG;
}
void initGPIO(GPIO_t* gpio){
	/* RCC */
	switch(gpio->port){
		case 'A':{
			RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
			gpio->port_ptr = GPIOA;
			break;
		}
		case 'B':{
			RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
			gpio->port_ptr = GPIOB;
			break;
		}
		case 'C':{
			RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
			gpio->port_ptr = GPIOC;
			break;
		}
		case 'D':{
			RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
			gpio->port_ptr = GPIOD;
			break;
		}
		default : return;
	}
	
	/* MODER and OSPEEDR */
	gpio->port_ptr->MODER = 0;
	gpio->port_ptr->OSPEEDR = 0;
	for(uint32_t i = 0; i < 16; i++)
		if((gpio->pins>>i)&1U){
			gpio->port_ptr->MODER 		|= (gpio->mode	<<(i*GPIO_MODER_MODE1_Pos));		 	 // in       out         af         an
			gpio->port_ptr->OSPEEDR 	|= (gpio->speed	<<(i*GPIO_OSPEEDR_OSPEED1_Pos));	// 4~8MHz   10~50MHz    10~50MHz   10~50MHz {see datasheet}
			//gpio->port_ptr->AFR[i>=8]	|= (gpio->af	<<((i%8)*GPIO_AFRL_AFRL1));
		}
	gpio->port_ptr->ODR = 0;
}

void initDMA(DMA_t* dma){
	uint32_t DMA_IRQn;
	/*RCC*/
	if(dma->n==1){
		RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
		switch(dma->stream){
			case 0:{
				dma->stream_ptr = DMA1_Stream0;
				DMA_IRQn = DMA1_Stream0_IRQn;
				break;
			}
			case 1:{
				dma->stream_ptr = DMA1_Stream1;
				DMA_IRQn = DMA1_Stream1_IRQn;
				break;
			}
			case 2:{
				dma->stream_ptr = DMA1_Stream2;
				DMA_IRQn = DMA1_Stream2_IRQn;
				break;
			}
			case 3:{
				dma->stream_ptr = DMA1_Stream3;
				DMA_IRQn = DMA1_Stream3_IRQn;
				break;
			}
			case 4:{
				dma->stream_ptr = DMA1_Stream4;
				DMA_IRQn = DMA1_Stream4_IRQn;
				break;
			}
			case 5:{
				dma->stream_ptr = DMA1_Stream5;
				DMA_IRQn = DMA1_Stream5_IRQn;
				break;
			}
			case 6:{
				dma->stream_ptr = DMA1_Stream6;
				DMA_IRQn = DMA1_Stream6_IRQn;
				break;
			}
			case 7:{
				dma->stream_ptr = DMA1_Stream7;
				DMA_IRQn = DMA1_Stream7_IRQn;
				break;
			}
			default:{
				return;
			}
			
		}
		
	}
		
	else if(dma->n==2){
		RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;	
		switch(dma->stream){
			case 0:{
				dma->stream_ptr = DMA2_Stream0;
				DMA_IRQn = DMA2_Stream0_IRQn;
				break;
			}
			case 1:{
				dma->stream_ptr = DMA2_Stream1;
				DMA_IRQn = DMA2_Stream1_IRQn;
				break;
			}
			case 2:{
				dma->stream_ptr = DMA2_Stream2;
				DMA_IRQn = DMA2_Stream2_IRQn;
				break;
			}
			case 3:{
				dma->stream_ptr = DMA2_Stream3;
				DMA_IRQn = DMA2_Stream3_IRQn;
				break;
			}
			case 4:{
				dma->stream_ptr = DMA2_Stream4;
				DMA_IRQn = DMA2_Stream4_IRQn;
				break;
			}
			case 5:{
				dma->stream_ptr = DMA2_Stream5;
				DMA_IRQn = DMA2_Stream5_IRQn;
				break;
			}
			case 6:{
				dma->stream_ptr = DMA2_Stream6;
				DMA_IRQn = DMA2_Stream6_IRQn;
				break;
			}
			case 7:{
				dma->stream_ptr = DMA2_Stream7;
				DMA_IRQn = DMA2_Stream7_IRQn;
				break;
			}
			default:{
				return;
			}
			
		}
	}
	else return;

	/* Stream and Channel */
	dma->stream_ptr->CR = 0U;
	while(dma->stream_ptr->CR != 0U){}
	dma->stream_ptr->CR |= (dma->ch<<DMA_SxCR_CHSEL_Pos);
	
	/* M0AR, M1AR, PAR NDTR*/
	dma->stream_ptr->M0AR = dma->m0ar;
	dma->stream_ptr->M1AR = dma->m1ar;
	dma->stream_ptr->PAR  = dma->par;
	dma->stream_ptr->NDTR = dma->ndtr;
		
	/* Size, Priority and Increment */
	dma->stream_ptr->CR |= DMA_SxCR_MSIZE_1 | DMA_SxCR_PSIZE_1;
	dma->stream_ptr->CR |= DMA_SxCR_PL_Msk;
	dma->stream_ptr->CR |= DMA_SxCR_PINC;
	dma->stream_ptr->FCR = 0U;
		
	/* Direction */
	dma->stream_ptr->CR |= DMA_SxCR_DIR_1;//Mem2Mem
	
	/* CIRC */
	//dma->stream_ptr->CR |= DMA_SxCR_CIRC;//|DMA_SxCR_DBM;

	/* IRQ */
	dma->stream_ptr->CR |= DMA_SxCR_TCIE;
	NVIC_EnableIRQ(DMA_IRQn);
	
	/* Enable */
	//dma->stream_ptr->CR |= DMA_SxCR_EN;
}
void reloadDMA(DMA_t* dma){	
	dma->stream_ptr->CR &= ~DMA_SxCR_EN;
	/* M0AR, M1AR, PAR NDTR*/
	dma->stream_ptr->M0AR = dma->m0ar;
	//dma_stream->M1AR = dma->m1ar;
	dma->stream_ptr->PAR  = dma->par;
	dma->stream_ptr->NDTR = dma->ndtr;
}

void enableDMA(DMA_t* dma){	
	dma->stream_ptr->CR |= DMA_SxCR_EN;
}
