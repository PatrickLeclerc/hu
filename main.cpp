extern "C"{
#include "drivers.h"
#include "it.h"
}



/* IRQ flags */
static volatile int tim_flag = 0;

int main(){
	/* Peripherals */
	DMA_t dma = {};
	GPIO_t gpio = {};
	TIM_t tim = {};
	{//DMA
		dma.n 		= 0;
		dma.stream  = 0;
		dma.ch 		= 0;
		dma.m0ar 	= 0;
		dma.m1ar 	= 0;
		dma.par		= 0;
		dma.ndtr	= 0;
	}
	{//GPIO
		gpio.port = 'C';
		gpio.pins = 3;
		gpio.speed= 0;
		gpio.af		= 0;
	}
	{//TIM
		tim.n		= 2;
		tim.psc	=	1;
		tim.arr	=	(SystemCoreClock/4)/100000-1;
		tim.tcie= 1;
	}
	
	startup(180,&tim,&gpio,&dma);
	while(1){
		if(tim_flag)
			tim_flag = 0;
	}
}


