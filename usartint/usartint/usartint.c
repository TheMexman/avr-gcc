/*
 * usartint.c
 */ 

//Defines
#define F_CPU 8000000UL
//Headers
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

//Prototypes
uint16_t CalculateBaudRate(uint32_t bps);
void USART0_Init(uint32_t bps);
void USART1_Init(uint32_t bps);

int main (void)
{
	USART0_Init(9600);
	USART1_Init(9600);
	sei() ; // Enable the Global Interrupt Enable flag so that interrupts can be processed
	for (;;) // Loop forever
	{
		// Do nothing - echoing is handled by the ISR instead of in the main loop
	}
	return 0;
}

ISR ( USART0_RX_vect )
{
	char ReceivedByte ;
	ReceivedByte = UDR0 ; // Fetch the received byte value into the variable "ByteReceived"
	UDR0 = ReceivedByte ; // Echo back the received byte back to the computer
}

ISR ( USART1_RX_vect )
{
	char ReceivedByte ;
	ReceivedByte = UDR1 ; // Fetch the received byte value into the variable "ByteReceived"
	UDR1 = ReceivedByte ; // Echo back the received byte back to the computer
}

uint16_t CalculateBaudRate(uint32_t baud){
	
	//Calculate UBBR
	//TODO modifiy to use 8* or 4* depending on doublesp ceed or async
	return (F_CPU/(16*baud)) - 1;
}

void USART0_Init(uint32_t baud){
	
	//Calculate and set the baud rate
	uint16_t UBBR1 = CalculateBaudRate(baud);
	UBRR0H = (unsigned char)(UBBR1>>8);  //4MSBs of Baud Rate in UBBR0H
	UBRR0L = (unsigned char)UBBR1;		 //8LSBs of Baud Rate in UBBR0L
	
	//Enable USART TX and RX
	UCSR0B = ((1 << RXEN0) | (1 << TXEN0));
	
	//Set Frame Format - 8N1 ASYNC 0 Pol
	UCSR0C &= ~(1 << UMSEL01) | ~(1 << UMSEL00) | ~(1 << UPM01) | ~(1 << UPM00) | ~(1 << USBS0) | ~(1 << UCSZ02) | (1 << UCSZ01) | (1 << UCSZ00) | ~(1 << UCPOL0);
	
	//Enable RX Interrupt
	UCSR0B |= (1 << RXCIE0 );
	
}

void USART1_Init(uint32_t baud){
	
	//Calculate and set the baud rate
	uint16_t UBBR1 = CalculateBaudRate(baud);
	UBRR1H = (unsigned char)(UBBR1>>8);  //4MSBs of Baud Rate in UBBR0H
	UBRR1L = (unsigned char)UBBR1;		 //8LSBs of Baud Rate in UBBR0L
	
	//Enable USART TX and RX
	UCSR1B = ((1 << RXEN1) | (1 << TXEN1));
	
	//Set Frame Format - 8N1 ASYNC 0 Pol
	UCSR1C &= ~(1 << UMSEL11) | ~(1 << UMSEL10) | ~(1 << UPM11) | ~(1 << UPM10) | ~(1 << USBS1) | ~(1 << UCSZ12) | (1 << UCSZ11) | (1 << UCSZ10) | ~(1 << UCPOL1);
	
	//Enable RX Interrupt
	UCSR1B |= (1 << RXCIE1 );
}