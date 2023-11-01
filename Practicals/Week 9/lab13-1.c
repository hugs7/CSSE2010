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
	/* <-YOUR CODE HERE-> */

	/* Set output compare register value */
	OCR1A = /* <-YOUR CODE HERE-> */;

	/* Set timer counter control registers A and B so that
	 *  - mode is - clear counter on compare match
	 *  - output compare match action is to toggle pin OC1A
	 *  - correct clock prescale value is chosen.
	 * TCCR1C can just stay as default value (0).
	 */
	TCCR1A = /* <-YOUR CODE HERE-> */;
	TCCR1B = /* <-YOUR CODE HERE-> */;

	/* Do nothing forever - the hardware takes care of everything */
	/* <- YOUR CODE HERE -> */
}
