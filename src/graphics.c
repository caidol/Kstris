#include "main.h"
#include <SDL2/SDL_events.h>
#include <stdlib.h>

#define default_alpha 255

#define NUM_BLOCKS 7

// tetronimo colour val defs

enum BLOCK_NAMES{
    RED, 
    GREEN, 
    BLUE, 
    CYAN, 
    ORANGE, 
    MAGENTA, 
    GRAY,
};

static const int BLOCK_COLOURS[NUM_BLOCKS][3] = {
    {255, 0, 0}, // RED
    {0, 255, 0}, // GREEN
    {0, 0, 255}, // BLUE
    {0, 100, 0}, // CYAN
    {255, 165, 0}, // ORANGE 
    {255, 0, 255}, // MAGENTA
    {128, 128, 128}, // GRAY
};

void draw_block(SDL_Renderer* renderer, u8 x_pos, u8 y_pos, u8 colour){
    //TODO: Add more comments
    u8 rgb_code[3];
    *rgb_code = *BLOCK_COLOURS[colour];
    *(rgb_code + 1) = *(BLOCK_COLOURS[colour] + 1);
    *(rgb_code + 2) = *(BLOCK_COLOURS[colour] + 2);
    
    SDL_Rect outer;
    SDL_Rect inner;
    
    // setting boundary for outer colour of tetronimo block 
    outer.x = (x_pos + 1) * BLOCK_SIZE;
    outer.y = (y_pos + 1) * BLOCK_SIZE; 
    outer.w = BLOCK_SIZE;
    outer.h = BLOCK_SIZE; 

    // setting boundary for inner colour of tetronimo block
    inner.x = (x_pos + 1) * BLOCK_SIZE + 4;
    inner.y = (y_pos + 1) * BLOCK_SIZE + 4; 
    inner.w = BLOCK_SIZE - 8;
    inner.h = BLOCK_SIZE - 8;

    // Perform shifts to change the colour of the outside edge
    unsigned int r, g, b;
    r = (*(rgb_code) >> 1) & 0xFF;
    g = (*(rgb_code + 1) >> 1) & 0xFF;
    b = (*(rgb_code + 2) >> 2) & 0xFF;
    
    SDL_SetRenderDrawColor(renderer, r, g, b, default_alpha);
    SDL_RenderFillRect(renderer, &outer);

    SDL_SetRenderDrawColor(renderer, *(rgb_code), *(rgb_code + 1), *(rgb_code + 2), default_alpha);
    SDL_RenderFillRect(renderer, &inner);
}

void draw_playfield(SDL_Renderer* renderer){
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    // Draw in a coloured box 
    SDL_RenderClear(renderer);   
    SDL_RenderPresent(renderer);

    draw_block(renderer, PLAYFIELD_WIDTH / 2, PLAYFIELD_HEIGHT / 2, ORANGE); 

    SDL_RenderPresent(renderer);
}

// Initialise graphics
void init_graphics(){
    if (SDL_Init(SDL_INIT_VIDEO) != 0){
	printf("error initialising SDL: %s\n", SDL_GetError());
	exit(1);
    }

    SDL_Window* window = SDL_CreateWindow(
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
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, render_flags);

    if (!renderer){
	printf("Error creating renderer: %s\n", SDL_GetError());
	SDL_DestroyWindow(window);
	SDL_Quit();
	
	exit(1);
    }
    
    draw_playfield(renderer);
}

void render_frame(SDL_Renderer* renderer){
    SDL_RenderPresent(renderer);
}

void cleanup_graphics(SDL_Window* window){
    // Clean resources by destroying window and quitting from SDL2 
    SDL_DestroyWindow(window);
    SDL_Quit();
}
