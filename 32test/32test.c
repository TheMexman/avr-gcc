#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

/* Prototypes */
void USART0_Init(uint32_t bps);
void USART1_Init(uint32_t bps);
unsigned char USART0_Receive( void );
unsigned char USART1_Receive( void );
void USART0_Transmit( unsigned char data );
void USART1_Transmit( unsigned char data );
void USART0_putstring(const char* StringPtr);
void USART1_putstring(const char* StringPtr);
uint16_t CalculateBaudRate(uint32_t bps);

/*Globals*/
const char hello[] PROGMEM = "Hello, World!\n";				//PROGMEM must be global

/* Main - a simple test program*/
int main( void )
{
	USART0_Init( 250000 ); 
	USART1_Init( 250000 ); 


	for(;;) 	    /* Forever */
	{
		//USART0_Transmit( USART0_Receive() ); /* Echo the received character */
		USART0_putstring(hello);
		USART1_putstring(PSTR("Goodbye, world!\n"));
		_delay_ms(1000);
	}
}

void USART0_putstring(const char* StringPtr){
	
	while(pgm_read_byte(StringPtr) != 0x00){    //Here we check if there is still more chars to send, this is done checking the actual char and see if it is different from the null char
		USART0_Transmit(pgm_read_byte(StringPtr));    //Using the simple send function we send one char at a time
		StringPtr++;
		}        //We increment the pointer so we can read the next char
	
}

void USART1_putstring(const char* StringPtr){
	
	while(pgm_read_byte(StringPtr) != 0x00){    //Here we check if there is still more chars to send, this is done checking the actual char and see if it is different from the null char
		USART1_Transmit(pgm_read_byte(StringPtr));     //Using the simple send function we send one char at a time
		StringPtr++;
		}        //We increment the pointer so we can read the next char
	
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
	
}

uint16_t CalculateBaudRate(uint32_t baud){
	
	//Calculate UBBR
	return (F_CPU/(16*baud)) - 1;
}


/* Read and write functions */
unsigned char USART0_Receive( void )
{
	/* Wait for incomming data */
	while ( !(UCSR0A & (1<<RXC0)) )
	;
	/* Return the data */
	return UDR0;
}

unsigned char USART1_Receive( void )
{
	/* Wait for incomming data */
	while ( !(UCSR1A & (1<<RXC1)) )
	;
	/* Return the data */
	return UDR1;
}

void USART0_Transmit( unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !(UCSR0A & (1<<UDRE0)) )
	;
	/* Start transmittion */
	UDR0 = data;
}	

void USART1_Transmit( unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !(UCSR1A & (1<<UDRE1)) )
	;
	/* Start transmittion */
	UDR1 = data;
}
