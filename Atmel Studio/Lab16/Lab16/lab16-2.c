/*
 * FILE: lab16-2.c
 *
 * Replace the "<-YOUR CODE HERE->" comments with your code.
 */

#include <avr/io.h>
#include <avr/interrupt.h>

void output_string(char*);

/* Global variables */
/* 
** Circular buffer to hold outgoing characters. The insert_pos variable
** keeps track of the position (0 to BUFFER_SIZE-1) that the next
** outgoing character should be written to. bytes_in_buffer keeps
** count of the number of characters currently stored in the buffer 
** (ranging from 0 to BUFFER_SIZE). This number of bytes immediately
** prior to the current insert_pos are the bytes waiting to be output.
** If the insert_pos reaches the end of the buffer it will wrap around
** to the beginning (assuming those bytes have been output).
*/
#define BUFFER_SIZE 64
volatile char buffer[BUFFER_SIZE];
volatile unsigned char insert_pos;
volatile unsigned char bytes_in_buffer;

/*
** Text to be output in place of digits
*/
char* numbers[10] = {"zero", "one", "two", "three", "four",
		"five", "six", "seven", "eight", "nine"};

/*
 * main -- Main program.
 */
int main(void)
{
	/*
	** Initialise our buffer
	*/
	insert_pos = 0;
	bytes_in_buffer = 0;

	/* Set the baud rate to 9600.
	*/
	UBRR0 = /* <- YOUR CODE HERE -> */

	/*
	** Enable transmission and receiving via UART and also 
	** enable the Receive Complete Interrupt and the Data Register
	** Empty interrupt. This ensures that we get an interrupt
	** when the UART receives a character and when it is ready
	** to accept a new character for transmission.
	** (See pages 194-195 of the datasheet)
	*/
	UCSR0B = /* <- YOUR CODE HERE -> */

	/* Enable interrupts */
	sei();

	output_string("Testing\r\n");
	
	/* Sit back and let it happen - this will loop forever */
	for (;;) {
	}
}

/*
** output_char
**
** Procedure to output a character (by adding it to the outgoing buffer)
** (The characters will get consumed by an interrupt handler (see below).)
*/
void output_char(char c) {
	/* Add the character to the buffer for transmission if there
	** is space to do so. We advance the insert_pos to the next
	** character position. If this is beyond the end of the buffer
	** we wrap around back to the beginning of the buffer */
	/* NOTE: this only gets executed within an interrupt handler
	** so we can be guaranteed uninterrupted access to the buffer.
	*/
	if(bytes_in_buffer < BUFFER_SIZE) {
		/* We have room to add this byte */
		buffer[insert_pos++] = c;
		bytes_in_buffer++;
		if(insert_pos == BUFFER_SIZE) {
			/* Wrap around buffer pointer if necessary */
			insert_pos = 0;
		}
	} 
	/* else, we have no room to add the byte - just discard it */
}

/*
** output_string
**
** Procedure to output a string (by adding it to the outgoing buffer
** character by character). We iterate over all characters in the
** string. (Remember, strings are null-terminated.)
*/
void output_string(char* str) {
	unsigned char i;	/* index into the string */
	for(i=0; str[i] != 0; i++) {
		output_char(str[i]);
	}
}

/*
 * Define the interrupt handler for UART Data Register Empty (i.e. 
 * another character can be taken from our buffer and written out)
 */

/* <- YOUR CODE HERE -> */
{
	/* Check if we have data in our buffer */
	if(bytes_in_buffer > 0) {
		/* Yes we do - remove the pending byte and output it
		** via the UART. The pending byte (character) is the
		** one which is "bytes_in_buffer" characters before the 
		** insert_pos (taking into account that we may 
		** need to wrap around to the end of the buffer).
		*/
		char c;
		if(insert_pos - bytes_in_buffer < 0) {
			/* Need to wrap around */
			c = buffer[insert_pos - bytes_in_buffer
				+ BUFFER_SIZE];
		} else {
			c = buffer[insert_pos - bytes_in_buffer];
		}
		/* Decrement our count of the number of bytes in the 
		** buffer 
		*/
		bytes_in_buffer--;
		
		/* Output the character via the UART */
		UDR0 = c;
	}
	/* else, no data in buffer - do nothing. This will cause this
	** interrupt to trigger again immediately (unless there is an
	** interrupt of higher priority (e.g. Receive complete)). Our
	** program has nothing else to do so this is not a problem.
	*/
}

		
/*
 * Define the interrupt handler for UART Receive Complete - i.e. a new
 * character has arrived in the UART Data Register (UDR).
 */
/* <- YOUR CODE HERE -> */
{
	/* A character has been received - if it is not a number, then
	** put it into the transmit buffer to echo it back. If it is
	** a number than put the text equivalent into the transmit
	** buffer
	*/
	char input;

	/* Extract character from UART Data register and place in input
	** variable
	*/
	input = UDR0;

	/* If the character received is a digit (0 to 9), output the
	** string equivalent (i.e. see the numbers array above). 
	** If the character received is not a digit, just send it back.
	*/
	/* <-YOUR CODE HERE -> */
}


