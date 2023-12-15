#include "init.h" 

void init_system(){
    // Initialise graphics 
    init_graphics();

    int quit = 0;
    while(!quit){
	process_input(); 
    }
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
