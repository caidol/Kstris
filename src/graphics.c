#include "defs.h"
#include <SDL2/SDL_video.h>

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

    SDL_Delay(5000);

    // Clean resources 
    SDL_DestroyWindow(window);
    SDL_Quit();

}
