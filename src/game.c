#include "game.h"
#include "graphics.c"

#define NUM_TETROMINOS 7
static const Tetromino TETROMINOS[NUM_TETROMINOS] = {
    {
	// I (line)	
	.colour = GRAY,
	.rotations = {
	    0x00F0,
	    0x2222,
	    0x0F00,
	    0x4444,
	}
    },
    {
	// L (right L)
	.colour = BLUE, 
	.rotations = {
	    0x2E00,
	    0x4460,
	    0x0E80,
	    0xC440
	}
    },       
    {
	// J (left L)
	.colour = ORANGE, 
	.rotations = {
	    0x8E00, 
	    0x6440,
	    0x0E20, 
	    0x44C0,
	}
    },
    {
	// S (right skew)
	.colour = GREEN,
	.rotations = {
	    0x6C00, 
	    0x4640,
	    0x06C0, 
	    0x8C40,
	}
    }, 
    {
	// Z (left skew) 
	.colour = RED, 
	.rotations = {
	    0xC600, 
	    0x2640, 
	    0x0C60, 
	    0x4C80,
	}
    }, 
    {
	// T 
	.colour = MAGENTA, 
	.rotations = {
	    0x4E00,
	    0x4640, 
	    0x0E40, 
	    0x4C40,
	}
    },
    {
	// O (cube)
	.colour = YELLOW, 
	.rotations = {
	    0x0660, 
	    0x0660, 
	    0x0660, 
	    0x0660, 
	}
    }
};

// Test for now 
const Tetromino *tetromino_O = &TETROMINOS[NUM_TETROMINOS - 1];
