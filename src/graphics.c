#include "main.h"
#include "graphics.h"
#include "game.h"
#include <SDL2/SDL_events.h>
#include <stdlib.h>

#define default_alpha 255

#define check_rgb_overflow(channel) ( channel > 255 ) ? ( channel = 255 ) : ( channel )
#define check_rgb_underflow(channel) ( channel < 0 ) ? ( channel = 0 ) : ( channel )

// define the SDL window and renderer 
SDL_Window* window;
SDL_Renderer* renderer;

void draw_block(SDL_Renderer* renderer, u8 x_pos, u8 y_pos, u8 colour){
    //TODO: Add more comments
    printf("drawing block\n");
    assert(colour >= 0 && colour <= NUM_BLOCK_COLOURS);
    u8 rgb_code[3] = {*BLOCK_COLOURS_RGB[colour], *(BLOCK_COLOURS_RGB[colour] + 1), *(BLOCK_COLOURS_RGB[colour] + 2)};

    SDL_Rect outer;
    SDL_Rect inner;
    
    // setting boundary for outer colour of tetronimo block 
    outer.x = (x_pos + 1) * BLOCK_SIZE;
    outer.y = (y_pos + 1) * BLOCK_SIZE; 
    outer.w = BLOCK_SIZE;
    outer.h = BLOCK_SIZE; 

    // setting boundary for inner colour of tetronimo block
    inner.x = ((x_pos + 1) * BLOCK_SIZE) + 3;
    inner.y = ((y_pos + 1) * BLOCK_SIZE) + 3; 
    inner.w = BLOCK_SIZE - 6;
    inner.h = BLOCK_SIZE - 6;

    // Perform shifts to change the colour of the outside edge
    unsigned int r, g, b;
    r = (*rgb_code >> 1) & 0xFF;
    check_rgb_overflow(r); check_rgb_underflow(r);
    g = *(rgb_code + 1) >> 1 & 0xFF;
    check_rgb_overflow(g); check_rgb_underflow(g);
    b = *(rgb_code + 2) >> 1 & 0xFF; 
    check_rgb_overflow(b); check_rgb_underflow(b);

    SDL_SetRenderDrawColor(renderer, r, g, b, default_alpha);
    SDL_RenderFillRect(renderer, &outer);

    SDL_SetRenderDrawColor(renderer, *rgb_code, *(rgb_code + 1), *(rgb_code + 2), default_alpha);
    SDL_RenderFillRect(renderer, &inner);
}

void draw_outline(SDL_Renderer *renderer, u8 x_pos, u8 y_pos, u8 colour){
    assert(colour >= 0 && colour <= NUM_BLOCK_COLOURS);
    u8 rgb_code[3] = {*BLOCK_COLOURS_RGB[colour], *(BLOCK_COLOURS_RGB[colour] + 1), *(BLOCK_COLOURS_RGB[colour] + 2)};
    
    SDL_Rect outline;
    SDL_Rect inner;

    outline.x = (x_pos + 1) * BLOCK_SIZE;
    outline.y = (y_pos + 1) * BLOCK_SIZE; 
    outline.w = BLOCK_SIZE;
    outline.h = BLOCK_SIZE; 

    inner.x = ((x_pos + 1) * BLOCK_SIZE) + 2;
    inner.y = ((y_pos + 1) * BLOCK_SIZE) + 2; 
    inner.w = BLOCK_SIZE - 4;
    inner.h = BLOCK_SIZE - 4; 

    SDL_SetRenderDrawColor(renderer, *rgb_code, *(rgb_code + 1), *(rgb_code + 2), default_alpha);
    SDL_RenderFillRect(renderer, &outline);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, default_alpha);
    SDL_RenderFillRect(renderer, &inner);
}

void draw_playfield(SDL_Renderer* renderer){
    // Function to draw the playfield, starting by drawing the border around which 
    // the playfield will be contained

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, default_alpha);
    
    SDL_RenderClear(renderer);   
    SDL_RenderPresent(renderer);

    // Drawing border
    for (int width = 0; width < PLAYFIELD_WIDTH; width++){
	for (int height = 0; height < PLAYFIELD_HEIGHT; height++){
	    if (height == 0 || height == PLAYFIELD_HEIGHT - 1){
		// Draw the row of gray border blocks for the top and bottom row
		
		draw_block(renderer, width, height, GRAY);
	    }
	    else if (width == 0 || width == PLAYFIELD_WIDTH - 1){
		// Draw the column of border blocks for the left and right columns 
		
		draw_block(renderer, width, height, GRAY);
	    }   
	}
    }

    SDL_RenderPresent(renderer);
}

// Initialise graphics
void init_graphics(){
    if (SDL_Init(SDL_INIT_VIDEO) != 0){
	printf("error initialising SDL: %s\n", SDL_GetError());
	exit(1);
    }

    window = SDL_CreateWindow(
	// Window title 
	WINDOW_TITLE,
	SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
	WINDOW_WIDTH, WINDOW_HEIGHT,
	0
    );


    if (!window){
	printf("error creating window: %s\n", SDL_GetError());
	SDL_Quit();
	exit(1);
    }

    // Create a renderer, which sets up the graphics hardware 
    u32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    renderer = SDL_CreateRenderer(window, -1, render_flags);

    if (!renderer){
	printf("Error creating renderer: %s\n", SDL_GetError());
	SDL_DestroyWindow(window);
	SDL_Quit();
	
	exit(1);
    }
    
    draw_playfield(renderer);
    SDL_RenderPresent(renderer);
    // Test below
    /*
    Tetromino_state tetromino_t = {
	TETROMINOS[3],
	1,
	3, 3 
    };
    */

    //u8 *tetromino_coords = NULL;
    //valid_render_tetromino(tetromino_t, tetromino_t.x, tetromino_t.y, tetromino_coords); 
    //render_ghost_tetromino(tetromino_t, tetromino_t.x, tetromino_t.y);
    spawn_tetromino();
    SDL_RenderPresent(renderer);
}

void render_frame(SDL_Renderer* renderer){
    SDL_RenderPresent(renderer);
}

void cleanup_graphics(SDL_Window* window){
    // Clean resources by destroying window and quitting from SDL2 
    SDL_DestroyWindow(window);
    SDL_Quit();
}
