/*
 * sleep.h
 *
 * Created: 12/01/2023 01:09:40
 *  Author: Cris
 */ 


#ifndef SLEEP_H_
#define SLEEP_H_

void initializeSleep() {
	//Set sleep mode
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);

	//Enable sleep
	sleep_enable();
}

void sleep() {
	if (sleeping) {
		allLedsOff();
		setIrLed(LED_OFF);
		PORTB.PIN0CTRL = 0x4;
		//PORTB.PIN1CTRL = 0x4;
		delay(10);	
		sleep_cpu();
	}
	if (!sleeping) {
		initializeLeds();
		getChargingState();
		setLeds();
	}
}

#endif /* SLEEP_H_ */