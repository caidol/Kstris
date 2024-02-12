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
    //u8 PLAYFIELD[EXCLUDE_BORDER_HEIGHT][EXCLUDE_BORDER_WIDTH];
    Hex_colours board[EXCLUDE_BORDER_HEIGHT][EXCLUDE_BORDER_WIDTH];

    u32 score, lines;

    struct {
	const Tetromino *next; 
	const Tetromino *current;
    } ttm_queue;

    const Tetromino *hold;
    
    SDL_TimerID autodrop_timer;
    SDL_TimerID softdrop_timer;
} game_process;

void init_playfield(){
    // Initialise every value within the playfield to AN EMPTY BLOCK
     
    for(int row = 0; row < EXCLUDE_BORDER_HEIGHT; row++){
	for(int col = 0; col < EXCLUDE_BORDER_WIDTH; col++){
	    //game_process.PLAYFIELD[row][col] = 0;
	    game_process.board[row][col] = EMPTY_HEX;
	}
    }
}

void init_tetris(){
    // set up SDL timer  
    if (game_process.autodrop_timer != 0){
	printf("REMOVING TIMER\n");
	SDL_RemoveTimer(game_process.autodrop_timer);
    }
    
    if (game_process.softdrop_timer != 0){
	SDL_RemoveTimer(game_process.softdrop_timer);
    }

    game_process.autodrop_timer = 0;
    game_process.softdrop_timer = 0;

    init_playfield();
    draw_playfield(renderer);
    spawn_tetromino();
}

bool valid_render_tetromino(Tetromino_state tetromino, u8 *tetromino_coordinate_queue, u8 *array_size){ 
    bool can_render_tetromino = true;
    u8 row = 0, col = 0;

    i8 x = tetromino.x;
    i8 y = tetromino.y;

    // access correct rotation matrix data
    u16 bit, piece;
    piece = tetromino.shape.rotations[tetromino.rotation_id]; 

    *array_size = 2;    

    if(tetromino.x == 0){
	tetromino.x += 1;
    }

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
    
    for(int i = 0; i < 4; i++){
	u8 _x = previous_coords[i * 2];
	u8 _y = previous_coords[i * 2 + 1];

	//game_process.PLAYFIELD[_x][_y] = 0;

	// TODO: WORK ON CHECKING IF ANY COORDINATES AVAILABLE BEFORE RENDERING
	//printf("previous: %d\n", previous_coords[0]);
	if (previous_coords[0] != 0){
	    draw_block(renderer, _x, _y, EMPTY); 
	}
    }

    // render the newly updated block coordinates 
    
    for(int i = 0; i < 4; i++){
	u8 _x = tetromino_coordinate_queue[i * 2];
	u8 _y = tetromino_coordinate_queue[i * 2 + 1];
	
	//game_process.PLAYFIELD[_x][_y] = 1;
	game_process.board[_x][_y] = tetromino.shape.colour;

	//board[_x * PLAYFIELD_WIDTH + _y] = tetromino.shape.colour;

	draw_block(renderer, _x, _y, tetromino.shape.colour_id);

	// store the previous coordinates
	previous_coords[i * 2] = _x;
	previous_coords[i * 2 + 1] = _y;
    }

    free(tetromino_coordinate_queue);
    return true;
}

bool render_ghost_tetromino(Tetromino_state ghost_tetromino, u8 previous_ghost_coords[]){
    u8 *tetromino_coordinate_queue = NULL;

    if (tetromino_coordinate_queue == NULL){
	tetromino_coordinate_queue = (u8*) malloc(2 * sizeof(u8)); // TODO: Check for a null pointer
    }
    
    if (!check_tetromino(ghost_tetromino, tetromino_coordinate_queue)){
	return false;
    } 

    i8 ghost_x = ghost_tetromino.x, ghost_y = ghost_tetromino.y;

    for(int i = 0; i < 4; i++){
	u8 _x = previous_ghost_coords[i * 2];
	u8 _y = previous_ghost_coords[i * 2 + 1];
    
	// TODO: WORK ON CHECKING IF ANY COORDINATES AVAILABLE BEFORE RENDERING
	if (previous_ghost_coords[0] != 0){
	    printf("CLEARING\n");
	    draw_block(renderer, _x, _y, EMPTY); 
	}
    } 

    // shift all the blocks down to the lowest possible row
    while(check_tetromino(ghost_tetromino, tetromino_coordinate_queue)){
	ghost_tetromino.y += 1;
	
	for(int i = 0; i < 4; i++){
	    u8 _y = tetromino_coordinate_queue[i * 2 + 1];

	    tetromino_coordinate_queue[i * 2 + 1] = _y + 1;
	}
    }

    // shift position up by one -> TODO: FIX PROBLEM THAT CAUSES THIS
    for(int i = 0; i < 4; i++){
	u8 _y = tetromino_coordinate_queue[i * 2 + 1];

	tetromino_coordinate_queue[i * 2 + 1] = _y - 1;
    }    

    for(int i = 0; i < 4; i++){
	u8 _x = tetromino_coordinate_queue[i * 2];
	u8 _y = tetromino_coordinate_queue[i * 2 + 1];
	
	//game_process.PLAYFIELD[_x][_y] = 1;

	//board[_x * PLAYFIELD_WIDTH + _y] = tetromino.shape.colour;

	draw_outline(renderer, _x, _y, ghost_tetromino.shape.colour_id);

	// store the previous coordinates
	previous_ghost_coords[i * 2] = _x;
	previous_ghost_coords[i * 2 + 1] = _y;
    }

    free(tetromino_coordinate_queue);
    return true;
}

bool render_current_tetromino(Tetromino_state tetromino, Tetromino_state ghost_tetromino){

    if(render_ghost_tetromino(ghost_tetromino, previous_ghost_coords)){
	GHOST_TETROMINO = ghost_tetromino;
    }

    if(render_tetromino(tetromino, previous_coords)){
       //&& render_ghost_tetromino(ghost_tetromino, previous_ghost_coords)){
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
    draw_playfield(renderer);
    // use current time as a seed 
    srand(time(0));

    if(game_process.ttm_queue.next == NULL){
	game_process.ttm_queue.next = (const Tetromino*) &TETROMINOS[rand() % NUM_TETROMINOS];
    } 

    game_process.ttm_queue.current = game_process.ttm_queue.next; 
    
    // Initialise the Tetromino_state structs with our selected Tetromino from queue
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
    
    // ghost tetromino equivalent
    Tetromino_state current_ghost_tetromino = {
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
    GHOST_TETROMINO = current_ghost_tetromino;

    //printf("col: %d\n", current_tetromino.x);
    //printf("row: %d\n", current_tetromino.y);
    //printf("colour: %d\n", current_tetromino.shape.colour);
    //printf("colour id: %d\n", current_tetromino.shape.colour_id);
    //printf("rotation id: %d\n", current_tetromino.rotation_id);

    // render in the current tetromino 
    
    //valid_render_tetromino(current_tetromino, current_tetromino.x, current_tetromino.y, previous_coords);
    //render_tetromino(current_tetromino, previous_coords);
    //render_ghost_tetromino(current_ghost_tetromino, previous_ghost_coords);

    game_process.ttm_queue.next = (const Tetromino*) &TETROMINOS[rand() % NUM_TETROMINOS];
    
    return true;
}

Tetromino_state wall_kick(Tetromino_state tetromino){
    // this is a simple variation of the wall kick algorithm. 
    // in official tetris games the surrounding vertices would be more rigorously tested
    
    // The I tetromino has different wall kick behaviours
    if(tetromino.shape.colour_id == CYAN){ 
	if(tetromino.x <= 0){
	    tetromino.x = 1;
	}
	else if (tetromino.x >= 8){
	    tetromino.x = 7;
	} 
	
	printf("x after: %d\n", tetromino.x);
	return tetromino;
    }
    
    if(tetromino.x == 0){ 
	tetromino.x = 1; 
    }
    else if((tetromino.x + 3) == PLAYFIELD_WIDTH 
	    && tetromino.shape.colour_id != YELLOW){ 
	tetromino.x -= 1; 
    }
    else if(tetromino.y == 0){ 
	tetromino.y += 1; 
    }

    return tetromino;
}

u32 autodrop_callback(u32 interval, void *param){
    //printf("CALLBACK 1 CALLED\n");

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

    AUTODROP_EXPIRED = true;
    return interval;
}

u32 softdrop_callback(u32 interval, void *param){
    //printf("CALLBACK 2 CALLED\n");

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

    SOFTDROP_EXPIRED = true;
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
	game_process.autodrop_timer = SDL_AddTimer(500, autodrop_callback, NULL);
    }

    if (game_process.softdrop_timer == 0){
	game_process.softdrop_timer = SDL_AddTimer(10, softdrop_callback, NULL);
    }

    //printf("timer: %d\n", game_process.autodrop_timer);

    Tetromino_state tetromino = CURRENT_TETROMINO;
    Tetromino_state ghost_tetromino = GHOST_TETROMINO;

    // action from the keyboard 

    switch(TETROMINO_ACTION){
	case NONE: 
	    //printf("NONE\n");
	    break; 

	case MOVE_LEFT:
	    printf("MOVE LEFT\n");
	    printf("TETROMINO ACTION: %d\n", TETROMINO_ACTION); 
	    
	    tetromino.x -= 1;
	    ghost_tetromino.x -= 1;

	    printf("x: %d\n", tetromino.x);
	    render_current_tetromino(tetromino, ghost_tetromino);

	    break;

	case MOVE_RIGHT:
	    printf("MOVE RIGHT\n");
	    printf("TETROMINO ACTION: %d\n", TETROMINO_ACTION);	
	    
	    tetromino.x += 1;
	    ghost_tetromino.x += 1;

	    printf("x: %d\n", tetromino.x);
	    render_current_tetromino(tetromino, ghost_tetromino);

	    break; 

	case ROTATE:
	    printf("ROTATE\n");
	    printf("TETROMINO ACTION: %d\n", TETROMINO_ACTION);

	    tetromino = wall_kick(tetromino);
	    tetromino.rotation_id = (tetromino.rotation_id + 1) % 4;

	    ghost_tetromino = wall_kick(ghost_tetromino);
	    ghost_tetromino.rotation_id = (ghost_tetromino.rotation_id + 1) % 4;

	    render_current_tetromino(tetromino, ghost_tetromino);

	    break;

	case SOFT_DROP:
	    printf("SOFT DROP\n");
	    printf("TETROMINO ACTION: %d\n", TETROMINO_ACTION);
	    
	    if (SOFTDROP_EXPIRED){
		tetromino.y += 1;
		SOFTDROP_EXPIRED = false;
	    }

	    //u8 *soft_drop_queue = NULL;

	    /*
	    while(check_tetromino(tetromino, soft_drop_queue)) {
		tetromino.y += 1;
	    } 
	    */	

	    render_current_tetromino(tetromino, ghost_tetromino); 

	    // lock tetromino 
	    
	    break;
	case HARD_DROP:
	    printf("HARD DROP\n");
	    printf("TETROMINO ACTION: %d\n", TETROMINO_ACTION);
	    tetromino.y += 1;

	    while(render_current_tetromino(tetromino, ghost_tetromino)){
		tetromino.y += 1;
	    }
	    
	    // lock tetromino

	    break;

	case AUTO_DROP:
	    printf("AUTO DROP\n");
	    printf("TETROMINO ACTION: %d\n", TETROMINO_ACTION);
	    // Write a function to deal with tetrominos that are "locked"
	    // at the bottom of the board
	    
	    if (AUTODROP_EXPIRED){
		tetromino.y += 1;
		AUTODROP_EXPIRED = false;
		printf("y: %d\n", tetromino.y);
	    }

	    u8 *autodrop_queue = NULL;

	    if (!render_current_tetromino(tetromino, ghost_tetromino)){
		lock_delay_count++;
	    }
	    else{
		lock_delay_count = 0;
	    }

	    break;
    } 

    TETROMINO_ACTION = NONE;
}
