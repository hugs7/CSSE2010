/*
 * game.c
 *
 * Contains functions relating to the play of Diamond Miners
 *
 * Author: Luke Kamols
 */ 

#include "game.h"
#include "display.h"

#define PLAYER_START_X  0;
#define PLAYER_START_Y  0;
#define FACING_START_X  1;
#define FACING_START_Y  0;

// the initial game layout
// the values 0, 3, 4 and 5 are defined in display.h
// note that this is not laid out in such a way that starting_layout[x][y]
// does not correspond to an (x,y) coordinate but is a better visual
// representation
static const uint8_t starting_layout[HEIGHT][WIDTH] = 
		{
			{0, 3, 0, 3, 0, 0, 0, 4, 4, 0, 0, 4, 0, 4, 0, 4},
			{0, 4, 0, 4, 0, 0, 0, 3, 4, 4, 3, 4, 0, 3, 0, 4},
			{0, 4, 0, 4, 4, 4, 4, 0, 3, 0, 0, 0, 0, 4, 0, 4},
			{5, 4, 0, 4, 0, 0, 3, 0, 0, 4, 0, 0, 0, 4, 0, 0},
			{4, 4, 3, 4, 5, 0, 4, 0, 0, 4, 3, 4, 0, 0, 4, 4},
			{0, 0, 0, 4, 4, 4, 4, 0, 4, 0, 0, 0, 4, 3, 0, 4},
			{0, 0, 0, 3, 0, 0, 3, 0, 3, 0, 3, 0, 3, 0, 0, 4},
			{0, 0, 0, 4, 0, 0, 3, 0, 4, 0, 0, 3, 3, 0, 5, 4} 
		};
		
#define NUM_DIRECTIONS 8
static const uint8_t directions[NUM_DIRECTIONS][2] = { {0,1}, {0,-1}, {1,0}, {-1,0}};

// variables for the current state of the game
uint8_t playing_field[WIDTH][HEIGHT]; // what is currently located at each square
uint8_t visible[WIDTH][HEIGHT]; // whether each square is currently visible
uint8_t player_x;
uint8_t player_y;
uint8_t facing_x;
uint8_t facing_y;
uint8_t facing_visible;

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
	
	// go through and initialise the state of the playing_field
	for (int x = 0; x < WIDTH; x++) {
		for (int y = 0; y < HEIGHT; y++) {
			// initialise this square based on the starting layout
			// the indices here are to ensure the starting layout
			// could be easily visualised when declared
			playing_field[x][y] = starting_layout[HEIGHT - 1 - y][x];
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
	if (!in_bounds(x,y)) {
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

// check the header file game.h for a description of what this function should do
// it contains a few extra hints
void move_player(uint8_t dx, uint8_t dy) {
	// YOUR CODE HERE
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
	return 0;
}

/*
 * given an (x,y) coordinate, perform a depth first search to make any
 * squares reachable from here visible. If a wall is broken at a position
 * (x,y), this function should be called with coordinates (x,y)
 * YOU SHOULD NOT NEED TO MODIFY THIS FUNCTION
 */
void discoverable_dfs(uint8_t x, uint8_t y) {
	uint8_t x_adj, y_adj, object_here;
	// set the current square to be visible and update display
	visible[x][y] = 1;
	object_here = get_object_at(x, y);
	update_square_colour(x, y, object_here);
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