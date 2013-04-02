#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>

/* Prototypes */
void USART0_Init(uint32_t bps);
unsigned char USART0_Receive( void );
void USART0_Transmit( unsigned char data );
void USART_putstring(char* StringPtr);
uint16_t CalculateBaudRate(uint32_t bps);

/* Main - a simple test program*/
int main( void )
{
	USART0_Init( 250000 ); /* Set the baudrate to 19,200 bps using a 3.6864MHz crystal */

	for(;;) 	    /* Forever */
	{
		//USART0_Transmit( USART0_Receive() ); /* Echo the received character */
		USART_putstring("Hello, world!\n");
		_delay_ms(1000);
	}
}

void USART_putstring(char* StringPtr){
	
	while(*StringPtr != 0x00){    //Here we check if there is still more chars to send, this is done checking the actual char and see if it is different from the null char
		USART0_Transmit(*StringPtr);    //Using the simple send function we send one char at a time
	StringPtr++;}        //We increment the pointer so we can read the next char
	
}

/* Initialize UART */
//void USART0_Init( unsigned int baudrate )
//{
	///* Set the baud rate */
	//UBRR0H = (unsigned char) (baudrate>>8);
	//UBRR0L = (unsigned char) baudrate;
	//
	///* Enable UART receiver and transmitter */
	//UCSR0B = ( ( 1 << RXEN0 ) | ( 1 << TXEN0 ) );
	//
	///* Set frame format: 8 data 2stop */
	//UCSR0C = (1<<USBS0)|(1<<UCSZ01)|(1<<UCSZ00);              //For devices with Extended IO
	////UCSR0C = (1<<URSEL)|(1<<USBS0)|(1<<UCSZ01)|(1<<UCSZ00);   //For devices without Extended IO
//}

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

void USART0_Transmit( unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !(UCSR0A & (1<<UDRE0)) )
	;
	/* Start transmittion */
	UDR0 = data;
}	
