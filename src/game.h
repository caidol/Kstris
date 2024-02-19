#include "defs.h"
#include "input.h"
#include "graphics.h"

#ifndef _GAME_CONSTANTS_
#define _GAME_CONSTANTS_
// an empty block will be the same as the background colour
// -> black
#define EMPTY 8
#define EMPTY_HEX 0x000000FF

typedef enum {
    CYAN_HEX =		0x00FFFFFF, 
    BLUE_HEX =		0x0000FFFF, 
    ORANGE_HEX =	0xFF8D00FF, 
    GREEN_HEX =		0x00FF00FF, 
    RED_HEX =		0xFF0000FF, 
    MAGENTA_HEX =	0xFF00FFFF, 
    YELLOW_HEX =	0xFFFF00FF,
    GRAY_HEX =		0x808080FF, 
} Hex_colours;

// Tetromino struct which defines its color and rotation schemes 
//	--> per tetromino type

typedef struct{
    // Follow standard RGBA convention 0xRRGGBBAA
    u8 colour_id;

    // The following array contains the rotation scheme 
    // for each of the four max possible rotations. 
    //
    // The rotation system being used is the Nintendo rotation system,
    // where each element a 16 bit hex form of a 4x4 matrix for the rotation 
    
    u16 rotations[4];

} Tetromino;

#define NUM_TETROMINOS 7
static const Tetromino TETROMINOS[NUM_TETROMINOS] = {
    {
	// I (line)	
	.colour_id = CYAN, 

	.rotations = {
	    0x0F00,
	    0x2222,
	    0x00F0,
	    0x4444,
	}
    },
    {
	// L (right L)
	.colour_id = BLUE, 

	.rotations = {
	    0x2E00,
	    0x4460,
	    0x0E80,
	    0xC440
	}
    },       
    {
	// J (left L)
	.colour_id = ORANGE, 

	.rotations = {
	    0x8E00, 
	    0x6440,
	    0x0E20, 
	    0x44C0,
	}
    },
    {
	// S (right skew)
	.colour_id = GREEN, 

	.rotations = {
	    0x6C00, 
	    0x4620,
	    0x06C0, 
	    0x8C40,
	}
    }, 
    {
	// Z (left skew) 
	.colour_id = RED, 

	.rotations = {
	    0xC600, 
	    0x2640, 
	    0x0C60, 
	    0x4C80,
	}
    }, 
    {
	// T 
	.colour_id = MAGENTA, 

	.rotations = {
	    0x4E00,
	    0x4640, 
	    0x0E40, 
	    0x4C40,
	}
    },
    {
	// O (cube)
	.colour_id = YELLOW, 

	.rotations = {
	    0xCC00, 
	    0xCC00, 
	    0xCC00, 
	    0xCC00, 
	}
    }
};


// Tetromino struct which defines it's blueprint 
//	--> per type of tetromino

typedef struct {
    // tetromino shape to inherit
    Tetromino shape;

    // used to select correct rotation from Tetromino.rotations array
    u8 rotation_id; 
 
    i8 x;
    i8 y;

} Tetromino_state;

static u8 lock_delay_count = 0;
const static u8 lock_delay_limit = 2;

static u8 previous_coords[8]        =	{0, 0, 0, 0, 0, 0, 0, 0};
static u8 previous_ghost_coords[8]  =	{0, 0, 0, 0, 0, 0, 0, 0};
static u8 previous_next_coords[8]   =	{0, 0, 0, 0, 0, 0, 0, 0};
static u8 previous_hold_coords[8]   =	{0, 0, 0, 0, 0, 0, 0, 0};

static bool AUTODROP_EXPIRED = false;
static bool SOFTDROP_EXPIRED = false;

static Tetromino_state CURRENT_TETROMINO;
static Tetromino_state GHOST_TETROMINO;

extern SDL_Renderer *renderer;
extern Tetromino_Actions TETROMINO_ACTION;

void init_tetris();

void draw_playfield();

void draw_playfield_border(SDL_Renderer *renderer);

bool render_showcase_tetromino(Tetromino_state tetromino, u8 previous_coords[]);

bool valid_render_tetromino(Tetromino_state tetromino, u8 *tetromino_coordinate_queue);

bool render_tetromino(Tetromino_state tetromino, u8 previous_coords[]);

bool render_ghost_tetromino(Tetromino_state tetromino, u8 previous_ghost_coords[]);

bool render_current_tetromino(Tetromino_state tetromino, Tetromino_state ghost_tetromino);

bool spawn_tetromino();

void render_next_showcase();

void render_hold_showcase();

void render_showcase_information(SDL_Surface* scoreSurface);

void update_game();

u8 get_block_colour(u8 _x, u8 _y);

void set_playfield_block(u8 _x, u8 _y, u8 colour);

#endif
