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

void sdl_cleanup(){
    cleanup_graphics();
	
    // Completely quit SDL 
    SDL_Quit();
}
