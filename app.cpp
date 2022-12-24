extern "C"{
#include "drivers.h"
#include "it.h"
#include "stdio.h"
}
#define BUFF_SIZE 1024
/* IRQ flags */
volatile int tim2_flag = 0;
int main(){
	/* RCC */
	initClock(180U);
	
	/* Tables */ 
	DMA_t dma[8];
	static uint32_t tab_src[8][BUFF_SIZE];
	static uint32_t tab_dst[8][BUFF_SIZE];
	
	for(uint32_t i = 0; i < 8;i++)
		for(uint32_t j = 0; j < BUFF_SIZE;j++)
			tab_src[i][j] = (i+1)*j;
	
	/* Peripherals */
	for(uint32_t i = 0; i < 8;i++){//DMA
		dma[i].n		= 2;
		dma[i].stream	= i;
		dma[i].ch		= 0;
		dma[i].par		= uint32_t(tab_src[i]);
		dma[i].m0ar		= uint32_t(tab_dst[i]);
		dma[i].m1ar		= 0;
		dma[i].ndtr		= BUFF_SIZE;
		dma[i].pl		= DMA_PL_HIGH;
		dma[i].msize	= DMA_SIZE_W;
		dma[i].psize	= DMA_SIZE_W;
		dma[i].minc		= 1;//1;
		dma[i].pinc		= 1;//1;
		dma[i].dbm		= 0;
		dma[i].circ		= 0;
		dma[i].dir		= DMA_DIR_M2M;
		dma[i].tcie		= 0;
		dma[i].htie		= 0;
	}
	
	TIM_t tim2;{//TIM
		TIM_t* tim 		= &tim2;
		tim->n			= 0x2U;
		tim->psc		= 0x1U;
		uint32_t freq 	= 1000;
		tim->arr		= (SystemCoreClock/4)/freq-1;
		tim->uie		= 0x1U;}
	
	for(uint32_t i = 0; i < 8;i++)
		initDMA(&dma[i]);
	initTIM(&tim2);	
	for(uint32_t i = 0; i < 8;i++)
		enableDMA(&dma[i]);
	while(1){
		if(tim2_flag){
			tim2_flag = 0;
		}
	}
}


