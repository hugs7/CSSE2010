/*
 * FILE: lab13-2.c
 *
 * Replace the "<-YOUR CODE HERE->" comment lines with your code.
 *
 * A seven segment display is connected to port A, with the CC
 * (digit select) pin connected to port D, pin 0.
 * A push button will be connected to pin T0.
 * We will count the number of times that the push button 
 * is pressed using timer/counter 0. We will display this value on 
 * the seven segment display as a two digit number 00 -> 99. We
 * will alternate the digit display 1000 times per second so that
 * we can see both digits together.
 */

#include <avr/io.h>

/* Display digit function. Arguments are the digit number (0 to 9)
 * and the digit to display it on (0 = right, 1 = left). The function 
 * outputs the correct seven segment display value to port A and the 
 * correct digit select value to port D, pin 0.
 * See Lecture 14 example code for some code to base this on.
 */
void display_digit(uint8_t number, uint8_t digit) 
{
	/*<-YOUR CODE HERE->*/
}

/*
 * main -- Main program
 */
int main(void)
{
	uint8_t digit; /* 0 = right, 1 = left */
	uint8_t value;

	/* Set port A (all pins) to be outputs */
	/* <-YOUR CODE HERE-> */

    /* Set port D, pin 0 to be an output */
    /* <-YOUR CODE HERE-> */

	/* Set up timer/counter 0 to count the number of rising
	** edges on pin T0.
	*/
	/* <-YOUR CODE HERE -> */

	/* Set up timer/counter 1 so that it reaches an output compare
	** match every 1 millisecond (1000 times per second) and then
	** resets to 0.
	*/
	OCR1A = /* <-YOUR CODE HERE-> */;
	TCCR1A = /* <-YOUR CODE HERE-> */;
	TCCR1B = /* <-YOUR CODE HERE-> */;

	/* Repeatedly output the digits. We keep track of which 
	** digit. 0 = right (ones place), 1 = left (tens place) 
	*/
	digit = 0;
	while(1) {
		/* Output the current digit */
		if(digit == 0) {
			/* Extract the ones place from the timer counter 0 value */
			/* HINT: Consider the modulus (%) operator. */
			value = /* <-YOUR CODE HERE-> */;
		} else {
			/* Extract the tens place from the timer counter 0 */
			value = /* <-YOUR CODE HERE -> */;
		}
		display_digit(value, digit);
		/* Change the digit flag for next time. if 0 becomes 1, if 1 becomes 0. */
		digit = /* <-YOUR CODE HERE-> */;

		/* Wait for timer 1 to reach output compare A value.
		 * We can monitor the OCF1A bit in the TIFR1 register. When 
		 * it becomes 1, we know that the output compare value has
		 * been reached. We can write a 1 to this bit to clear it.
		 * See page 143-144 of datasheet for details.
		 */
		while ((TIFR1 & (1 << OCF1A)) == 0) {
		 	; /* Do nothing - wait for the bit to be set */
		}
		/* Clear the output compare flag - by writing a 1 to it. */
		TIFR1 &= (1 << OCF1A);
	}
}
