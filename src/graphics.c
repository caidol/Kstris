#include "main.h"
#include <SDL2/SDL_events.h>
#include <stdlib.h>

#define default_alpha 255

// tetronimo colour val defs

enum block_colours{
    RED, 
    GREEN, 
    BLUE, 
    CYAN, 
    ORANGE, 
    MAGENTA, 
    GRAY,
};

void draw_block(SDL_Renderer* renderer, u8 x_pos, u8 y_pos, u8 colour){
    u8 rgb_code[3];
    
    // TODO: Change the below to make it more elegant
    switch(colour){
	case RED:
	    rgb_code[0] = 255; rgb_code[1] = 0; rgb_code[2] = 0;
	    break;
	case GREEN:
	    rgb_code[0] = 0; rgb_code[1] = 255; rgb_code[2] = 0;
	    break; 
	case BLUE:
	    rgb_code[0] = 0; rgb_code[1] = 0; rgb_code[2] = 255;
	    break;
	case CYAN:
	    rgb_code[0] = 0; rgb_code[1] = 100; rgb_code[2] = 0;
	    break;
	case ORANGE:
	    rgb_code[0] = 255; rgb_code[1] = 165; rgb_code[2] = 0;
	    break;
	case MAGENTA:
	    rgb_code[0] = 255; rgb_code[1] =  0; rgb_code[2] = 255;
	    break; 
	case GRAY:
	    rgb_code[0] = 128; rgb_code[1] = 128; rgb_code[2] = 128;
	    break; 
	default:
	    return;
    }

    SDL_Rect outer;
    SDL_Rect inner;
    
    // setting boundary for outer colour of tetronimo block 
    outer.x = (x_pos + 1) * BLOCK_SIZE;
    outer.y = (y_pos + 1) * BLOCK_SIZE; 
    outer.w = BLOCK_SIZE;
    outer.h = BLOCK_SIZE; 

    // setting boundary for inner colour of tetronimo block
    inner.x = (x_pos + 1) * BLOCK_SIZE + 5;
    inner.y = (y_pos + 1) * BLOCK_SIZE + 5; 
    inner.w = BLOCK_SIZE - 10;
    inner.h = BLOCK_SIZE - 10;

    // Perform shifts to change the colour of the outside edge

    // TODO: Find out how to shift rgb code for colour to make it darker
    SDL_SetRenderDrawColor(renderer, 107, 28, 2, 255);
    SDL_RenderFillRect(renderer, &outer);

    SDL_SetRenderDrawColor(renderer, *(rgb_code), *(rgb_code + 1), *(rgb_code + 2), default_alpha);
    SDL_RenderFillRect(renderer, &inner);
}

void draw_playfield(SDL_Renderer* renderer){
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    // Draw in a coloured box 
    SDL_RenderClear(renderer);   
    SDL_RenderPresent(renderer);

    draw_block(renderer, PLAYFIELD_WIDTH / 2, PLAYFIELD_HEIGHT / 2, RED); 

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
