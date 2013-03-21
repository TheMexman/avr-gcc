/*
 * DS1307.c
 *
 * Created: 11/25/2012 7:44:31 PM
 *  Author: tony
 */ 
//INCLUDES
#include "USI_TWI_Master.h"
#include "DS1307.h"

//GLOBALS
const uint8_t DS1307 = 0x68; //I2C ADDR OF RTC MODULE
uint8_t RTCBuf [8]; //I2C Buffer
uint8_t noerror;		//error flag
//Declarations

uint8_t bcd2dec(uint8_t num){
	return ((num/16 * 10) + (num % 16));
}

 uint8_t dec2bcd (uint8_t num){ 
	return ((num/10 * 16) + (num % 10)); 
}
 
 uint8_t RTC24Hours(){
	RTCBuf[0] =	(DS1307<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);	//write to rtc
	RTCBuf[1] = 0x02;											//at 2 offset
	noerror = USI_TWI_Start_Read_Write( RTCBuf, 2 );			//write request
	
	RTCBuf[0] = (DS1307<<TWI_ADR_BITS) | (TRUE<<TWI_READ_BIT);	//read from rtc - get hours
	noerror = USI_TWI_Start_Read_Write( RTCBuf, 2 );			//read request
	
	return !(RTCBuf[1] & 0b0100000);							//Return if in 24 hour mode, 0 24h 1 12h, so invert
 }
  
 uint8_t RTCAfternoon(){ 
	RTCBuf[0] =	(DS1307<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);	//write to rtc
	RTCBuf[1] = 0x02;											//at 2 offset
	noerror = USI_TWI_Start_Read_Write( RTCBuf, 2 );			//write request
		
	RTCBuf[0] = (DS1307<<TWI_ADR_BITS) | (TRUE<<TWI_READ_BIT);	//read from rtc - get hours
	noerror = USI_TWI_Start_Read_Write( RTCBuf, 2 );			//read request
	
	if(RTC24Hours())											//Check if 24 hour mode
		return bcd2dec(RTCBuf[1])  >= 12 ? 1 : 0 ;				//If in 24 hour mode check time >= 12
	else return (RTCBuf[1] & 0b00100000);						//Otherwise return bit 5 which indicates AM or PM (0 or 1)
		 
 }

uint8_t RTCCheckRunning(){
	RTCBuf[0] =	(DS1307<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);	//write to rtc
	RTCBuf[1] = 0x00;											//at zero offset
	noerror = USI_TWI_Start_Read_Write( RTCBuf, 2 );			//write request
	
	RTCBuf[0] = (DS1307<<TWI_ADR_BITS) | (TRUE<<TWI_READ_BIT);	//read from rtc - get seconds
	noerror = USI_TWI_Start_Read_Write( RTCBuf, 2 );			//read request
	
	return !(RTCBuf[1] & 0b1000000);							//Return CH bit, 0 is started, 1 is stopped so invert
}

uint8_t RTCStopClock(){	
	RTCBuf[0] =	(DS1307<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);	//write to rtc
	RTCBuf[1] = 0x00;											//at zero offset
	noerror = USI_TWI_Start_Read_Write( RTCBuf, 2 );			//write request
	
	RTCBuf[0] = (DS1307<<TWI_ADR_BITS) | (TRUE<<TWI_READ_BIT);	//read from rtc - get seconds
	noerror = USI_TWI_Start_Read_Write( RTCBuf, 2 );			//read request
	uint8_t seconds = RTCBuf[1];
	
	RTCBuf[0] =	(DS1307<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);	//write to rtc
	RTCBuf[1] = 0x00;											//at zero offset
	RTCBuf[2] = seconds | 0b10000000;							//set CH bit
	noerror = USI_TWI_Start_Read_Write( RTCBuf,3 );			//write request
	
	return noerror;
}

uint8_t RTCStartClock(){
	RTCBuf[0] =	(DS1307<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);	//write to rtc
	RTCBuf[1] = 0x00;											//at zero offset
	noerror = USI_TWI_Start_Read_Write( RTCBuf, 2 );			//write request
	
	RTCBuf[0] = (DS1307<<TWI_ADR_BITS) | (TRUE<<TWI_READ_BIT);	//read from rtc - get seconds
	noerror = USI_TWI_Start_Read_Write( RTCBuf, 2 );			//read request
	uint8_t seconds = RTCBuf[1];
	
	RTCBuf[0] =	(DS1307<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);	//write to rtc
	RTCBuf[1] = 0x00;											//at zero offset
	RTCBuf[2] = seconds & ~(0b10000000);						//set CH bit, set MSB to 0 by anding with 1's complement of Mask
	noerror = USI_TWI_Start_Read_Write( RTCBuf, 3 );			//write request
	
	return noerror;
}

uint8_t RTCSetSeconds(uint8_t seconds){
	uint8_t bcds = dec2bcd(seconds);
	bcds &= ~(0b1000000);										//Make sure CH is not set
	
	RTCBuf[0] =	(DS1307<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);	//write to rtc
	RTCBuf[1] = 0x00;											//at zero offset
	RTCBuf[2] = bcds;											//write seconds
	noerror = USI_TWI_Start_Read_Write( RTCBuf, 3 );			//write request
	
	return noerror;
}

uint8_t RTCSetMinutes(uint8_t minutes){
	uint8_t bcdm = dec2bcd(minutes);
	
	RTCBuf[0] =	(DS1307<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);	//write to rtc
	RTCBuf[1] = 0x01;											//at zero offset
	RTCBuf[2] = bcdm;											//write minutes
	noerror = USI_TWI_Start_Read_Write( RTCBuf, 3 );			//write request
	
	return noerror;
}

uint8_t RTCSetHours(uint8_t	hours){
	uint8_t bcdh = dec2bcd(hours);
	//if(!RTC24Hours()){ //add a change of time, will need a set pm function
		//if(hours>)
	//}
	
	RTCBuf[0] =	(DS1307<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);	//write to rtc
	RTCBuf[1] = 0x02;											//at zero offset
	RTCBuf[2] = bcdh;								//write hours
	noerror = USI_TWI_Start_Read_Write( RTCBuf, 3 );			//write request
		
	return noerror;
}

uint8_t RTCSetDayOfWeek(uint8_t dow){
	if(dow > 7){
		return 0;  //return error if set DOW > 7
	}
	RTCBuf[0] =	(DS1307<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);	//write to rtc
	RTCBuf[1] = 0x03;											//at zero offset
	RTCBuf[2] = dow;											//write day of week
	noerror = USI_TWI_Start_Read_Write( RTCBuf, 3 );			//write request
		
	return noerror;
}

uint8_t RTCSetDate(uint8_t date){
	uint8_t bcdd = dec2bcd(date);
	RTCBuf[0] =	(DS1307<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);	//write to rtc
	RTCBuf[1] = 0x04;											//at zero offset
	RTCBuf[2] = bcdd;											//write day of week
	noerror = USI_TWI_Start_Read_Write( RTCBuf, 3 );			//write request
	
	return noerror;	
}

uint8_t RTCSetMonth(uint8_t month){
	uint8_t bcdm = dec2bcd(month);
	RTCBuf[0] =	(DS1307<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);	//write to rtc
	RTCBuf[1] = 0x05;											//at zero offset
	RTCBuf[2] = bcdm;											//write day of week
	noerror = USI_TWI_Start_Read_Write( RTCBuf, 3 );			//write request
	
	return noerror;	
}

uint8_t RTCSetYear(uint8_t year){
	uint8_t bcdy = dec2bcd(year);
	RTCBuf[0] =	(DS1307<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);	//write to rtc
	RTCBuf[1] = 0x06;											//at zero offset
	RTCBuf[2] = bcdy;											//write day of week
	noerror = USI_TWI_Start_Read_Write( RTCBuf, 3 );			//write request
	
	return noerror;
}

uint8_t RTCGetTime(RTCInfo *RTC){
	RTCBuf[0] = (DS1307<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT);
	RTCBuf[1] = 0x00;
	noerror = USI_TWI_Start_Read_Write( RTCBuf, 2 );
	
	RTCBuf[0] = (DS1307<<TWI_ADR_BITS) | (TRUE<<TWI_READ_BIT);
	noerror = USI_TWI_Start_Read_Write( RTCBuf, 8 );
	RTC->seconds = bcd2dec(RTCBuf[1] & 0x7f);
	RTC->minutes = bcd2dec(RTCBuf[2]);
	RTC->hours	 = bcd2dec(RTCBuf[3]);
	RTC->day	 = bcd2dec(RTCBuf[4]);
	RTC->date    = bcd2dec(RTCBuf[5]);
	RTC->month   = bcd2dec(RTCBuf[6]);
	RTC->year    = bcd2dec(RTCBuf[7]);
	
	return noerror;
}