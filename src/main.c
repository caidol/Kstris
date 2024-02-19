#include "main.h"
#include <stdio.h>

SDL_Texture *display;

int main(int argc, const char * argv[]){
    // Initialise system
    
    init_system();
    update_render(display);

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
	fprintf(stderr, 
	    "\nUnable to initialize SDL: %s\n", 
	    SDL_GetError()
	);
	return 1;
    }

    atexit(cleanup_graphics);

    bool quit = false; 
    while(!quit){
	pre_render(display);

	process_input();

	update_game();

	update_render(display);
	
	// to allow for 30fps wait 1000 / 30 = 33ms
	// attempting 30fps for now
	SDL_Delay(33);
    }
    
    sdl_cleanup();
    return 0;
};
