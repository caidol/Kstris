#include "defs.h"
#include "main.h"
#include "graphics.h"

// an empty block will be the same as the background colour
// -> black
#define EMPTY 8

// Tetromino struct which defines its color and rotation schemes 
//	--> per tetromino type

typedef struct{
    // Follow standard RGBA convention 0xRRGGBBAA
    Hex_colours colour;

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
	.colour = CYAN_HEX,
	.rotations = {
	    0xF000,
	    0x2222,
	    0x0F00,
	    0x4444,
	}
    },
    {
	// L (right L)
	.colour = BLUE_HEX, 
	.rotations = {
	    0x2E00,
	    0x4460,
	    0x0E80,
	    0xC440
	}
    },       
    {
	// J (left L)
	.colour = ORANGE_HEX, 
	.rotations = {
	    0x8E00, 
	    0x6440,
	    0x0E20, 
	    0x44C0,
	}
    },
    {
	// S (right skew)
	.colour = GREEN_HEX,
	.rotations = {
	    0x6C00, 
	    0x4620,
	    0x06C0, 
	    0x8C40,
	}
    }, 
    {
	// Z (left skew) 
	.colour = RED_HEX, 
	.rotations = {
	    0xC600, 
	    0x2640, 
	    0x0C60, 
	    0x4C80,
	}
    }, 
    {
	// T 
	.colour = MAGENTA_HEX, 
	.rotations = {
	    0x4E00,
	    0x4640, 
	    0x0E40, 
	    0x4C40,
	}
    },
    {
	// O (cube)
	.colour = YELLOW_HEX, 
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
 
    u8 x;
    u8 y;

} Tetromino_state;

extern SDL_Renderer *renderer;

bool valid_render_tetromino(Tetromino_state tetromino, u8 *tetromino_coordinate_queue, u8 *array_size);
bool render_ghost_tetromino(Tetromino_state tetromino, u8 x, u8 y);
bool spawn_tetromino();
