#include "game.h"
#include <stdio.h>

// Define playfield macros 

#define EXCLUDE_BORDER_WIDTH 10
#define EXCLUDE_BORDER_HEIGHT 20
#define PLAYFIELD_SIZE (PLAYFIELD_WIDTH * PLAYFIELD_WIDTH)

// TODO: Add challenge to the game once it is completed by decreasing the "frames per level"
// to speed up the game.

// Static struct for managing the "state" of the game, e.g ongoing processes 
//	-> static ensures that only one instance can exist during execution
static struct{
    //u8 PLAYFIELD[EXCLUDE_BORDER_HEIGHT][EXCLUDE_BORDER_WIDTH];
    u8 playfield[22][12];
    int score, lines, level;

    // TODO: Use these surfaces
    SDL_Surface 
	*nextSurface, *holdSurface, 
	*scoreSurface, *linesSurface, *levelSurface;

    struct {
	const Tetromino *next; 
	const Tetromino *current;
    } ttm_queue;

    const Tetromino *hold;
    
    SDL_TimerID autodrop_timer;
    SDL_TimerID softdrop_timer;
} game_process;

void draw_playfield_border(SDL_Renderer* renderer){
    // function to draw the playfield border which will 
    // be around the pieces.

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, default_alpha); 
    SDL_RenderClear(renderer);       

    // Drawing border
    for (int height = 0; height < PLAYFIELD_HEIGHT; height++){
	for (int width = 0; width < PLAYFIELD_WIDTH; width++){
	    if (height == 0 || height == PLAYFIELD_HEIGHT - 1){
		// Draw the row of gray border blocks for the top and bottom row
		
		//draw_block(renderer, width, height, GRAY);
		set_playfield_block(width, height, GRAY);
	    }
	    else if (width == 0 || width == PLAYFIELD_WIDTH - 1){
		// Draw the column of border blocks for the left and right columns 
		
		//draw_block(renderer, width, height, GRAY);
		set_playfield_block(width, height, GRAY);
	    }   
	}
    }

    render_changed = true;
}

void init_playfield(){
    // Initialise every value within the playfield (excluding border) to AN EMPTY BLOCK
    
    for(int row = 1; row < PLAYFIELD_HEIGHT - 1; row++){
	for(int col = 1; col < PLAYFIELD_WIDTH - 1; col++){
	    //game_process.PLAYFIELD[row][col] = 0;
	    //game_process.playfield[row][col] = EMPTY;   
	    
	    set_playfield_block(col, row, EMPTY);
	}
    }

    for(int row = 1; row < PLAYFIELD_HEIGHT - 1; row++){
	for(int col = 1; col < PLAYFIELD_WIDTH - 1; col++){
	    //game_process.PLAYFIELD[row][col] = 0;
	    printf("colour: %d\n", game_process.playfield[row][col]);
	}
    }
}

void draw_playfield(){
    for(int row = 0; row < PLAYFIELD_HEIGHT; row++){
	for(int col = 0; col < PLAYFIELD_WIDTH; col++){

	    set_playfield_block(col, row, game_process.playfield[row][col]);
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
    game_process.scoreSurface = NULL;
    game_process.score = 0;
    game_process.lines = 0;
    game_process.level = 0;

    init_playfield();
    draw_playfield_border(renderer);

    spawn_tetromino();
}

void lock_tetromino(){
    // reset the lock delay count 
    lock_delay_count = 0;

    for(int i = 0; i < 4; i++){
	u8 x = i * 2;
	u8 y = (i * 2) + 1;

	u8 _x = previous_coords[x];
	u8 _y = previous_coords[y];

	previous_coords[x] = 0;
	previous_coords[y] = 0;

	previous_ghost_coords[x] = 0;
	previous_ghost_coords[y] = 0;
	
	set_playfield_block(_x, _y, CURRENT_TETROMINO.shape.colour_id);
    }

    // check for complete lines and clear them 
    u8 row = PLAYFIELD_HEIGHT - 1;
    i8 copy_row = -1;
    i8 completed_lines = 0;

    while(row --> 1){
	bool completed_line = true;

	for(int col = 1; col < PLAYFIELD_WIDTH - 1; col++){
	    if (get_block_colour(col, row) == EMPTY){
		completed_line = false;
		break;
	    }
	}
	
	if(completed_line){
	    completed_lines++;

	    if(copy_row < row){
		copy_row = row;
	    }

	    for(int col = 1; col < PLAYFIELD_WIDTH - 1; col++){
		set_playfield_block(col, row, EMPTY);
	    }
	}
	else if (copy_row > row){
	    
	    for (int col = 1; col < PLAYFIELD_WIDTH - 1; col++){
		set_playfield_block(col, copy_row, get_block_colour(col, row));
	    }

	    copy_row--;

	}
    }

    //TODO: Could make a constant
    u16 scores[4] = {40, 100, 300, 1200};    

    // update the lines
    game_process.lines += completed_lines;

    if (game_process.lines % 10 == 0 && game_process.lines != 0){
	game_process.level += 1;
    }

    // update the score 

    printf("score: %d\n", scores[completed_lines - 1]);
    printf("level: %d\n", game_process.level);
    printf("lines: %d\n", game_process.lines);

    if(completed_lines > 0){
	// https://tetris.wiki/Scoring for more scoring info
	// 
	// -> Using Original Nintendo scoring system

	game_process.score += (scores[completed_lines - 1]) * (game_process.level + 1);
    }

    draw_playfield();
    spawn_tetromino();
}

bool render_showcase_tetromino(Tetromino_state tetromino, u8 previous_coords[]){
    // The differnce between this function and valid_render_tetromino, is that 
    // this function will specifically render the NEXT and HOLD tetromino showcases, as they
    // fall out of range of the border and so must be rendered separately

    u8 row = 0, col = 0;
    i8 x = tetromino.x, y = tetromino.y;

    // access correct rotation matrix data 
    u16 bit, piece; 
    piece = tetromino.shape.rotations[0]; // Still image -> default to first rotation matrix 

    for (int i = 0; i < 4; i++){
	u8 _x = previous_coords[i * 2];
	u8 _y = previous_coords[i * 2 + 1];

	if (previous_coords[0] != 0){
	    draw_block(renderer, _x, _y, EMPTY); 
	}
    }

    u8 pos = 0;
    u8 block_limit = 0;
    for (bit = 0x8000; bit > 0 && block_limit < 4; bit = bit >> 1){
	if (piece & bit){
	    u8 _x = x + col;
	    u8 _y = y + row;

	    draw_block(renderer, _x, _y, tetromino.shape.colour_id);

	    // store coordinates
	    previous_coords[pos * 2] = _x;
	    previous_coords[pos * 2 + 1] = _y;

	    pos++;
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

bool valid_render_tetromino(Tetromino_state tetromino, u8 *tetromino_coordinate_queue){ 
    u8 row = 0, col = 0;

    i8 x = tetromino.x;
    i8 y = tetromino.y;

    // access correct rotation matrix data
    u16 bit, piece;
    piece = tetromino.shape.rotations[tetromino.rotation_id]; 

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
		|| get_block_colour(_x, _y) != EMPTY){
		
		// tetromino block falls out of bounds 
		return false;
		break;
	    } 
	    else {
		int queue_limit = 2 * (block_limit + 1);
    
		if (array_size < queue_limit && tetromino_coordinate_queue != NULL){
		    tetromino_coordinate_queue = (u8*) realloc(
			tetromino_coordinate_queue, 
			queue_limit * sizeof(u8)
		    );

		    array_size += 2;
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

bool render_tetromino(Tetromino_state tetromino, u8 previous_coords[]){ //, u8 previous_coords[]){
    // TODO: Check the previous tetromino spawn as well to ensure that no tetromino type 
    //       is spawned more than once.

    u8 *tetromino_coordinate_queue = NULL;

    if (tetromino_coordinate_queue == NULL){
	tetromino_coordinate_queue = (u8*) malloc(2 * sizeof(u8)); // TODO: Check for a null pointer
    }
    
    if (!valid_render_tetromino(tetromino, tetromino_coordinate_queue)){
	return false;
    }

    // If previous coordinates are available, then clear them 
    
    for(int i = 0; i < 4; i++){
	u8 _x = previous_coords[i * 2];
	u8 _y = previous_coords[i * 2 + 1];

	if (previous_coords[0] != 0){
	    draw_block(renderer, _x, _y, EMPTY); 
	}
    }

    // render the newly updated block coordinates 
    
    for(int i = 0; i < 4; i++){
	u8 _x = tetromino_coordinate_queue[i * 2];
	u8 _y = tetromino_coordinate_queue[i * 2 + 1];
	
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
    
    if (!valid_render_tetromino(ghost_tetromino, tetromino_coordinate_queue)){
	return false;
    } 

    i8 ghost_x = ghost_tetromino.x, ghost_y = ghost_tetromino.y;

    for(int i = 0; i < 4; i++){
	u8 _x = previous_ghost_coords[i * 2];
	u8 _y = previous_ghost_coords[i * 2 + 1];
    
	// TODO: WORK ON CHECKING IF ANY COORDINATES AVAILABLE BEFORE RENDERING
	if (previous_ghost_coords[0] != 0){
	    draw_block(renderer, _x, _y, EMPTY); 
	}
    } 

    // shift all the blocks down to the lowest possible row
    while(valid_render_tetromino(ghost_tetromino, tetromino_coordinate_queue)){
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
	
	draw_outline(renderer, _x, _y, ghost_tetromino.shape.colour_id);

	// store the previous coordinates
	previous_ghost_coords[i * 2] = _x;
	previous_ghost_coords[i * 2 + 1] = _y;
    }

    free(tetromino_coordinate_queue);
    return true;
}

bool render_current_tetromino(Tetromino_state tetromino, Tetromino_state ghost_tetromino){

    if(render_ghost_tetromino(ghost_tetromino, previous_ghost_coords)
       && render_tetromino(tetromino, previous_coords)){
	GHOST_TETROMINO = ghost_tetromino;
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
    // use current time as a seed 
    srand(time(0));

    if(game_process.ttm_queue.next == NULL){
	game_process.ttm_queue.next = (const Tetromino*) &TETROMINOS[rand() % NUM_TETROMINOS];
    } 

    game_process.ttm_queue.current = game_process.ttm_queue.next; 
    
    // Initialise the Tetromino_state structs with our selected Tetromino from queue
    Tetromino_state current_tetromino = {
	*game_process.ttm_queue.current,
	0,
	(u8) (PLAYFIELD_WIDTH / 2) - 2, (u8) 1,
    };
    
    // ghost tetromino equivalent
    Tetromino_state current_ghost_tetromino = {
	*game_process.ttm_queue.current,
	0,
	(u8) (PLAYFIELD_WIDTH / 2) - 2, (u8) 1,
    };

    CURRENT_TETROMINO = current_tetromino;
    GHOST_TETROMINO = current_ghost_tetromino;

    if (!can_swap_held){
	can_swap_held = true;
    }

    game_process.ttm_queue.next = (const Tetromino*) &TETROMINOS[rand() % NUM_TETROMINOS];
    
    render_next_showcase();
    render_showcase_information(game_process.scoreSurface);

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

void render_next_showcase(){
    // Set the text colour -> implementing hexadecimal
    SDL_Color fg_textColour = { 0xFF, 0xFF, 0xFF };

    SDL_Surface *nextSurface = TTF_RenderText_Blended(gFont, "NEXT", fg_textColour);

    // convert SDL_Surface into a texture 
    SDL_Texture *next_message = SDL_CreateTextureFromSurface(renderer, nextSurface);

    if (next_message == NULL){
	fprintf(stderr, 
		"\nSDL_CreateTextureFromSurface Error: %s\n", 
		SDL_GetError()
	);
	exit(1);
    }

    SDL_Rect next_rect;
    next_rect.x = 15 * (BLOCK_SIZE), next_rect.y = 1 * (BLOCK_SIZE);
    next_rect.w = 150, next_rect.h = 50;

    SDL_RenderCopy(renderer, next_message, NULL, &next_rect);

    Tetromino_state next_tetromino = {
	*game_process.ttm_queue.next, 
	0,
	(u8) 14, (u8) 2,
    };
    
    render_showcase_tetromino(next_tetromino, previous_next_coords);

    SDL_DestroyTexture(next_message);
    SDL_FreeSurface(nextSurface);
}

void render_hold_showcase(){
    // Set the text colour -> implementing hexadecimal
    SDL_Color fg_textColour = { 0xFF, 0xFF, 0xFF };

    SDL_Surface *holdSurface = TTF_RenderText_Blended(gFont, "HOLD", fg_textColour);

    // convert SDL_Surface into a texture 
    SDL_Texture *hold_message = SDL_CreateTextureFromSurface(renderer, holdSurface);

    if (hold_message == NULL){
	fprintf(stderr, 
		"\nSDL_CreateTextureFromSurface Error: %s\n", 
		SDL_GetError()
	);
	exit(1);
    }

    SDL_Rect hold_rect;
    hold_rect.x = 21 * (BLOCK_SIZE), hold_rect.y = 1 * (BLOCK_SIZE);
    hold_rect.w = 150, hold_rect.h = 50;

    SDL_RenderCopy(renderer, hold_message, NULL, &hold_rect);

    Tetromino_state hold_tetromino = {
	*game_process.hold, 
	0,
	(u8) 20, (u8) 2,
    };
    
    render_showcase_tetromino(hold_tetromino, previous_hold_coords);

    SDL_DestroyTexture(hold_message);
    SDL_FreeSurface(holdSurface);
}

void render_showcase_information(SDL_Surface *scoreSurface){
    // Show the score, number of lines cleared and level
    SDL_Color textColor = { 0xFF, 0xFF, 0xFF};
    
    char score_buffer[10];
    char line_buffer[10];
    char level_buffer[10];

    scoreSurface = TTF_RenderText_Blended(
	gFont, SDL_itoa(game_process.score, score_buffer, 10), textColor
    );

    SDL_Texture *scoreMessage = SDL_CreateTextureFromSurface(renderer, scoreSurface);

    if (scoreMessage == NULL){
	fprintf(stderr, 
		"\nSDL_CreateTextureFromSurface Error: %s\n", 
		SDL_GetError()
	);
	exit(1);
    }

    int NUM_DIGITS = count_digits(game_process.score);

    SDL_Rect score_rect;
    score_rect.x = 15 * (BLOCK_SIZE), score_rect.y = 10 * (BLOCK_SIZE);
    score_rect.w = 40 * (NUM_DIGITS), score_rect.h = 50;

    if (scoreSurface != NULL){
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(renderer, &score_rect);

	render_changed = true;
    }

    SDL_RenderCopy(renderer, scoreMessage, NULL, &score_rect);

    SDL_DestroyTexture(scoreMessage);
    SDL_FreeSurface(scoreSurface);
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
	game_process.autodrop_timer = SDL_AddTimer(450, autodrop_callback, NULL);
    }

    if (game_process.softdrop_timer == 0){
	game_process.softdrop_timer = SDL_AddTimer(5, softdrop_callback, NULL);
    }

    Tetromino_state tetromino = CURRENT_TETROMINO;
    Tetromino_state ghost_tetromino = GHOST_TETROMINO;

    // action from the keyboard 

    switch(TETROMINO_ACTION){
	case NONE: 
	    break; 

	case MOVE_LEFT:
	    
	    tetromino.x -= 1;
	    ghost_tetromino.x -= 1;

	    render_current_tetromino(tetromino, ghost_tetromino);

	    break;

	case MOVE_RIGHT:
	    
	    tetromino.x += 1;
	    ghost_tetromino.x += 1;

	    render_current_tetromino(tetromino, ghost_tetromino);

	    break; 

	case ROTATE:

	    tetromino = wall_kick(tetromino);
	    tetromino.rotation_id = (tetromino.rotation_id + 1) % 4;

	    ghost_tetromino = wall_kick(ghost_tetromino);
	    ghost_tetromino.rotation_id = (ghost_tetromino.rotation_id + 1) % 4;

	    render_current_tetromino(tetromino, ghost_tetromino);

	    break;

	case SOFT_DROP:
	    
	    if (SOFTDROP_EXPIRED){
		tetromino.y += 1;
		SOFTDROP_EXPIRED = false;
	    
		if (!render_current_tetromino(tetromino, ghost_tetromino)){
		    lock_delay_count++;
		}
		else{
		    lock_delay_count = 0;
		}
	    
		if (lock_delay_count >= lock_delay_limit){
		    lock_tetromino();
		}
	    } 
	    
	    break;
	case HARD_DROP:

	    tetromino.y += 1;

	    while(render_current_tetromino(tetromino, ghost_tetromino)){
		tetromino.y += 1;
	    }
	    
	    // lock tetromino
	    lock_tetromino();

	    break;

	case AUTO_DROP:
	    // Write a function to deal with tetrominos that are "locked"
	    // at the bottom of the board
	    
	    if (AUTODROP_EXPIRED){
		tetromino.y += 1;
		AUTODROP_EXPIRED = false;
		
		if (!render_current_tetromino(tetromino, ghost_tetromino)){
		    lock_delay_count++;
		}
		else{
		    lock_delay_count = 0;
		}
	    
		if (lock_delay_count >= lock_delay_limit){
		    lock_tetromino();
		}	
	    } 

	    break;

	case HOLD:
	    if (can_swap_held){
		const Tetromino *temp = game_process.ttm_queue.current;
		
		// check if first time holding
		if (!tetromino_held){
		    game_process.hold = temp;

		    tetromino_held = true;

		    // spawn a new tetromino 
		    spawn_tetromino();
		}
		else{
		    game_process.ttm_queue.current = game_process.hold;
		    game_process.hold = temp;

		    // re-initalise the current tetromino

		    Tetromino_state current_tetromino = {
			*game_process.ttm_queue.current,
			0,
			(u8) (PLAYFIELD_WIDTH / 2) - 2, (u8) 1,
		    };
    
		    // ghost tetromino equivalent
		    Tetromino_state current_ghost_tetromino = {
			*game_process.ttm_queue.current,
			0,
			(u8) (PLAYFIELD_WIDTH / 2) - 2, (u8) 1,
		    };

		    CURRENT_TETROMINO = current_tetromino;
		    GHOST_TETROMINO = current_ghost_tetromino;

		    // Move tetromino back to the top
		    CURRENT_TETROMINO.y = 1;
		}

		can_swap_held = false;
	    }

	    // render showcase info for hold
	    render_hold_showcase();
	    
	    break;
    }  

    TETROMINO_ACTION = NONE;
}

u8 get_block_colour(u8 _x, u8 _y){
    return game_process.playfield[_y][_x];
}

void set_playfield_block(u8 _x, u8 _y, u8 colour){
    game_process.playfield[_y][_x] = colour;

    draw_block(renderer, _x, _y, colour);
}
