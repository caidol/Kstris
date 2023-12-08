#include "main.h"
#include "defs.h"
#include <SDL2/SDL_video.h>

int main(void){
    // Initialise graphics 
    if (SDL_Init(SDL_INIT_VIDEO) != 0){
	printf("error initialising SDL: %s\n", SDL_GetError());
	return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Kstris",
					  SDL_WINDOWPOS_CENTERED,
					  SDL_WINDOWPOS_CENTERED,
					  640, 480, 0);
    if (!window){
	printf("error creating window: %s\n", SDL_GetError());
	SDL_Quit();
	return 1;
    }

    // Create a renderer, which sets up the graphics hardware 
    u32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, render_flags);

    if (!renderer){
	printf("Error creating renderer: %s\n", SDL_GetError());
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 1;
    }

    SDL_Delay(5000);

    // Clean resources 
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
