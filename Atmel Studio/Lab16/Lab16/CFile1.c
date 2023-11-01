/*
 * lab16-3.c
 *
 * Program which reads analog voltages on ADC0 (X) and ADC1 (Y) pins
 * and outputs the digital equivalent to the serial port. 
 * This uses a separate serial IO library which (a) requires
 * interrupts and (b) must be initialised using init_serial_stdio().
 * Standard input/output functions (printf etc.) are hooked up to the 
 * serial connection.
 */ 

#include "serialio.h"
#include <stdio.h>
#include <avr/interrupt.h>

int main(void)
{
	uint16_t value;
	uint8_t x_or_y = 0;	/* 0 = x, 1 = y */
	
	/* Set up the serial port for stdin communication at 19200 baud, no echo */
	init_serial_stdio(19200,0);
	
	/* Turn on global interrupts */
	sei();
	
	// Set up ADC - AVCC reference, right adjust
	// Input selection doesn't matter yet - we'll swap this around in the while
	// loop below.
	ADMUX = (1 << REFS0);
	// Turn on the ADC (but don't start a conversion yet). Choose a clock
	// divider of 64. (The ADC clock must be somewhere
	// between 50kHz and 200kHz. We will divide our 8MHz clock by 64
	// to give us 125kHz.)
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1);
	
	/* Print a welcome message
	*/
	printf("ADC Test\n");
	
	while(1) {
		// Set the ADC mux to choose ADC0 if x_or_y is 0, ADC1 if x_or_y is 1
		if(x_or_y == 0) {
			ADMUX |= (0 << MUX0);
		} else {
			ADMUX |= (1 << MUX0)
		}
		// Start the ADC conversion
		ADCSRA |= (1 << ADSC)
		
		while(ADCSRA & (1<<ADSC)) {
			; /* Wait until conversion finished */
		}
		value = ADC; // read the value
		if(x_or_y == 0) {
			printf("X: %4d ", value);
		} else {
			printf("Y: %4d\n", value);
		}
		// Next time through the loop, do the other direction
		x_or_y ^= 1;
	}
}
