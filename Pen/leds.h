/*
 * leds.h
 *
 * Created: 12/01/2023 01:23:53
 *  Author: Cris
 */ 


#ifndef LEDS_H_
#define LEDS_H_

uint16_t chargeLedCount = 0;
volatile uint8_t redLedFade = false;
volatile uint8_t redChargeLedFade = false;

void initializeLeds() {
	//Configure green charge led on PC2 and red charge led on PC3
	PORTB.DIRSET = PIN1_bm | PIN2_bm;										//Set as output
	PORTB.PIN1CTRL = PORT_INVEN_bm;											//Invert output
	PORTB.PIN2CTRL = PORT_INVEN_bm;											//Invert output
	
	//Configure TCA0 to generate PWM for red charging led
	TCA0.SINGLE.CTRLD = 1;													//enable split mode
	TCA0.SPLIT.CTRLB = TCA_SPLIT_LCMP0EN_bm | TCA_SPLIT_LCMP2EN_bm;			//Enable LCMP1 & HCMP0 output
	TCA0.SPLIT.LPER   = RED_LED_MAX;										//Count down from 255 for red led
	TCA0.SPLIT.LCMP0  = 0;													//0% duty cycle to switch led off
	TCA0.SPLIT.LPER   = 255;												//Count down from 255 for red charging led
	TCA0.SPLIT.LCMP2  = 0;													//0% duty cycle to switch led off
	TCA0.SPLIT.INTCTRL= 1;													//Enable LCMP0 interrupt for millis timer
	TCA0.SPLIT.CTRLA  = TCA_SPLIT_CLKSEL_DIV64_gc | TCA_SPLIT_ENABLE_bm;	//Enable the timer with prescaler of 16
	
	setChargeLedRed(OFF);
	setChargeLedGreen(OFF);
	setRedLed(OFF);
	//while(1) {};
}

void setRedLed(uint8_t type) {
	if (type == OFF) {
		TCA0.SPLIT.LCMP0 = 0;
		redLedFade = false;
	}
	else if (type == ON) {
		TCA0.SPLIT.LCMP0 = RED_LED_MAX;
		redLedFade = false;
	}
	else redLedFade = true;
}


void setChargeLedRed(uint8_t type) {
	if (type == FADE) {
		redChargeLedFade = true;
		return;
	}
	if (type == OFF) {
		TCA0.SPLIT.LCMP2 = 0;
		redChargeLedFade = false;
	}
	else if (type == ON) {
		TCA0.SPLIT.LCMP2 = CHARGE_LED_MAX;
		redChargeLedFade = false;
	}
}

void setChargeLedGreen(uint8_t en) {
	if (en) PORTB.OUTSET = PIN1_bm;
	else PORTB.OUTCLR = PIN1_bm;
}

void allLedsOff() {
	setRedLed(OFF);
	setChargeLedRed(OFF);
	setChargeLedGreen(OFF);
}

void setLeds() {
	//Shutdown => not connected to charger
	if (chargingStatus == CH_SHUTDOWN) {
		setChargeLedRed(OFF);
		setChargeLedGreen(ON);
		if (active && lowBattery) {
			setRedLed(FADE);
		}
		else if (active) {
			setRedLed(ON);
		}
	}
	//Charge complete
	else if (chargingStatus == CH_CHARGED) {
		setChargeLedRed(OFF);
		setChargeLedGreen(ON);
		setRedLed(OFF);
	}
	//Charging
	else {
		setChargeLedRed(FADE);
		setRedLed(OFF);
		setChargeLedGreen(OFF);
	}
}

void test() {
	chargeLedCount++;
	uint8_t duty = abs((int8_t)(chargeLedCount/4));
	if (redLedFade) TCA0.SPLIT.LCMP0 = RED_LED_MIN + duty;
	if (redChargeLedFade) TCA0.SPLIT.LCMP2 = duty;
}

ISR(TCA0_LUNF_vect)
{
	chargeLedCount++;
	uint8_t duty = abs((int8_t)(chargeLedCount/4));
	if (redLedFade) TCA0.SPLIT.LCMP0 = RED_LED_MIN + duty;
	if (redChargeLedFade) TCA0.SPLIT.LCMP2 = CHARGE_LED_MIN + duty;
	TCA0_SINGLE_INTFLAGS = TCA0_SINGLE_INTFLAGS;
}

#endif /* LEDS_H_ */