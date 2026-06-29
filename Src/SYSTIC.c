#include "SYSTIC.h"

void one_sec_delay(void)
{
	//Load value in the reload register
	SYST_RVR = ONE_SEC_VALUE - 1;

	//Clear the current value register
	SYST_CVR = 0;

	//Set clock source,enable interrupt and systick timer
	SYST_CSR |= CLK_SRC | COUNTER_EN | TICKINT_EN;




}


void uS_delay(int delay)
{
	if(delay > 1000000){
		while(1);
	}
	//Load value in the reload register
	SYST_RVR = (delay*ONE_US_VALUE) - 1;

	//Clear the current value register
	SYST_CVR = 0;

	//Set clock source,enable interrupt and systick timer
	SYST_CSR |= CLK_SRC | COUNTER_EN;

	while(!(SYST_CSR &(1U << 16)));

	SYST_CSR &= ~(1U << 0);//Disable timer
}
