/*
 * FILE: lab13-1.c
 *
 * Replace the "<-YOUR CODE HERE->" comment lines with your code.
 */

#include <avr/io.h>

/*
 * main -- Main program
 */
int main(void)
{
	/* Set OC1A pin to be an output */
	// OC1A is pin 19 PD5
	DDRD = (1 << 5);

	/* Set output compare register value */
	OCR1A = 15624;

	/* Set timer counter control registers A and B so that
	 *  - mode is - clear counter on compare match
	 *  - output compare match action is to toggle pin OC1A
	 *  - correct clock prescale value is chosen.
	 * TCCR1C can just stay as default value (0).
	 */
	TCCR1A = (1 << COM1A0);
	TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10);

	/* Do nothing forever - the hardware takes care of everything */
	/* <- YOUR CODE HERE -> */
	while (1) {
		;
	}
}
