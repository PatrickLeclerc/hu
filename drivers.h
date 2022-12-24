#ifndef DRIVERS_H
#define DRIVERS_H
#include "stm32f446xx.h"
/* Enums */
//DMA
typedef enum DMA_SIZE{
	DMA_SIZE_B = 0,
	DMA_SIZE_HW,
	DMA_SIZE_W
}DMA_SIZE_t;
typedef enum DMA_DIR{
	DMA_DIR_P2M = 0,	//PAR ->MXAR
	DMA_DIR_M2P,		//MXAR->PAR
	DMA_DIR_M2M			//PAR ->M0AR
}DMA_DIR_t;
typedef enum DMA_PL{
	DMA_PL_LOW = 0,
	DMA_PL_MEDIUM,
	DMA_PL_HIGH,
	DMA_PL_VERYHIGH
}DMA_PL_t;
//GPIO
typedef enum GPIO_SPEED{
	GPIO_SPEED_4_8MHz = 0,
	GPIO_SPEED_25_50MHz,
	GPIO_SPEED_50_100MHz,
	GPIO_SPEED_100_180MHz
}GPIO_SPEED_t;
typedef enum GPIO_MODE{
	GPIO_MODE_IN = 0,
	GPIO_MODE_OUT,
	GPIO_MODE_AF,
	GPIO_MODE_AN
}GPIO_MODE_t;

/* Structs */
typedef struct __attribute__((__packed__)) DMA{
	DMA_Stream_TypeDef* stream_ptr;
	uint32_t 			n;
	uint32_t			stream;
	uint32_t 			ch;
	uint32_t 			m0ar;
	uint32_t 			m1ar;
	uint32_t 			par;
	uint32_t 			ndtr;
	DMA_PL_t 			pl;
	DMA_SIZE_t 			msize;
	DMA_SIZE_t 			psize;
	uint32_t 			minc;
	uint32_t 			pinc;
	uint32_t 			dbm;
	uint32_t 			circ;
	DMA_DIR_t 			dir;
	uint32_t 			tcie;
	uint32_t 			htie;
}DMA_t;

typedef struct __attribute__((__packed__)) GPIO{
	GPIO_TypeDef* 		port_ptr;
	char 				port;
	uint32_t			pins;
	GPIO_SPEED_t		speed;
	GPIO_MODE_t			mode;
	uint32_t			af;
}GPIO_t;

typedef struct __attribute__((__packed__)) TIM{
	TIM_TypeDef *		tim_ptr;
	uint32_t 			n;
	uint32_t			psc;
	uint32_t			arr;
	uint32_t			uie;
}TIM_t;
typedef struct __attribute__((__packed__)) USART{
	USART_TypeDef *		usart_ptr;
	uint32_t 			n;
	uint32_t			br;
	uint32_t			rxie;
	uint32_t			txie;
}USART_t;
/* Functions */
//MAIN
void startup(uint32_t hclk,TIM_t* tim,GPIO_t* gpio,DMA_t* dma);//hclk in MHz, freq_tim2 in KHz
void initClock(uint32_t hclk);
//TIM
void initTIM(TIM_t* tim);
//GPIO
void initGPIO(GPIO_t* gpio);
//DMA
void initDMA(DMA_t* dma);
void reloadDMA(DMA_t* dma);
void enableDMA(DMA_t* dma);
void disableDMA(DMA_t* dma);
//USART
void initUSART(USART_t* usart);
int stdin_getchar (void);
int stdout_putchar (int ch);
#endif
