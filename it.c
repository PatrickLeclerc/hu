#include "drivers.h"
#include "it.h"
#include "stm32f446xx.h"
extern volatile int tim2_flag;

void TIM2_IRQHandler(){
	if(TIM2->SR & TIM_SR_UIF){
		TIM2->SR &= ~TIM_SR_UIF;
		tim2_flag = 1;
	}
}

void DMA2_Stream0_IRQHandler(void){
	if(DMA2->LISR & DMA_LISR_TCIF0){
		DMA2->LIFCR &= ~DMA_LIFCR_CTCIF0;
		DMA2_Stream0->CR &= ~DMA_SxCR_EN;
		//dma2S0_flag = 1;
	}
}
