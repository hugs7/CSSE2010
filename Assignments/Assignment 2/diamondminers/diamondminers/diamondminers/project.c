/*
 * project.c
 *
 * Main file for IN students
 *
 * Authors: Peter Sutton, Luke Kamols
 * Diamond Miners Inspiration: Daniel Cumming
 * Modified by Hugo Burton
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "game.h"
#include "display.h"
#include "ledmatrix.h"
#include "buttons.h"
#include "serialio.h"
#include "terminalio.h"
#include "timer0.h"

#define F_CPU 8000000L
#include <util/delay.h>

// Function prototypes - these are defined below (after main()) in the order
// given here
void initialise_hardware(void);
void start_screen(void);
void new_game(void);
void play_game(void);
void handle_game_over(void);

/////////////////////////////// main //////////////////////////////////
int main(void) {
	// Setup hardware and call backs. This will turn on 
	// interrupts.
	initialise_hardware();
	
	// Show the splash screen message. Returns when display is complete
	start_screen();
	
	// Loop forever,
	while(1) {
		new_game();
		play_game();
		handle_game_over();
	}
}

void initialise_hardware(void) {
	ledmatrix_setup();
	init_button_interrupts();
	// Setup serial port for 19200 baud communication with no echo
	// of incoming characters
	init_serial_stdio(19200,0);
	
	init_timer0();
	
	// Turn on global interrupts
	sei();
	
	// PIN OUTS
	DDRD |= (1 << 3);	// Diamond Indicator LED
	DDRD |= (1 << 4);	// Danger LED
	
	DDRC |= 0xFF;		// SSD
	DDRD |= (1 << 2);	// Digit select
	
	// SSD Timer
	
	// OCR mode every 1 millisecond
	// then reset to 0
	// divide clock by 8 and count 100 cycles (0 to 999)
	
	OCR1A = 999;
	TCCR1A = (0 << COM1A1) | (1 << COM1A0) | (0 << WGM11) | (0 << WGM10);	// toggle OC1A on compare match, least 2 significant WGM bits
	TCCR1B = (0 << WGM13) | (1 << WGM12) | (0 << CS12) | (1 << CS11) | (0 << CS10); // Two most significant WGM bits, divide clock by 8
	
	// joystick
	
	ADMUX = (1 << REFS0);
	
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1);
}

void start_screen(void) {
	// Clear terminal screen and output a message
	clear_terminal();
	move_terminal_cursor(10,10);
	printf_P(PSTR("Diamond Miners"));
	move_terminal_cursor(10,12);
	printf_P(PSTR("CSSE2010 project by Hugo Burton"));
	move_terminal_cursor(1,1);
	
	// Output the static start screen and wait for a push button 
	// to be pushed or a serial input of 's'
	start_display();
	
	// Wait until a button is pressed, or 's' is pressed on the terminal
	while(1) {
		// First check for if a 's' is pressed
		// There are two steps to this
		// 1) collect any serial input (if available)
		// 2) check if the input is equal to the character 's'
		char serial_input = -1;
		if (serial_input_available()) {
			serial_input = fgetc(stdin);
		}
		// If the serial input is 's', then exit the start screen
		if (serial_input == 's' || serial_input == 'S') {
			break;
		}
		// Next check for any button presses
		int8_t btn = button_pushed();
		if (btn != NO_BUTTON_PUSHED) {
			break;
		}
	}
}

void new_game(void) {
	// Clear the serial terminal
	clear_terminal();
	
	move_terminal_cursor(45,2);
	printf_P(PSTR("Diamonds Collected: 0"));
	move_terminal_cursor(45,3);
	printf_P(PSTR("Cheats: off"));
	move_terminal_cursor(1,1);
	
	// Initialise the game and display
	initialise_game();
	
	// Clear a button push or serial input if any are waiting
	// (The cast to void means the return value is ignored.)
	(void)button_pushed();
	clear_serial_input_buffer();
}

void play_game(void) {
	
	uint32_t last_flash_time, current_time, last_diamond_time, last_joy_time;
	uint8_t btn; //the button pushed
	uint32_t bomb_time = 0;
	uint8_t bomb_placed = 0;
	uint8_t steps = 0;
	uint8_t digit;
	uint8_t value;
	uint8_t x_or_y = 0;		/* 0 = x, 1 = y */
	uint16_t joy;
	
	last_flash_time = get_current_time();
	last_diamond_time = get_current_time();
	last_joy_time = get_current_time();
	char serial_input = -1;
	
	digit = 0;
	// We play the game until it's over
	while(!is_game_over()) {
		// We need to check if any button has been pushed, this will be
		// NO_BUTTON_PUSHED if no button has been pushed
		btn = button_pushed();
		
		if (btn == BUTTON0_PUSHED) {	// right
			// If button 0 is pushed, move right, i.e increase x by 1 and leave
			// y the same
			move_player(1, 0);
		} else if (btn == BUTTON1_PUSHED) {		// down
			move_player(0, -1);
		} else if (btn == BUTTON2_PUSHED) {		// up
			move_player(0, 1);
		} else if (btn == BUTTON3_PUSHED) {		// left
			move_player(-1, 0);
		} else if (serial_input_available()) {
			serial_input = fgetc(stdin);
			if (serial_input == 'p' || serial_input == 'P') {		// Pause
				toggle_pause();
			}
			
			if (get_paused() == 0) {
				if (serial_input == 'd' || serial_input == 'D') {	// right
					move_player(1, 0);
				} else if (serial_input == 's' || serial_input == 'S') {	// down
					move_player(0, -1);
				} else if (serial_input == 'w' || serial_input == 'W') {	// up
					move_player(0, 1);
				} else if (serial_input == 'a' || serial_input == 'A') {	// left
					move_player(-1, 0);
				} else if (serial_input == 'e' || serial_input == 'E') {	// inspect
					// inspect block
					inspect_block();
				} else if (serial_input == 'c' || serial_input == 'C') {	// cheats
					toggle_cheats();
				} else if (serial_input == ' ') {
					if (bomb_placed == 0) {
						bomb_placed = 1;
						place_bomb();
						// set timer
						bomb_time = get_current_time();
					}
				} else if (serial_input == 'f' || serial_input == 'F') {	// field of view
					toggle_field_of_view();
				}
			}
			
		}
		
		// SSD step counter
		
		steps = get_steps();
		
		if (digit == 0) {
			value = steps % 10;
			} else {
			value = (steps / 10) % 10;
		}
		
		display_digit(value, digit);
		digit = 1 - digit;
		
		// timer
		
		while ((TIFR1 & (1 << OCF1A)) == 0) {
			;	/* Do nothing - wait for the bit to be set */
		}
		
		/* Clear the output compare flag - by writing a 1 to it. */
		TIFR1 &= (1 << OCF1A);
		
		// when paused skip after this if statement
		
		if (get_paused() == 1) {
			continue;
		}
		
		current_time = get_current_time();
		if(current_time >= last_flash_time + 500) {
			// 500ms (0.5 second) has passed since the last time we
			// flashed the cursor, so flash the cursor
			flash_facing();
			
			// Update the most recent time the cursor was flashed
			last_flash_time = current_time;
			
		}
		
		// diamond led
		// update distance to diamond led
		int8_t d_dis = distance_to_diamond();
		int flash = 0;
		if (d_dis == 4) {
			flash = 1500;
		} else if (d_dis == 3) {
			flash = 1000;
		} else if (d_dis == 2) {
			flash = 500;
		} else if (d_dis == 1) {
			flash = 250;
		}
		
		current_time = get_current_time();
		if (flash != 0) {
			if (current_time >= last_diamond_time + flash) {
				flash_diamond();
				
				last_diamond_time = current_time;
			}
		}
		
		// Bombs
		
		current_time = get_current_time();
		if (bomb_placed == 1) {
			if (current_time >= (bomb_time + 2000)) {
				bomb_placed = 2;
				detonate_bomb();
				bomb_graphics();
				check_danger();
			}
			
			if (current_time == (bomb_time + 250)) {
				flash_bomb();
			} else if (current_time == (bomb_time + 500)) {
				flash_bomb();
			} else if (current_time == (bomb_time + 750)) {
				flash_bomb();
			} else if (current_time == (bomb_time + 1000)) {
				flash_bomb();
			} else if (current_time == (bomb_time + 1250)) {
				flash_bomb();
			} else if (current_time == (bomb_time + 1500)) {
				flash_bomb();
			} else if (current_time == (bomb_time + 1600)) {
				flash_bomb();
			} else if (current_time == (bomb_time + 1700)) {
				flash_bomb();
			} else if (current_time == (bomb_time + 1800)) {
				flash_bomb();
			} else if (current_time == (bomb_time + 1800)) {
				flash_bomb();
			} else if (current_time == (bomb_time + 1900)) {
				flash_bomb();
			} else if (current_time == (bomb_time + 1980)) {
				flash_bomb();
			}
			
			// danger led
			check_danger();
		}
		
		if (bomb_placed == 2) {
			current_time = get_current_time();
			if (current_time >= (bomb_time + 2100)) {
				bomb_graphics();
				bomb_placed = 0;
				bomb_time = 0;
			}
		}
		
		// Joystick
		
		if(x_or_y == 0) {
			ADMUX &= ~1;
		} else {
			ADMUX |= 1;
		}
		
		ADCSRA |= (1 << ADSC);
		
		while (ADCSRA & (1 << ADSC)) {
			;
		}
		
		joy = ADC; // get joystick value
		
		/* 0 = x, 1 = y */
		
		current_time = get_current_time();
		if (current_time >= last_joy_time + pow(abs(500 - joy),-1) * 25000) {
		
			if (x_or_y == 0) {	// x
				if (joy > 700) {	// right
					move_player(1, 0);
				} else if (joy < 300) {		// left
					move_player(-1, 0);		
				}
			} else {		// y
				if (joy > 700) {	// up
					move_player(0, 1);
					} else if (joy < 300) {		// down
					move_player(0, -1);
				}
			}
			
			last_joy_time = get_current_time();
		}
		
		x_or_y ^= 1;	// do other axis next time
	}
	
	// We get here if the game is over.
}

void handle_game_over() {
	char serial_input = -1;
	
	move_terminal_cursor(10,14);
	printf_P(PSTR("GAME OVER"));
	move_terminal_cursor(10,15);
	printf_P(PSTR("Press a button to start again"));
	
	while(button_pushed() == NO_BUTTON_PUSHED) {
		// check also for keyboard input
		if (serial_input_available()) {
			serial_input = fgetc(stdin);
			if (serial_input != -1) {
				break;
			}
		}
	}
}
