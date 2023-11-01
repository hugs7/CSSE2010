/*
 * game.c
 *
 * Contains functions relating to the play of Diamond Miners
 *
 * Author: Luke Kamols
 */ 

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>

#include "stdio.h"
#include "terminalio.h"
#include "game.h"
#include "display.h"

#define PLAYER_START_X  0;
#define PLAYER_START_Y  0;
#define FACING_START_X  1;
#define FACING_START_Y  0;

void handle_game_over();
void new_game(void);

// the initial game layout
// the values 0, 3, 4 and 5 are defined in display.h
// note that this is not laid out in such a way that starting_layout[x][y]
// does not correspond to an (x,y) coordinate but is a better visual
// representation

static const int NUM = 2;

static const uint8_t levels [2][HEIGHT][WIDTH] = 
		{
			{
				{0, 3, 0, 3, 0, 0, 0, 4, 4, 0, 0, 4, 0, 4, 0, 4},
				{0, 4, 0, 4, 0, 0, 0, 3, 4, 4, 3, 4, 0, 3, 0, 4},
				{0, 4, 0, 4, 4, 4, 4, 0, 3, 0, 0, 0, 0, 4, 0, 4},
				{5, 4, 0, 4, 0, 0, 3, 0, 0, 4, 0, 0, 0, 4, 0, 0},
				{4, 4, 3, 4, 5, 0, 4, 0, 0, 4, 3, 4, 0, 0, 4, 4},
				{0, 0, 0, 4, 4, 4, 4, 0, 4, 0, 0, 0, 4, 3, 0, 4},
				{0, 0, 0, 3, 0, 0, 3, 0, 3, 0, 3, 0, 3, 0, 0, 4},
				{0, 0, 0, 4, 0, 0, 3, 0, 4, 0, 0, 3, 3, 0, 5, 4}
			},
			
			{
				{0, 0, 0, 3, 0, 0, 0, 0, 3, 0, 0, 0, 0, 3, 0, 0},
				{5, 4, 4, 4, 0, 0, 0, 0, 4, 0, 0, 0, 4, 4, 4, 0},
				{4, 4, 3, 0, 0, 4, 4, 4, 4, 4, 4, 3, 4, 5, 3, 0},
				{0, 0, 0, 3, 0, 3, 0, 0, 0, 5, 4, 0, 3, 0, 3, 0},
				{3, 4, 3, 4, 0, 4, 0, 0, 4, 4, 4, 0, 3, 0, 4, 4},
				{0, 0, 4, 4, 3, 4, 3, 4, 4, 0, 4, 0, 4, 4, 4, 0},
				{0, 0, 0, 3, 0, 4, 0, 0, 3, 0, 4, 0, 0, 3, 3, 0},
				{0, 0, 0, 4, 0, 3, 0, 0, 4, 0, 4, 0, 0, 4, 4, 0}
			}
		};
		
#define NUM_DIRECTIONS 8
static const uint8_t directions[NUM_DIRECTIONS][2] = { {0,1}, {0,-1}, {1,0}, {-1,0}};
static const uint8_t seven_seg[10] = {63, 6, 91, 79, 102, 109, 125, 7, 127, 111};

// variables for the current state of the game
uint8_t playing_field[WIDTH][HEIGHT]; // what is currently located at each square
uint8_t visible[WIDTH][HEIGHT]; // whether each square is currently visible
uint8_t player_x;
uint8_t player_y;
uint8_t facing_x;
uint8_t facing_y;
uint8_t facing_visible;
uint8_t cheats;
uint8_t diamonds;
uint8_t bomb_x, bomb_y;
uint8_t steps;
uint8_t paused;
uint8_t level_select = 0;
uint8_t game_over;
uint8_t field_of_view;
uint8_t bomb_view;
uint8_t bomb_graphics_enabled = 0;

// function prototypes for this file
void discoverable_dfs(uint8_t x, uint8_t y);
void initialise_game_display(void);
void initialise_game_state(void);

/*
 * initialise the game state, sets up the playing field, visibility
 * the player and the player direction indicator
 */
void initialise_game_state(void) {
	// initialise the player position and the facing position
	player_x = PLAYER_START_X;
	player_y = PLAYER_START_Y;
	facing_x = FACING_START_X;
	facing_y = FACING_START_Y;
	facing_visible = 1;
	
	cheats = 0;
	diamonds = 0;
	steps = 0;
	paused = 0;
	game_over = 0;
	field_of_view = 0;
		
	// go through and initialise the state of the playing_field
	for (int x = 0; x < WIDTH; x++) {
		for (int y = 0; y < HEIGHT; y++) {
			// initialise this square based on the starting layout
			// the indices here are to ensure the starting layout
			// could be easily visualised when declared
			//playing_field[x][y] = starting_layout[HEIGHT - 1 - y][x];  // original
			playing_field[x][y] = levels[level_select % NUM][HEIGHT - 1 - y][x];

			// set all squares to start not visible, this will be
			// updated once the display is initialised as well
			visible[x][y] = 0;
		}
	}	
}

/*
 * initialise the display of the game, shows the player and the player
 * direction indicator. 
 * executes a visibility search from the player's starting location
 */
void initialise_game_display(void) {
	// initialise the display
	initialise_display();
	// make the entire playing field undiscovered to start
	for (int x = 0; x < WIDTH; x++) {
		for (int y = 0; y < HEIGHT; y++) {
			update_square_colour(x, y, UNDISCOVERED);
		}
	}
	// now explore visibility from the starting location
	discoverable_dfs(player_x, player_y);
	// make the player and facing square visible
	update_square_colour(player_x, player_y, PLAYER);
	update_square_colour(facing_x, facing_y, FACING);
}

void initialise_game(void) {
	// to initialise the game, we need to initialise the state (variables)
	// and the display
	initialise_game_state();
	initialise_game_display();
}

uint8_t in_bounds(uint8_t x, uint8_t y) {
	// a square is in bounds if 0 <= x < WIDTH && 0 <= y < HEIGHT
	return x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT;
}

uint8_t get_object_at(uint8_t x, uint8_t y) {
	// check the bounds, anything outside the bounds
	// will be considered an unbreakable wall
	// next level portal
	if (x == 16 && y == 4) {
		// check for existing diamonds
		
		for (int x = 0; x < WIDTH; x++) {
			for (int y = 0; y < HEIGHT; y++) {
				uint8_t object = get_object_at(x, y);
				if (object == DIAMOND) {
					return UNBREAKABLE;
				}
			}
		}
		
		// if no diamonds left
		// change level
		level_select++;
		
		return NEXT_LEVEL;
	} else if (!in_bounds(x,y)) {
		return UNBREAKABLE;
	} else {
		//if in the bounds, just index into the array
		return playing_field[x][y];
	}
}

void flash_facing(void) {
	// only flash the facing cursor if it is in bounds
	if (in_bounds(facing_x, facing_y)) {
		if (facing_visible) {
			// we need to flash the facing cursor off, it should be replaced by
			// the colour of the piece which is at that location
			uint8_t piece_at_cursor = get_object_at(facing_x, facing_y);
			update_square_colour(facing_x, facing_y, piece_at_cursor);
		
		} else {
			// we need to flash the facing cursor on
			update_square_colour(facing_x, facing_y, FACING);
		}
		facing_visible = 1 - facing_visible; //alternate between 0 and 1
	}
}

void flash_diamond(void) {
	if (paused == 1) {
		return;
	}
	
	PORTD ^= (1 << 3);
}

uint8_t get_steps() {
	return steps;
}

// check the header file game.h for a description of what this function should do
// it contains a few extra hints
void move_player(uint8_t dx, uint8_t dy) {
	update_square_colour(player_x, player_y, get_object_at(player_x, player_y));		// set current square black
	
	//2
	uint8_t new_player_x = player_x + dx;
	uint8_t new_player_y = player_y + dy;
	// check object at new position
	uint8_t object_new = get_object_at(new_player_x, new_player_y);
	
	if (object_new == NEXT_LEVEL) {
		new_game();
	}
	// if can move to position (i.e. not a wall)
	
	if (object_new == EMPTY_SQUARE || object_new == FACING || object_new == DIAMOND) {
		// update position of player
		player_x = new_player_x;
		player_y = new_player_y;
		
		if (object_new == DIAMOND) {
			playing_field[player_x][player_y] = EMPTY_SQUARE;
			update_square_colour(facing_x, facing_y, get_object_at(player_x, player_y));
			diamonds += 1;
			
			// Update console
			move_terminal_cursor(65,2);
			printf("%d", diamonds);
			move_terminal_cursor(1,1);
		}
		
		if (steps < 99) {
			steps += 1;
		}
	}
	
	// FOV
	fov_update();
	
	// update player pixels
	update_square_colour(facing_x, facing_y, get_object_at(facing_x, facing_y));
	update_square_colour(player_x, player_y, PLAYER);
	
	// update facing
	facing_x = player_x + dx;
	facing_y = player_y + dy;
	
	flash_facing();
	
	// suggestions for implementation:
	// 1: remove the display of the player at the current location
	//    (and replace it with whatever else is at that location)
	// 2: determine if a move is possible
	// 3: if the player can move, update the positional knowledge of the player, 
	//    this will include variables player_x and player_y
	// 4: display the player at the new location
}

uint8_t is_game_over(void) {
	// initially the game never ends
	return game_over;
}

/*
 * given an (x,y) coordinate, perform a depth first search to make any
 * squares reachable from here visible. If a wall is broken at a position
 * (x,y), this function should be called with coordinates (x,y)
 * YOU SHOULD NOT NEED TO MODIFY THIS FUNCTION
 */
void discoverable_dfs(uint8_t x, uint8_t y) {
	uint8_t x_adj, y_adj, object_here;
	int left_bound = player_x - 2;
	int right_bound = player_x + 2;
	int bottom_bound = player_y - 2;
	int top_bound = player_y + 2;
	
	// set the current square to be visible and update display
	visible[x][y] = 1;
	object_here = get_object_at(x, y);
	if (field_of_view == 1) {
		if (x >= left_bound && x <= right_bound && y >= bottom_bound && y <= top_bound) {
			if (!((x == left_bound || x == right_bound) && (y == bottom_bound || y == top_bound))) {
				update_square_colour(x, y, object_here);
			}
			
		}
	} else {
		update_square_colour(x, y, object_here);
	}
	
	
	// we can continue exploring from this square if it is empty
	if (object_here == EMPTY_SQUARE || object_here == DIAMOND) {
		// consider all 4 adjacent square
		for (int i = 0; i < NUM_DIRECTIONS; i++) {
			x_adj = x + directions[i][0];
			y_adj = y + directions[i][1];
			// if this square is not visible yet, it should be explored
			if (in_bounds(x_adj, y_adj) && !visible[x_adj][y_adj]) {
				// this recursive call implements a depth first search
				// the visible array ensures termination
				discoverable_dfs(x_adj, y_adj);
			}
		}
	}
}

void inspect_block(void) {
	int8_t inspected = get_object_at(facing_x, facing_y);
	if (inspected == BREAKABLE || inspected == DIS_BREAK) {
		int8_t block;
		if (cheats) {
			block = EMPTY_SQUARE;
		} else {
			block = DIS_BREAK;
		}
		playing_field[facing_x][facing_y] = block;
		update_square_colour(facing_x, facing_y, get_object_at(facing_x, facing_y));
		discoverable_dfs(facing_x, facing_y);
	}
}

void toggle_cheats(void) {
	cheats = 1 - cheats;
	
	// update console
	move_terminal_cursor(53,3);
	
	if (cheats == 1) {
		printf("on ");
	} else {
		printf("off");
	}
	
	move_terminal_cursor(1,1);
}

void toggle_pause(void) {
	paused ^= 1;
	
	move_terminal_cursor(45, 4);
	
	if (paused == 1) {
		printf("Paused");
	} else {
		printf("      ");
	}
	
	move_terminal_cursor(1,1);
}

uint8_t get_paused(void) {
	return paused;
}

void toggle_field_of_view(void) {
	field_of_view ^= 1;
	
	if (field_of_view == 1) {
		// set all cells to be hidden
		for (uint8_t x = 0; x < WIDTH; x++) {
			for (uint8_t y = 0; y < HEIGHT; y++) {
				update_square_colour(x, y, UNDISCOVERED);
			}
		}
		
		render_close();
	} else {
		for (uint8_t x = 0; x < WIDTH; x++) {
			for (uint8_t y = 0; y < HEIGHT; y++) {
				if (visible[x][y] == 1) {
					update_square_colour(x, y, get_object_at(x, y));
				}
			}
		}
	}
	
	// make the player and facing square visible
	update_square_colour(player_x, player_y, PLAYER);
	update_square_colour(facing_x, facing_y, FACING);	// not sure if i need these. safety
}

void fov_update(void) {
	if (field_of_view == 1) {
		// set all cells to be hidden
		int left_bound = player_x - 3;
		int right_bound = player_x + 3;
		int bottom_bound = player_y - 3;
		int top_bound = player_y + 3;
		
		for (int x = left_bound; x <= right_bound; x++) {
			for (int y = bottom_bound; y <= top_bound; y++) {
				if (in_bounds(x, y)) {
					if (!((x == left_bound || x == right_bound) && (y == bottom_bound || y == top_bound))) {	// if not corners
						if (visible[x][y] == 1) {
							update_square_colour(x, y, UNDISCOVERED);
						}
					}
				}
			}
		}
		
		render_close();
	}
}

void render_close(void) {
	int left_bound = player_x - 2;
	int right_bound = player_x + 2;
	int bottom_bound = player_y - 2;
	int top_bound = player_y + 2;
	
	for (int x = left_bound; x <= right_bound; x++) {
		for (int y = bottom_bound; y <= top_bound; y++) {
			if (in_bounds(x, y)) {
				if (!((x == left_bound || x == right_bound) && (y == bottom_bound || y == top_bound))) {	// if not corners
					if (visible[x][y] == 1) {
						update_square_colour(x, y, get_object_at(x, y));
					}
				}
			}
		}
	}
}

uint8_t distance_to_diamond(void) {
	uint8_t min_distance = 99;
	uint8_t man_dis = 0;
	for (int x = 0; x < WIDTH; x++) {
		for (int y = 0; y < HEIGHT; y++) {
			uint8_t object = get_object_at(x, y);
			if (object == DIAMOND) {
				man_dis = abs(player_x - x) + abs(player_y - y);
				
				if (man_dis < min_distance) {
					min_distance = man_dis;
				}
			}
		}
	}
	
	return min_distance;
}

void place_bomb(void) {
	bomb_x = player_x;
	bomb_y = player_y;
		
	playing_field[bomb_x][bomb_y] = BOMB;
	if (!(player_x == bomb_x && player_y == bomb_y)) {
		update_square_colour(bomb_x, bomb_y, get_object_at(bomb_x, bomb_y));
	}
	
	bomb_graphics_enabled = 1;
}

void detonate_bomb(void) {
	uint8_t new_x;
	uint8_t new_y;
	
	uint8_t moves[5][2] = {{0,0}, {0,-1}, {-1,0}, {1, 0}, {0, 1}};
	
	uint8_t new_object;
	
	for (uint8_t i = 0; i < 5; i++) {
		new_x = bomb_x + moves[i][0];
		new_y = bomb_y + moves[i][1];
		new_object = playing_field[new_x][new_y];
		if (new_x == player_x && new_y == player_y) {	// handle player next to bomb
			game_over = 1;
			handle_game_over();
			new_game();
		} else if (new_object == BREAKABLE || new_object == BOMB || new_object == DIS_BREAK) {
			playing_field[new_x][new_y] = EMPTY_SQUARE;
			discoverable_dfs(new_x, new_y);
		}
	}
	
	update_square_colour(bomb_x, bomb_y, get_object_at(bomb_x, bomb_y));
	
	
}

void flash_bomb(void) {
	uint8_t bomb;
	if (bomb_x == 17 && bomb_y == 17) {
		return;
	}
	
	bomb_view ^= 1;
	
	if (bomb_view == 0) {
		bomb = EMPTY_SQUARE;
	} else {
		bomb = BOMB;
	}
	
	update_square_colour(bomb_x, bomb_y, bomb);
}


void bomb_graphics(void) {
	uint8_t new_x;
	uint8_t new_y;
	uint8_t moves[5][2] = {{0,0}, {0,-1}, {-1,0}, {1, 0}, {0, 1}};
	
	if (bomb_graphics_enabled == 0) {
		for (uint8_t i = 0; i < 5; i++) {
			new_x = bomb_x + moves[i][0];
			new_y = bomb_y + moves[i][1];
			
			update_square_colour(new_x, new_y, get_object_at(new_x, new_y));
		}
		
		bomb_x = 17;
		bomb_y = 17;
	} else {
		for (uint8_t i = 0; i < 5; i++) {
			new_x = bomb_x + moves[i][0];
			new_y = bomb_y + moves[i][1];
			
			update_square_colour(new_x, new_y, BOMB);
		}
		
		bomb_graphics_enabled = 0;
	}
}

void check_danger(void) {
	if (bomb_x == 17 && bomb_y == 17) {
		PORTD |= (0 << 4);
	}
	
	if ((abs(player_x - bomb_x) <= 1 && abs(player_y - bomb_y) == 0) || (abs(player_x - bomb_x) == 0 && abs(player_y - bomb_y) <= 1)) {
		PORTD |= (1 << 4);
	} else {
		PORTD |= (0 << 4);
	}
}

void display_digit(uint8_t number, uint8_t digit) {
	PORTD = (digit << 2);	// ? test
	if (!(digit == 1 && number == 0)) {
		PORTC = seven_seg[number];	// We assume digit is in range 0 to 9
	} else {
		PORTC = 0;
	}
}