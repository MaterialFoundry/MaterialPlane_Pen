/*
 * millis_timer.h
 *
 * Created: 11/01/2023 15:58:10
 *  Author: Cris
 */ 


#ifndef MILLIS_TIMER_H_
#define MILLIS_TIMER_H_

volatile uint64_t millisCount = 0;

void delay(uint32_t t) {
	uint64_t tNow = millisCount;
	while (millisCount - tNow < t) {}
}

void initializeMillisTimer() {
	TCB1.CTRLB = 0;											//No async, initial state low, no output, periodic interrupt mode
	TCB1.INTCTRL = TCB_CAPT_bm;								//enable capture interrupt
	TCB1.CCMP = 2500;										//Set period
	TCB1.CTRLA = TCB_CLKSEL_CLKDIV2_gc | TCB_ENABLE_bm;		//enable timer with no prescaler
}

uint64_t millis() {
	return millisCount;
}

/*
 * Interrupt service routine for TCB1
 */
ISR(TCB1_INT_vect)
//ISR(TCA0_LUNF_vect)
{
	millisCount++;
	//TCA0_SINGLE_INTFLAGS = TCA0_SINGLE_INTFLAGS;
	TCB1.INTFLAGS = TCB_CAPT_bm;
}

#endif /* MILLIS_TIMER_H_ */