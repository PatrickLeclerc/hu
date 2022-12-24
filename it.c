#include "drivers.h"
#include "it.h"

static volatile int tim_flag;

void TIM2_IRQHandler(){
	if(TIM2->SR & TIM_SR_UIF){
		TIM2->SR &= ~TIM_SR_UIF;
		tim_flag = 1;
	}
}
