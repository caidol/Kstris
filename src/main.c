#include "main.h"

int main(int argc, const char * argv[]){
    // Initialise system
    
    init_system();

    bool quit = false; 
    while(!quit){
	
	process_input();

	update_game();

	update_render();
	
	// to allow for 30fps wait 1000 / 30 = 33ms
	// attempting 30fps for now
	//SDL_Delay(33);
	SDL_Delay(33);
    }
    
    sdl_cleanup();
    return 0;
};
