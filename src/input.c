#include "input.h"

Tetromino_Actions TETROMINO_ACTION;

// Basic SDL input system -> will be altered later
void process_input(){
    SDL_Event event;
    //printf("event: %d\n", SDL_PollEvent(&event));

    // poll until all events are handled
    while (SDL_PollEvent(&event)){
	switch(event.type){
	    case SDL_QUIT:
		printf("QUIT\n");
		// TODO: Potentially return QUIT 
		exit(0);
		break;
	    case SDL_KEYDOWN:
		printf("KEYDOWN\n");
		switch(event.key.keysym.sym){
		    case SDLK_ESCAPE:
			exit(0);
			break;
		    
		    case SDLK_s:
		    case SDLK_DOWN:
			printf("ACTION: SOFT DROP\n");
			TETROMINO_ACTION = SOFT_DROP;
			break;
		    
		    case SDLK_w:
		    case SDLK_UP:
			printf("ACTION: ROTATE\n");
			TETROMINO_ACTION = ROTATE;
			break;
		    
		    case SDLK_a:
		    case SDLK_LEFT:
			printf("ACTION: MOVE LEFT\n");
			TETROMINO_ACTION = MOVE_LEFT;
			break;

		    case SDLK_d:
		    case SDLK_RIGHT:
			printf("ACTION: MOVE RIGHT\n");
			TETROMINO_ACTION = MOVE_RIGHT;
			break;

		    case SDLK_SPACE:
			printf("ACTION: HARD DROP\n");
			TETROMINO_ACTION = HARD_DROP;
			break;

		    default:
			break;
	    }
	}
    }
}
