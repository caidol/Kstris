#include "input.h"

// Basic SDL input system -> will be altered later
void process_input(){
    SDL_Event event;

    // poll until all events are handled
    while (SDL_PollEvent(&event)){
	switch(event.type){
	    case SDL_QUIT:
		// TODO: Potentially return QUIT 
		exit(0);
		break;
	    case SDL_KEYDOWN:
		switch(event.key.keysym.sym){
		    case SDLK_ESCAPE:
			exit(0);
			break;
		    default:
			break;
	    }
	}
    }
}
