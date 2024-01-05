#include "game.h"

// Test for now 
//const Tetromino *tetromino_O = &TETROMINOS[NUM_TETROMINOS - 1];

bool valid_render_tetromino(Tetromino_state tetromino, u8 x, u8 y, u8 *tetromino_coordinate_queue){ 
    bool can_render_tetromino = true;
    u8 row = 0, col = 0;

    // access correct rotation matrix data
    u16 bit, piece;
    piece = tetromino.shape.rotations[tetromino.rotation_id];

    if (tetromino_coordinate_queue == NULL){
	tetromino_coordinate_queue = (u8*) malloc(2 * sizeof(u8)); // TODO: Check for a null pointer
    }

    u8 array_size = 2;    

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
    for (bit = 0x8000; bit > 0 && block_limit < 4; bit = bit >> 1){

	if (piece & bit){ // check if a bit occurs
	    u8 _x = x + col; 
	    u8 _y = y + row;

	    // account for gray block border
	    if ((_x < 1) || (_x >= PLAYFIELD_WIDTH - 1)
		|| (_y < 1) || (_y >= PLAYFIELD_HEIGHT - 1)
		){
		// tetromino block falls out of bounds 
		return false;
		break;
	    } 
	    else {
		int queue_limit = 2 * (block_limit + 1);
    
		if (array_size < queue_limit){
		    tetromino_coordinate_queue = (u8*) realloc(
			tetromino_coordinate_queue, 
			queue_limit * sizeof(u8)
		    );

		    array_size += 2;
		}	

		// update queue 
		tetromino_coordinate_queue[block_limit * 2] = _x; 
		tetromino_coordinate_queue[block_limit * 2 + 1] = _y;
	    
		block_limit++;
	    }
	}

	// cycle between columns and rows 
	col++;
	if(col == 4){
	    row++;
	    col = 0;
	}
    }

    free(tetromino_coordinate_queue);
    return true;
}

bool render_ghost_tetromino(Tetromino_state tetromino, u8 x, u8 y){
    u8 outline_colour = tetromino.shape.colour;
    u8 row = 0, col = 0;

    u16 current_rotation = tetromino.shape.rotations[tetromino.rotation_id];
    u16 bit;

    // TODO: possibly use dynamic memory allocation for the below
    u8 search_columns[4];

    int i = 0;
    for (bit = 0x8000; bit > 0; bit = bit >> 1){
	if (current_rotation & bit){ // occurence of a tetromino block
	    bool no_blocks_beneath = true;
	    u8 rows_to_search = 3 - row;

	    while (rows_to_search --> 0){
		u16 temp_bit = 0x8000 >> ((rows_to_search * 4) + (col + 1));

		if (current_rotation & temp_bit){ // occurence of block below row but same column
                    no_blocks_beneath = false; 
		}
	    }

	    if(no_blocks_beneath){
		search_columns[i] = col;
		i++;
	    }
	}

	col++;
	if(col == 3){
	    row++;
	    col = 0;
	}
    }

    // TODO: Search downward through each column and find the tallest block from all available columns
    //		-> will require a board variable to manage positions of blocks

    return true;
}

bool render_tetromino(Tetromino_state tetromino, u8 previous_coords[]){
    u8 *tetromino_coordinate_queue = NULL;

    if(!valid_render_tetromino(tetromino, tetromino.x, tetromino.y, tetromino_coordinate_queue)){
	return false;
    }
    
    // If previous coordinates are available, then clear them 
    for(int i = 0; i < sizeof(&previous_coords) / sizeof(u8); i++){
	u8 _x = previous_coords[i * 2];
	u8 _y = previous_coords[i * 2 + 1];

	draw_block(renderer, _x, _y, EMPTY); 
    }

    // render the newly updated block coordinates 
    for(int i = 0; i < 4; i++){
	u8 _x = *(tetromino_coordinate_queue + i * 2);
	u8 _y = *(tetromino_coordinate_queue + i * 2 + 1);
	
	draw_block(renderer, _x, _y, tetromino.shape.colour);

	// store the new coordinates
	previous_coords[i * 2] = _x;
	previous_coords[i * 2 + 1] = _y;
    }

    return true;
}
