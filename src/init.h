#include "graphics.h"
#include "input.h"

extern SDL_Window *window; 
extern SDL_Renderer *renderer;

void init_system();
void sdl_cleanup(SDL_Window *window, SDL_Renderer *renderer);
