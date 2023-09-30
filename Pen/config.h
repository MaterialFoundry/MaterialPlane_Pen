/*
 * config.h
 *
 * Created: 11/01/2023 15:49:58
 *  Author: Cris
 */ 


#ifndef CONFIG_H_
#define CONFIG_H_

#define DEBUG true


/*
DEVICE_ID: 
0 = reserved
1 = base v1.0.0
2 = pen v1.0.0
*/
#define DEVICE_ID	2

#define IR_PERIOD	130

//Low battery is detected if battery voltage (in mV) is below MIN_BAT_WARNING. State is cleared if voltage is above MIN_BAT_WARNING + MIN_BAT_HYSTERESIS
#define MIN_BAT_WARNING	3200
#define MIN_BAT_HYSTERESIS	100

#define CHARGE_LED_STEP 10
#define CHARGE_LED_PERIOD 1000

#define CHARGE_LED_MIN  1
#define CHARGE_LED_MAX  255
#define CHARGE_LED_FACTOR	0.1
#define RED_LED_MIN		10
#define RED_LED_MAX		150

#define FALSE			0
#define TRUE			1

#define OFF				0
#define ON				1
#define FADE			2
#define TRANSMIT		3

#define CH_SHUTDOWN		0
#define CH_CHARGING		1
#define CH_CHARGED		2


#endif /* CONFIG_H_ */