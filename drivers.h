#ifndef DRIVERS_H
#define DRIVERS_H
#include "stm32f446xx.h"
/* Structs */
typedef struct DMA{
	DMA_Stream_TypeDef* stream_ptr;
	uint32_t 			n;
	uint32_t			stream;
	uint32_t 			ch;
	uint32_t 			m0ar;
	uint32_t 			m1ar;
	uint32_t 			par;
	uint32_t 			ndtr;
}DMA_t;

typedef struct __attribute__((__packed__)) GPIO{
	GPIO_TypeDef* 		port_ptr;
	char 				port;
	uint32_t			pins;
	uint32_t			speed;
	uint32_t			mode;
	uint8_t				af;
}GPIO_t;

typedef struct TIM{
	TIM_TypeDef *		tim_ptr;
	uint32_t 			n;
	uint32_t			psc;
	uint32_t			arr;
	uint32_t			tcie;
}TIM_t;

/* Functions */
void startup(uint32_t hclk,TIM_t* tim,GPIO_t* gpio,DMA_t* dma);//hclk in MHz, freq_tim2 in KHz
void initClock(uint32_t hclk);
void initTIM(TIM_t* tim);
void initGPIO(GPIO_t* gpio);
void initDMA(DMA_t* dma);
void reloadDMA(DMA_t* dma);
void enableDMA(DMA_t* dma);
#endif
