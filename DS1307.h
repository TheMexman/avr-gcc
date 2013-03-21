/*
 * DS1307.h
 *
 * Created: 11/25/2012 7:44:44 PM
 *  Author: tony
 */ 

#include <stdint.h>

#ifndef DS1307_H_
#define DS1307_H_

//Variables
typedef struct {
	uint8_t	seconds;
	uint8_t	minutes;
	uint8_t	hours;
	uint8_t	day;
	uint8_t	date;
	uint8_t	month;
	uint8_t	year;
} RTCInfo;

//Function Prototypes
uint8_t bcd2dec(uint8_t num);
uint8_t dec2bcd (uint8_t num);
uint8_t RTC24Hours();
uint8_t RTCAfternoon();
uint8_t RTCCheckRunning();
uint8_t RTCStopClock();
uint8_t RTCStartClock();
uint8_t RTCSetSeconds(uint8_t seconds);
uint8_t RTCSetMinutes(uint8_t minutes);
uint8_t RTCSetHours(uint8_t	hours);
uint8_t RTCSetDayOfWeek(uint8_t dow);
uint8_t RTCSetDate(uint8_t date);
uint8_t RTCSetMonth(uint8_t month);
uint8_t RTCSetYear(uint8_t year);
uint8_t	RTCGetTime(RTCInfo *RTC);

#endif /* DS1307_H_ */