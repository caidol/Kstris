#include "main.h"
#include "game.h"

int main(int argc, const char * argv[]){
    // Initialise system
    
    init_system();

    bool quit = false; 
    while(!quit){
	//printf("test\n");
	
	process_input();

	update_game();

	// To allow for 60fps wait 1000 / 60
	SDL_Delay(16);
    }
	
    return 0;
};
