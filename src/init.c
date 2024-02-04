#include "init.h" 

void init_system(){
    // Initialise SDL_ttf 
    if (TTF_Init() == -1){
	printf("Error initialising ttf: %s\n", TTF_GetError());
	printf("Program Quitting...");
	
	exit(1);
    }


    // Initialise graphics 
    init_graphics();
    
    init_tetris();
}

void sdl_cleanup(SDL_Window *window, SDL_Renderer *renderer){
    if (window){
	SDL_DestroyWindow(window);
    }

    if (renderer){
	SDL_DestroyRenderer(renderer);
    }

    // Completely quit SDL 
    SDL_Quit();
}
