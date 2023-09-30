/*
 * irLed.h
 *
 * Created: 16/05/2023 05:06:47
 *  Author: Cris
 */ 

#ifndef IRLED_H_
#define IRLED_H_

uint8_t irPeriod;

uint8_t rearIrLed = false;

void setRearIrLed(uint8_t en) {
	rearIrLed = en;
}

void setIrPeriod(uint8_t period) {
	TCB0_CCMPL = period;
	irPeriod = period;
}

//0 to 100%
void setIrDuty(uint8_t duty) {
	uint16_t offS = (100-duty)*irPeriod/100;
	TCB0_CCMPH = (uint8_t)(offS);
}

void setIrLed(uint8_t state) {
	if (state == LED_OFF) {
		PORTB.OUTCLR = PIN4_bm;
		PORTA.OUTCLR = PIN7_bm;
		CCL.CTRLA = 0;
	}
	else if (state == LED_HIGH) {
		PORTB.OUTSET = PIN4_bm;
		if (rearIrLed) PORTA.OUTSET = PIN7_bm;
		else PORTA.OUTCLR = PIN7_bm;
		CCL.CTRLA = 0;
	}
	else if (state == LED_PULSE) {
		//PORTA.OUTSET = PIN4_bm;
		CCL.CTRLA = 1;
	}
}

void sendIr(uint8_t cmd, uint32_t serialNumber, uint8_t vBatState) {
	//printf("sendIr: %d\n",cmd);
	uint32_t data;
	if (cmd == CMD_STOP) {
		data = (uint32_t)serialNumber<<8 | 0b11100001;
	}
	else {
		data = (uint32_t)serialNumber<<8 | (vBatState&0x07)<<5 | cmd&0x1F;
	}
	//data = 43690 << 8 | cmd;
	
	setIrLed(LED_PULSE);
	_delay_us(IR_LONG);
	setIrLed(LED_HIGH);
	_delay_us(IR_SHORT);
	
	for (int i=23; i>=0; i--) {
		setIrLed(LED_PULSE);
		_delay_us(IR_SHORT);
		setIrLed(LED_HIGH);
		if ((data>>i)&1) _delay_us(IR_LONG);
		else _delay_us(IR_SHORT);
	}
	setIrLed(LED_PULSE);
	_delay_us(IR_SHORT);
	setIrLed(LED_HIGH);
	_delay_ms(75);
}

void configureIrLed() {
	setIrPeriod(130);
	setIrDuty(50);
	setIrLed(OFF);
}

#endif /* IRLED_H_ */