/*
 * definitions.h
 *
 * Created: 16/05/2023 05:10:21
 *  Author: Cris
 */ 


#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

/*
 * CMD
 * Sets the command to be sent
 */
#define CMD			2
#define CMD_A		3
#define CMD_B		4
#define CMD_C		5
#define CMD_D		6
#define CMD_AD		7
#define CMD_BC		8
#define CMD_STOP	15
//#define CMD_CAL		3
//#define CMD_PIT		5

//@5MHz
#define IR_LOW  2440
#define IR_MID	4875
#define IR_HIGH 7311
#define IR_SHORT	500
#define IR_LONG		1000

//@3.33MHz
//#define IR_LOW  1667
//#define IR_MID	IR_LOW*2
//#define IR_HIGH IR_LOW*3

#define LED_OFF 0
#define LED_HIGH 1
#define LED_PULSE 2



#endif /* DEFINITIONS_H_ */