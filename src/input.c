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
		    
		    case SDLK_s:
		    case SDLK_DOWN:
			TETROMINO_ACTION = SOFT_DROP;
			break;
		    
		    case SDLK_w:
		    case SDLK_UP:
			TETROMINO_ACTION = ROTATE;
			break;
		    
		    case SDLK_a:
		    case SDLK_LEFT:
			TETROMINO_ACTION = MOVE_LEFT;
			break;

		    case SDLK_d:
		    case SDLK_RIGHT:
			TETROMINO_ACTION = MOVE_RIGHT;
			break;

		    case SDLK_SPACE:
			TETROMINO_ACTION = HARD_DROP;
			break;

		    default:
			break;
	    }
	}
    }
}
