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
    SDL_TimerID autodrop_timer;
} game_process;

void init_tetris(){
    // set up SDL timer  
    if (game_process.autodrop_timer != 0){
	printf("REMOVING TIMER\n");
	SDL_RemoveTimer(game_process.autodrop_timer);
    }
    game_process.autodrop_timer = 0;

    spawn_tetromino();
    SDL_RenderPresent(renderer);
}

void init_playfield(){
    // Initialise every value within the playfield to 0
    
    for(int row = 0; row < EXCLUDE_BORDER_HEIGHT; row++){
	for(int col = 0; col < EXCLUDE_BORDER_WIDTH; col++){
	    game_process.PLAYFIELD[row][col] = 0;
	}
    }
}

bool valid_render_tetromino(Tetromino_state tetromino, u8 *tetromino_coordinate_queue, u8 *array_size){ 
    bool can_render_tetromino = true;
    u8 row = 0, col = 0;

    u8 x = tetromino.x;
    u8 y = tetromino.y;

    // access correct rotation matrix data
    u16 bit, piece;
    piece = tetromino.shape.rotations[tetromino.rotation_id]; 

    *array_size = 2;    

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
    
		if (*array_size < queue_limit && tetromino_coordinate_queue != NULL){
		    tetromino_coordinate_queue = (u8*) realloc(
			tetromino_coordinate_queue, 
			queue_limit * sizeof(u8)
		    );

		    *array_size += 2;
		}	

		// update queue 
		if (tetromino_coordinate_queue != NULL){
		    tetromino_coordinate_queue[block_limit * 2] = _x; 
		    tetromino_coordinate_queue[block_limit * 2 + 1] = _y;
		}

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

bool render_ghost_tetromino(Tetromino_state tetromino){
    u8 x = tetromino.x, y = tetromino.y;
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
	    draw_outline(renderer, x + col, y + (row_difference) + row, tetromino.shape.colour_id);
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

// TODO: Maybe think of another workaround for this solution 

// This function will quickly run the valid_render_tetromino() function
// to ensure whether it is capable of being run
bool check_tetromino(Tetromino_state tetromino, u8 *coordinate_queue){
    u8 size = 0;
    u8 *array_size = &size;

    if(!valid_render_tetromino(tetromino, coordinate_queue, array_size)){
	return false;
    }

    return true;
}

bool render_tetromino(Tetromino_state tetromino, u8 previous_coords[]){ //, u8 previous_coords[]){
    // TODO: Check the previous tetromino spawn as well to ensure that no tetromino type 
    //       is spawned more than once.

    u8 *tetromino_coordinate_queue = NULL;

    if (tetromino_coordinate_queue == NULL){
	tetromino_coordinate_queue = (u8*) malloc(2 * sizeof(u8)); // TODO: Check for a null pointer
    }
    
    if (!check_tetromino(tetromino, tetromino_coordinate_queue)){
	return false;
    }

    // If previous coordinates are available, then clear them 
    
    for(int i = 0; i < sizeof(&previous_coords) / sizeof(u8); i++){
	u8 _x = previous_coords[i * 2];
	u8 _y = previous_coords[i * 2 + 1];

	// TODO: WORK ON CHECKING IF ANY COORDINATES AVAILABLE BEFORE RENDERING
	draw_block(renderer, _x, _y, EMPTY); 
    }

    // render the newly updated block coordinates 
    
    for(int i = 0; i < 4; i++){
	u8 _x = tetromino_coordinate_queue[i * 2];
	u8 _y = tetromino_coordinate_queue[i * 2 + 1];
	
	draw_block(renderer, _x, _y, tetromino.shape.colour_id);

	// store the new coordinates
	previous_coords[i * 2] = _x;
	previous_coords[i * 2 + 1] = _y;
    }

    free(tetromino_coordinate_queue);
    return true;
}

bool render_current_tetromino(Tetromino_state tetromino){

    if(render_tetromino(tetromino, previous_coords)
       && render_ghost_tetromino(tetromino)){
	CURRENT_TETROMINO = tetromino;

	return true;
    }

    return false;
}

/*
This function will retrieve the next tetromino from the render queue

-> updates the current tetromino and next tetromino buffer
-> a release of a held tetromino will override queue
*/
bool spawn_tetromino(){
    printf("HELLO\n");
    // use current time as a seed 
    srand(time(0));

    if(game_process.ttm_queue.next == NULL){
	game_process.ttm_queue.next = (const Tetromino*) &TETROMINOS[rand() % NUM_TETROMINOS];
    } 

    game_process.ttm_queue.current = game_process.ttm_queue.next; 
    
    // Initialise the Tetromino_state struct with our selected Tetromino from queue
    Tetromino_state current_tetromino = {
	(Tetromino) {
	    game_process.ttm_queue.current->colour, 
	    game_process.ttm_queue.current->colour_id,

	    {
		game_process.ttm_queue.current->rotations[0], 
		game_process.ttm_queue.current->rotations[1],  
		game_process.ttm_queue.current->rotations[2], 
		game_process.ttm_queue.current->rotations[3]
	    }
	}, 
	0,
	(u8) (PLAYFIELD_WIDTH / 2) - 2, (u8) 1,
    };

    CURRENT_TETROMINO = current_tetromino;

    //printf("col: %d\n", current_tetromino.x);
    //printf("row: %d\n", current_tetromino.y);
    //printf("colour: %d\n", current_tetromino.shape.colour);
    //printf("colour id: %d\n", current_tetromino.shape.colour_id);
    //printf("rotation id: %d\n", current_tetromino.rotation_id);

    // render in the current tetromino 
    
    //valid_render_tetromino(current_tetromino, current_tetromino.x, current_tetromino.y, previous_coords);
    render_tetromino(current_tetromino, previous_coords);
    render_ghost_tetromino(current_tetromino);

    game_process.ttm_queue.next = (const Tetromino*) &TETROMINOS[rand() % NUM_TETROMINOS];
    SDL_RenderPresent(renderer);
    return true;
}

u32 autodrop_callback(u32 interval, void *param){
    printf("CALLBACK CALLED\n");

    SDL_Event event;
    SDL_UserEvent user_event;

    user_event.type = SDL_USEREVENT;
    user_event.code = 0;
    user_event.data1 = NULL;
    user_event.data2 = NULL;

    event.type = SDL_USEREVENT;
    event.user = user_event;

    // Add the SDL event pointer to the queue
    SDL_PushEvent(&event);

    printf("interval: %d\n", interval);
    return interval;
}

void update_game(){
    // TODO:
    //
    // - Add an auto drop timer and move the entire tetromino down a place
    // - Render the current tetromino and score on the sides 
    // - Render the held tetromino -> potentially for another function
    // - Switch between different Tetromino actions to render the tetromino a certain way

    if (game_process.autodrop_timer == 0){
	printf("ADDING TIMER\n");
	game_process.autodrop_timer = SDL_AddTimer(2000, autodrop_callback, NULL);
    }

    printf("timer: %d\n", game_process.autodrop_timer);

    Tetromino_state tetromino = CURRENT_TETROMINO;

    //printf("col: %d\n", tetromino.x);
    //printf("row: %d\n", tetromino.y);
    //printf("colour: %d\n", tetromino.shape.colour);
    //printf("colour id: %d\n", tetromino.shape.colour_id);
    //printf("rotation id: %d\n", tetromino.rotation_id);

    // action from the keyboard 

    switch(TETROMINO_ACTION){
	case NONE: 
	    printf("NONE\n");
	    break; 

	case MOVE_LEFT:
	    printf("MOVE LEFT\n");
	    printf("TETROMINO ACTION: %d\n", TETROMINO_ACTION); 
	    tetromino.x -= 1;

	    printf("x: %d\n", tetromino.x);
	    render_current_tetromino(tetromino);

	    break;

	case MOVE_RIGHT:
	    printf("MOVE RIGHT\n");
	    printf("TETROMINO ACTION: %d\n", TETROMINO_ACTION);	
	    tetromino.x += 1;
	    
	    printf("x: %d\n", tetromino.x);
	    render_current_tetromino(tetromino);


	    break; 

	case ROTATE:
	    printf("ROTATE\n");
	    printf("TETROMINO ACTION: %d\n", TETROMINO_ACTION);
	    tetromino.rotation_id = (tetromino.rotation_id + 1) % 4;

	    render_current_tetromino(tetromino);

	    break;

	case SOFT_DROP:
	    printf("SOFT DROP\n");
	    printf("TETROMINO ACTION: %d\n", TETROMINO_ACTION);
	    tetromino.y += 1;
	     u8 *soft_drop_queue = NULL;

	    while(check_tetromino(tetromino, soft_drop_queue)) {
		tetromino.y += 1;
	    }   

	    render_current_tetromino(tetromino);


	    // lock tetromino 

	case HARD_DROP:
	    printf("HARD DROP\n");
	    printf("TETROMINO ACTION: %d\n", TETROMINO_ACTION);
	    tetromino.y += 1;
	    u8 *hard_drop_queue = NULL;

	    while(check_tetromino(tetromino, hard_drop_queue)){
		tetromino.y += 1;
	    }
	    
	    render_current_tetromino(tetromino);

	    // lock tetromino

	    break;

	case AUTO_DROP:
	    printf("AUTO DROP\n");
	    printf("TETROMINO ACTION: %d\n", TETROMINO_ACTION);
	    // Write a function to deal with tetrominos that are "locked"
	    // at the bottom of the board
	    tetromino.y += 1;
	    printf("y: %d\n", tetromino.y);

	    u8 *autodrop_queue = NULL;
	    
	    render_current_tetromino(tetromino);


	    if (check_tetromino(tetromino, autodrop_queue)){
		lock_delay_count++;
	    }
	    else{
		lock_delay_count = 0;
	    }

	    break;
    }

    TETROMINO_ACTION = NONE;
}
