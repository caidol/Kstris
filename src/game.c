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

bool render_tetromino(Tetromino_state tetromino, u8 x, u8 y){
    bool can_render_tetromino = true;
    u8 row = 0, col = 0;

    // access correct rotation matrix data
    u16 bit, piece;
    piece = tetromino.shape.rotations[tetromino.rotation_id];

    u8 *tetromino_render_queue = (u8*) malloc(2 * sizeof(u8));

    /*
    below is an error check 
       -> we assume the parameters x and y are the top left corners 
	  of the rotation scheme

	  using bitwise operators, we cycle through determing the position 
	  of where a bit in the rotation schemes occur, all the while constantly
	  updating the column and row position (row-order matrix)

	  upon finding a bits position, we perform a simple logical boundary check 
	  to deem whether it falls within our playfield, then update our queue 
	  accordingly
    */

    u8 block_limit = 0; // in case all four tetromino blocks were found we can break out of the loop
    for (bit = 0x8000; bit > 0 && block_limit < 3; bit = bit >> 1){
	if (piece & bit){ // check if a bit occurs
	    u8 _x = x + col; 
	    u8 _y = y + row;

	    // account for gray block border
	    if ((_x < BLOCK_SIZE) || (_x >= PLAYFIELD_WIDTH - BLOCK_SIZE)
		|| (_y < BLOCK_SIZE) || (_y >= PLAYFIELD_HEIGHT - BLOCK_SIZE)
		){
		// tetromino block falls out of bounds 
		return false;
	    } 
	    else {
		block_limit++;
		int queue_limit = 2 * block_limit;

		if (sizeof(&tetromino_render_queue) < queue_limit){
		    tetromino_render_queue = (u8*) realloc(
			tetromino_render_queue, 
			queue_limit * sizeof(u8)
		    );
		}	
	    }
	}

	// cycle between columns and rows 
	col++;
	if(col == 3){
	    row++;
	}
    }


    free(tetromino_render_queue);
    return true;
}
