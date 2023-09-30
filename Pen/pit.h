/*
 * pit.h
 *
 * Created: 12/01/2023 01:07:33
 *  Author: Cris
 */ 


#ifndef PIT_H_
#define PIT_H_

void initializePit() {
	//Configure PIT
	RTC.CLKSEL = 0x01;                      //set to internal 1.024khz clock
	RTC.PITINTCTRL = RTC_PI_bm;
	RTC.PITCTRLA = RTC_PERIOD_CYC2048_gc | RTC_PITEN_bm;    //set PIT to a period of 1Hz and enable it
}

/**
 * PIT ISR
 */
ISR(RTC_PIT_vect) {
	//printf("PIT\n");
	RTC.PITINTFLAGS=RTC_PI_bm;  //clear PIT interrupt flags
}

#endif /* PIT_H_ */