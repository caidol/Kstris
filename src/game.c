#include "game.h"

// Define playfield macros 

#define EXCLUDE_BORDER_WIDTH 20
#define EXCLUDE_BORDER_HEIGHT 10
#define PLAYFIELD_SIZE (PLAYFIELD_WIDTH * PLAYFIELD_WIDTH)

// TODO: Add challenge to the game once it is completed by decreasing the "frames per level"
// to speed up the game.

// Static struct for managing the "state" of the game, e.g ongoing processes 
//	-> static ensures that only one instance can exist during execution
static struct{
    u8 PLAYFIELD[EXCLUDE_BORDER_HEIGHT][EXCLUDE_BORDER_WIDTH];

    u32 score, lines;
    bool menu, pause, stopped, destroy, game_over;

    struct {
	const Tetromino *next; 
	const Tetromino *current;
    } ttm_queue;

    const Tetromino *hold;

    //TODO: Add controls

} game_process;

void init_playfield(){
    // Initialise every value within the playfield to 0
    
    for(int row = 0; row < EXCLUDE_BORDER_HEIGHT; row++){
	for(int col = 0; col < EXCLUDE_BORDER_WIDTH; col++){
	    game_process.PLAYFIELD[row][col] = 0;
	}
    }
}

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

    for(int i = 0; i < 4; i++){
	u8 _x = tetromino_coordinate_queue[i * 2];
	u8 _y = tetromino_coordinate_queue[i * 2 + 1];

	draw_block(renderer, _x, _y, tetromino.shape.colour);
    }

    free(tetromino_coordinate_queue);
    return true;
}

u8 find_tallest_row(u8 search_columns[], u8 original_rows[], u8 array_length, int *original_row, u8 x){
    int position;
    u8 selected_row = PLAYFIELD_HEIGHT - 2;

    // loop through columns and find the highest block
    for(position = 0; position < array_length; position++){
	u8 col = search_columns[position];
    
	// loop through every row on the specified columns
	// to check for block in lowest row position
	for(u8 row = 0; row < PLAYFIELD_HEIGHT-2; row++){
	    if(game_process.PLAYFIELD[row][col] == 1){
		if(row <= selected_row){
		    selected_row = row;
		    *original_row = original_rows[position];
		}
	    }
	}
    }

    // This occurs if no tetrominos have currently been placed on the board.
    // -> find the lowest row in the original rows and set that as the original
    //	  row
    if(selected_row == PLAYFIELD_HEIGHT - 2 && *original_row == -1){ // a value of -1 shows that the original_row has not been updated
	*original_row = original_rows[0];

	for(position = 0; position < array_length; position++){
	    if (original_rows[position] > *original_row){
		*original_row = original_rows[position];
	    } 
	}
    }

    return selected_row;
}

bool render_ghost_tetromino(Tetromino_state tetromino, u8 x, u8 y){
    u8 outline_colour = tetromino.shape.colour;
    u8 row = 0, col = 0;

    u16 current_rotation = tetromino.shape.rotations[tetromino.rotation_id];
    u16 bit;

    // TODO: possibly use dynamic memory allocation for the below
    u8 search_columns[4];
    u8 selected_rows[4];

    int i = 0;
    for (bit = 0x8000; bit > 0; bit = bit >> 1){
	if (current_rotation & bit){ // occurence of a tetromino block
	    bool no_blocks_beneath = true;
	    u8 rows_to_search = 4;

	    while (rows_to_search --> (row + 1)){
		u16 temp_bit = 0x8000 >> ((rows_to_search * 4) + (col + 1));

		if (current_rotation & temp_bit){ // occurence of block below row but same column
		    no_blocks_beneath = false; 
		}
	    }

	    if(no_blocks_beneath){
		search_columns[i] = x + col;
		selected_rows[i] = y + row;
		i++;
	    }
	}

	col++;
	if(col == 4){
	    row++;
	    col = 0;
	}
    }

    //TODO: Figure out a more efficient way to work out row difference later

    int original_row = -1; // Use this later on to calculate the row offset
    u8 tallest_row = find_tallest_row(search_columns, selected_rows, i, &original_row, x);
    u8 row_difference = tallest_row - original_row;

    // Render an outline of the same colour as the block and define its position
   
    col = 0, row = 0;
    for(bit = 0x8000; bit > 0; bit = bit >> 1){
	if(current_rotation & bit){
	    draw_outline(renderer, x + col, y + (row_difference) + row, tetromino.shape.colour);
	}
	col++;
	if(col == 4){
	    row++;
	    col = 0;
	}
    }
    SDL_RenderPresent(renderer);

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

/*
This function will retrieve the next tetromino from the render queue

-> updates the current tetromino and next tetromino buffer
-> a release of a held tetromino will override queue
*/
bool spawn_tetromino(Tetromino_state tetromino){
    // use current time as a seed 
    srand(time(0));

    if(game_process.ttm_queue.next == NULL){
	game_process.ttm_queue.next = (const Tetromino*) &TETROMINOS[rand() % NUM_TETROMINOS];
    } 

    game_process.ttm_queue.current = game_process.ttm_queue.next;
    game_process.ttm_queue.next = (const Tetromino*) &TETROMINOS[rand() % NUM_TETROMINOS];

    // Initialise the Tetromino_state struct with our selected Tetromino from queue

    Tetromino_state current_tetromino = {
	(Tetromino) {game_process.ttm_queue.current->colour, game_process.ttm_queue.current->rotations[0]}, 
	0,
	(u8) PLAYFIELD_WIDTH / 2, (u8) PLAYFIELD_HEIGHT / 2,
    };

    return true;
}
