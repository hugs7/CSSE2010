#include <avr/io.h>

/* Seven segment display values */
uint8_t seven_seg[16] = { 63,6,91,79,102,109,125,7,127,111,119,124,57,94,121,113};

int main(void) {
	uint8_t digit;
	
	/* Set port A pins to be outputs, port C pins to be inputs */
	DDRA = 0xFF;
	DDRC = 0;    /* This is the default, could omit. */
	DDRB = 0x04F;
	DDRD = 0x01;
	PORTB = 0x00;
	PORTD = 0x00;
	while(1) {
		/* Read in a digit from lower half of port C pins */
		/* We read the whole byte and mask out upper bits */
		digit = PINC & 0x0F;
		/* Write out seven segment display value to port A */
		PORTA = seven_seg[digit];
		PORTB = PINC;
		PORTD ^= 0x01;
		PORTB ^= 0x40;
	}
}