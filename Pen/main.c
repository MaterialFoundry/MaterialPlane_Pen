//#define F_CPU 115000UL  // 5 MHz
#define ENABLE_WHILE_CHARGING	false

#include <atmel_start.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <avr/sleep.h>
#include "definitions.h"
#include "config.h"
#include "eeprom.h"
#include "irLed.h"

#define CMD_STOP	0b11100001

//@5MHz
#define IR_LOW  2440
#define IR_MID	4875
#define IR_HIGH 7311
#define IR_SHORT	500
#define IR_LONG		1000

#define LED_OFF 0
#define LED_HIGH 1
#define LED_PULSE 2

#define BATTERY_PERIOD 10000

uint8_t const FW_VERSION[3] = {2,0,1};

uint16_t serialNumber;
uint8_t vBatState = 0;
uint8_t active = 1;
uint32_t chargeLedTimer = 0;
volatile uint8_t batReadReady = false;
uint16_t batteryVoltage = 0;
uint8_t lowBattery = FALSE;
volatile bool sleeping = FALSE;

unsigned long batteryTimer = 0;
volatile bool ACtriggered = FALSE;
uint8_t irPeriod;
uint8_t timeOutPeriod = 10;
volatile unsigned long sleepTimer = 0;

#include "battery.h"
#include "millis_timer.h"
#include "buttons.h"
#include "pit.h"
#include "sleep.h"
#include "leds.h"

void pinsToLowPowerMode() {
	PORTA.PIN0CTRL = 0x4;   //UPDI
	PORTA.PIN1CTRL = 0x4;	//Tx
	PORTA.PIN2CTRL = 0x4;	//Rx
	PORTA.PIN3CTRL = 0x4;
	PORTA.PIN4CTRL = 0x4;
	PORTA.PIN5CTRL = 0x4;
	PORTA.PIN6CTRL = 0x4;	//Stat
	//PORTA.PIN7CTRL = 0x4;	//IR LED rear

	//PORTB.PIN0CTRL = 0x4;	//Led Red
	//PORTB.PIN1CTRL = 0x4;	//Green charging LED
	//PORTB.PIN2CTRL = 0x4;	//Red charging LED
	//PORTB.PIN3CTRL = 0x4;	//SW RF
	//PORTB.PIN4CTRL = 0x4;	//IR LED front
	//PORTB.PIN5CTRL = 0x4;	//SW RR

	//PORTC.PIN0CTRL = 0x4;	//SW LF
	//PORTC.PIN1CTRL = 0x4;	//SW LR
	PORTC.PIN2CTRL = 0x4;
	PORTC.PIN3CTRL = 0x4;
}

bool state = FALSE;
int main(void)
{
	atmel_start_init();
	initializeButtons();
	initializeSleep();
	initializeLeds();
	configureBattery();
	initializeMillisTimer();
	initializePit();
	getChargingState();
	setLeds();
	configureIrLed();
	eepromPutDevice(DEVICE_ID);
	eepromPutVersion(FW_VERSION[0], FW_VERSION[1], FW_VERSION[2]);
	
	serialNumber = SIGROW.SERNUM8<<8 | SIGROW.SERNUM9;	//Get the uC's serial number
	if (eepromGetId() == 0xFFFF) eepromPutId(serialNumber);
	else serialNumber = eepromGetId();
	
	if (eepromGetSleepTimer() == 0xFF) eepromPutSleepTimer(timeOutPeriod);
	else timeOutPeriod = eepromGetSleepTimer();
	
	sleepTimer = timeOutPeriod*1000;

	sei();
	
	//Configure pins for testing
	setChargeLedGreen(0);
	setChargeLedRed(OFF);
	if (!testPin_get_level()) {
		while(1) {
			if (!testPin_get_level()) {
				setRedLed(ON);
				setRearIrLed(1);
				setIrLed(LED_HIGH);
			}
			else {
				setRedLed(OFF);
				setRearIrLed(0);
				setIrLed(OFF);
			}
		}
	}
	
	//Charging LED test
	/*
	while(1) {
		setChargeLedRed(ON);
		setChargeLedGreen(OFF);
		_delay_ms(1000);
		setChargeLedRed(OFF);
		setChargeLedGreen(ON);
		_delay_ms(1000);
	}
	*/
	
	pinsToLowPowerMode();

	while (1) {

		//If analog comparator is triggered, check the charging state
		if (ACtriggered) getChargingState();
		
		//Periodically check led configuration
		if (millis() - chargeLedTimer >= CHARGE_LED_PERIOD) {
			chargeLedTimer = millis();
			getChargingState();
			setLeds();
		}
		
		if (millis() - batteryTimer >= BATTERY_PERIOD) {
			batteryTimer = millis();
			//Read battery voltage if ADC is ready
			if (batReadReady) vBatState = getBatteryState();
			
		}
		
		//Do if not connected to charger
		if (chargingStatus == CH_SHUTDOWN || ENABLE_WHILE_CHARGING) {
			uint8_t buttons = getButtonState();
			
			if (!buttons && sleeping) {
				sleep();
			}
			
			//If no button has been pressed for a while, sleep
			if (!buttons && millis() - sleepTimer >= timeOutPeriod*1000) {
				sleeping = true;
				sleep();
			}
			//printf("buttons %d\n",buttons);
			if (!sleeping) {
				uint8_t cmd;
				if (buttons == 0)	   cmd = CMD;
				else if (buttons == 8) cmd = CMD_A;
				else if (buttons == 1) cmd = CMD_D;
				else if (buttons == 4) cmd = CMD_B;
				else if (buttons == 2) cmd = CMD_C;
				else if (buttons == 9) cmd = CMD_AD;
				else if (buttons == 6) cmd = CMD_BC;
				else cmd = CMD_AD;
				
				setRearIrLed(buttons>>2&1);
				sendIr(cmd, serialNumber, vBatState);
			}
		}
		
		//Do if connected to charger
		else {
			setRedLed(OFF);
			setRearIrLed(0);
			setIrLed(OFF);
		}
		
		
		
	}
}
