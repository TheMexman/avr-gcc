/*****************************************************************************
*
*
*
****************************************************************************/
//INCLUDES
#include <avr/interrupt.h>
#define F_CPU 1000000UL	      // Sets up the default speed for delay.h
#include <util/delay.h>
#include <avr/io.h>
#include "USI_TWI_Master.h"
#include "DS1307.h"			//Include My RTC library
#define numberOfButtons 3
#include "ButtonPress.h"

//DEFINES
#define PORT_ADDR			0x38	// For 8574A I/O port expander
#define MESSAGEBUF_SIZE		8

//GLOBALS
unsigned char index = 1;
const int hz = 1;
unsigned int number = 0;
unsigned const int MaxDigit = 9999;
unsigned char i2cData = 0x00;
RTCInfo RTC;
const uint8_t daysInMonth[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

//PROTOTYPES
void digitSplit(int inDigit, int ix);
void addDay();
void addMinute();
void remDay();
void remMinute();


int main( void )
{
	unsigned char messageBuf[MESSAGEBUF_SIZE];
	unsigned char TWI_targetSlaveAddress, noerror;
	USI_TWI_Master_Initialise();
	DDRB |= 0b00000010;	//Set Error/Check Pin AS Output - Set Button as Input
	PORTB &= ~_BV(PINB3) | ~_BV(PINB1)  | ~_BV(PINB4) | ~_BV(PINB5); //Set Pins LOW
	TWI_targetSlaveAddress   = PORT_ADDR;
	unsigned char mask = 0x00;
	unsigned const char DS1307 = 0x68;
	uint8_t seconds, minutes, hours, month, day, date, year,running = RTCCheckRunning(),minuteShow = 0;
	RTCInfo RTC;
	while(1){
		RTCGetTime(&RTC);
		if (ButtonPressed(2, PINB, 5, 100))
			minuteShow = !minuteShow;
		if( minuteShow)
		{
			number = RTC.hours * 100 + RTC.minutes;
			if (ButtonPressed(0, PINB, 3, 100))
				addMinute();
			else if (ButtonPressed(1, PINB, 4, 100))
				remMinute();
		}			
		else{
			number = RTC.month * 100 + RTC.date;
			if (ButtonPressed(0, PINB, 3, 100))
				addDay();
			else if (ButtonPressed(1, PINB, 4, 100))
				remDay();
		}			
		digitSplit(number,index);
		switch(index){
			case 1: mask = 0b10000000;
			break;
			case 2: mask = 0b01000000;
			break;
			case 3: mask = 0b0100000;
			break;
			case 4: mask = 0b00010000;
			break;
			default: break;
		}
		i2cData ^=  mask;
		if(index >= 4){
			index = 1;
			//number ++;
		}
		else
		index++;
		// Write to the I2C Port. Set LEDs according to switches
		messageBuf[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);
		messageBuf[1] = i2cData;  // The second byte is used for the data.
		//	Must be sure to set 4 LSB's HI so they work as inputs.
		noerror = USI_TWI_Start_Read_Write( messageBuf, 2 );
		if(!RTCAfternoon())
			PORTB |= _BV(PINB1);  //Set Pin High When Not Running
		else
			PORTB &= ~_BV(PINB1); //Set Low When Running
		_delay_ms(hz);
	}
	return 0;
}

void digitSplit(int inDigit, int ix){
	//Make sure number is not greater than 4Digit7seg Max
	i2cData = 0x00;
	if(inDigit > MaxDigit)
	number = inDigit = 0;
	int digit1 = inDigit / 1000;
	int digit2 = (inDigit - (digit1 * 1000)) / 100;
	int digit3 = (inDigit - (digit1 * 1000) - (digit2 * 100)) / 10;
	int digit4 = (inDigit - (digit1 * 1000) - (digit2 * 100) - (digit3 * 10));
	int var;
	
	if(ix == 1)
	var = digit1;
	else if (ix == 2)
	var = digit2;
	else if (ix == 3)
	var = digit3;
	else if (ix == 4)
	var = digit4;
	i2cData |= (var & 0b00001000) | (var & 0b00000100) | (var & 0b00000010) |(var & 0b00000001);
}

void addDay(){
	RTCGetTime(&RTC);
	uint8_t leapyear = 0, month = RTC.month, day = RTC.date;
	int year = RTC.year + 2000;
	if((RTC.year % 4 == 0) || ((RTC.year % 400 == 0)&&(RTC.year % 100 != 0)))
	leapyear = 1;
	
	if((month == 2) && (day == 28) && (leapyear))
	day++;
	else if(RTC.date >= daysInMonth[RTC.month-1]){
		day = 1;
		if(month == 12){
			month = 1;
			year++;
		}
		else
		month++;
	}
	else
	day++;
	RTCSetYear(year-2000);
	RTCSetMonth(month);
	RTCSetDate(day);
}

void remDay(){
	RTCGetTime(&RTC);
	uint8_t leapyear = 0, month = RTC.month, day = RTC.date;
	int year = RTC.year + 2000;
	if((RTC.year % 4 == 0) || ((RTC.year % 400 == 0)&&(RTC.year % 100 != 0)))
	leapyear = 1;
	
	if((month == 3) && (day == 1) && (leapyear)){
		day=29;
		month = 2;
	}
	else if(RTC.date == 1){
		if( (month == 1) ){
			year--;
			month = 12;
			day = 31;
		}
		else{
			month--;
			day = daysInMonth[month - 1];
		}
	}
	else
	day--;
	RTCSetYear(year-2000);
	RTCSetMonth(month);
	RTCSetDate(day);
}

void addMinute(){
	RTCGetTime(&RTC);
	uint8_t hour = RTC.hours, minutes = RTC.minutes;
	if(minutes == 59){
		if(hour == 23){
			addDay();
			hour = 0;
		}
		else
		hour++;
		minutes = 0;
	}
	else minutes++;
	RTCSetHours(hour);
	RTCSetMinutes(minutes);
	RTCSetSeconds(0);
}

void remMinute(){
	RTCGetTime(&RTC);
	uint8_t hour = RTC.hours, minutes = RTC.minutes;
	if(minutes == 00){
		if(hour == 00){
			remDay();
			hour = 23;
		}
		else
		hour--;
		minutes = 59;
	}
	else minutes--;
	RTCSetHours(hour);
	RTCSetMinutes(minutes);
	RTCSetSeconds(0);
}	
