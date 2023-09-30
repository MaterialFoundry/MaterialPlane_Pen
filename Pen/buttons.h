/*
 * buttons.h
 *
 * Created: 12/01/2023 00:57:49
 *  Author: Cris
 */ 


#ifndef BUTTONS_H_
#define BUTTONS_H_

void initializeButtons() {
	//set to input
	PORTB.DIRCLR = PIN3_bm | PIN5_bm;
	PORTC.DIRCLR = PIN0_bm | PIN1_bm;

	//Invert value, enable pullup, set interrupt to BOTH
	PORTB.PIN3CTRL = PORT_INVEN_bm | PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;
	PORTB.PIN5CTRL = PORT_INVEN_bm | PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;
	PORTC.PIN0CTRL = PORT_INVEN_bm | PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;
	PORTC.PIN1CTRL = 1 << 7 | 1<<3 | 1;
	
	//Clear interrupts
	PORTB.INTFLAGS=PORTB.INTFLAGS;
	PORTC.INTFLAGS=PORTC.INTFLAGS;
}

uint8_t getButtonState() {
	return (PORTB.IN>>3 & 1) << 3 | (PORTB.IN>>5 & 1) << 2 | (PORTC.IN & 3);
}

ISR(PORTB_PORT_vect) {
	PORTB.INTFLAGS=PORTB.INTFLAGS;  //clear pin interrupt flags
	sleepTimer = millis();
	sleeping = false;
}

ISR(PORTC_PORT_vect) {
	PORTC.INTFLAGS=PORTC.INTFLAGS;  //clear pin interrupt flags
	sleepTimer = millis();
	sleeping = false;
}

#endif /* BUTTONS_H_ */