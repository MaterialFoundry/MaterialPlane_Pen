/*
 * charging.h
 *
 * Created: 11/01/2023 15:37:55
 *  Author: Cris
 */ 


#ifndef BATTERY_H_
#define BATTERY_H_

uint8_t chargingStatus = 0;

/*
 * Configure peripherals for charging code
 */
void configureBattery() {
	//Configure VREF
	VREF.CTRLA = VREF_ADC0REFSEL_2V5_gc;										//set vref for ADC0 to 2.5V
	VREF.CTRLC = VREF_DAC1REFSEL_0V55_gc;										//set vref for AC1 to 0.55V
	VREF.CTRLD = VREF_DAC2REFSEL_4V34_gc;										//Set vref for AC2 to 4.34V

	//Configure ADC0 for battery voltage measurements
	ADC0.CTRLA = ADC_RESSEL_bm | ADC_FREERUN_bm | ADC_ENABLE_bm;				//10bit resolution, enable freerunning, enable ADC0
	ADC0.CTRLB = ADC_SAMPNUM_ACC64_gc;											//64 times accumulation
	ADC0.CTRLC = ADC_SAMPCAP_bm | ADC_REFSEL_VDDREF_gc | ADC_PRESC_DIV256_gc;	//sampcap, vdd ref, 256 prescaler
	ADC0.CTRLD = ADC_INITDLY_DLY256_gc;											//256 cycles initial delay
	ADC0.SAMPCTRL = 31;															//increases sample length by specified clk cycles
	ADC0.MUXPOS = ADC_MUXPOS_INTREF_gc;											//set int ref (vref) as input
	ADC0.INTCTRL = ADC_RESRDY_bm;												//enable result ready interrupt
	ADC0.COMMAND = ADC_STCONV_bm;												//start conversion

	//Configure AC1 and AC2 for charging state detection
	AC1.MUXCTRLA = 0x01 << 7 | 0x01 << 3 | 0x02;								//invert output, PA6 on positive, VREF on negative
	AC1.INTCTRL = 1;															//enable interrupt
	AC1.CTRLA = 0b10001111;														//runstdby, lpmode, hys50mv, enable
	AC2.MUXCTRLA = 0x00 << 3 | 0x02;											//PA6 on positive, VREF on negative
	AC2.INTCTRL = 1;															//enable interrupt
	AC2.CTRLA = 0b10001111;														//enable
}

uint16_t getBatteryVoltage() {
	batteryVoltage = (uint16_t)(-0.3024*ADC0.RES + 7161.3);
	if (!lowBattery && batteryVoltage < MIN_BAT_WARNING) lowBattery = TRUE;
	else if (lowBattery && batteryVoltage >= MIN_BAT_WARNING + MIN_BAT_HYSTERESIS) lowBattery = FALSE;
	ADC0.COMMAND = ADC_STCONV_bm;
	batReadReady = false;
	return batteryVoltage;
}

uint8_t getBatteryState() {
	uint16_t voltage = getBatteryVoltage();
	if (voltage >= 3800) return 6;	//85-100%
	if (voltage >= 3650) return 5;	//70-85%
	if (voltage >= 3500) return 4;	//55-70%
	if (voltage >= 3450) return 3;	//40-55%
	if (voltage >= 3300) return 2;	//25-40%
	if (voltage >= 3000) return 1;	//10-25%
	return 0;						//0-10%
}

/**
 * 00 => 0.55V < pin < 2.5V       shutdown
 * 01 => pin < 0.55V              charging
 * 10 => pin > 2.5V               charge complete
 * 11 => invalid
 */
uint8_t getChargingState() {
  chargingStatus = (AC2.STATUS >> 4) << 1 | AC1.STATUS >> 4;
  ACtriggered = FALSE;
  return chargingStatus;
}

ISR(AC1_AC_vect) {
	AC1.STATUS = AC_CMP_bm;
	ACtriggered = true;
}

ISR(AC2_AC_vect) {
	AC2.STATUS = AC_CMP_bm;
	ACtriggered = true;
}

ISR(ADC0_RESRDY_vect) {
	ADC0.INTFLAGS |= ADC_RESRDY_bm;
	batReadReady = true;
}

#endif /* BATTERY_H_ */