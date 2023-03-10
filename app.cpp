extern "C"{
#include "drivers.h"
#include "it.h"
#include "stdio.h"
}
#define BUFF_SIZE 1024
/* IRQ flags */
volatile int tim2_flag = 0;
volatile int state = 0;
int main(){
	/* RCC */
	initClock(180U);
	printf("RCC\r\n");
	
	/* Tables */ 
	static uint32_t tab_src[8][BUFF_SIZE];
	static uint32_t tab_dst[8][BUFF_SIZE];
	for(uint32_t i = 0; i < 8;i++)
		for(uint32_t j = 0; j < BUFF_SIZE;j++)
			tab_src[i][j] = (i+1)*j;
	printf("Tables\r\n");
	
	/* Peripherals config */
	DMA_t dma[8];
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
	TIM_t tim;{//TIM
		uint32_t freq 	= 1000;
		tim.n	= 0x1U;
		tim.psc	= 0x1U;
		tim.arr	= (SystemCoreClock/2)/freq-1;
		tim.urs	= 1;
		tim.uie	= 0;
		tim.mms	= TIM_MMS_UP;
		tim.ccds= TIM_CCDS_UP;
		tim.ts	= TIM_TS_ITR0;
		tim.sms	= TIM_SMS_DIS;
	}
	printf("Peripherals Config\r\n");
	
	/* Peripherals init */
	for(uint32_t i = 0; i < 8;i++)
		initDMA(&dma[i]);
	initTIM(&tim);	
	printf("Peripherals Init\r\n");
	
	/* FSM */
	while(1){
		if(tim2_flag){
			tim2_flag = 0;
			if(state==0){
				for(uint32_t i = 0; i < 8;i++)
					enableDMA(&dma[i]);
				state++;
			}
			if(state==1){
				for(uint32_t i = 0; i < 8;i++)
					for(uint32_t j = 0; j < BUFF_SIZE;j++)
						tab_src[i][j] = i;
				state++;
			}
			if(state==2){
				for(uint32_t i = 0; i < 8;i++)
					enableDMA(&dma[i]);
				state++;
			}
			//printf("System core clock: %i\r\n",SystemCoreClock);
		}
	}
}


