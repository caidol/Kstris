#include "defs.h"

#ifndef _BLOCK_COLOURS_
#define _BLOCK_COLOURS_

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern SDL_Texture *display;
extern TTF_Font *gFont;

#define NUM_BLOCK_COLOURS 9

// tetronimo colour val defs

enum colours{
    CYAN, 
    BLUE, 
    ORANGE, 
    GREEN, 
    RED, 
    MAGENTA, 
    YELLOW, 
    GRAY, 
};

// rgb codes for each block colour
static const u8 BLOCK_COLOURS_RGB[NUM_BLOCK_COLOURS][3] = {
    {0, 204, 204}, // CYAN
    {0, 128, 255}, // BLUE
    {255, 128, 0}, // ORANGE
    {51, 255, 51}, // GREEN
    {255, 0, 0}, // RED 
    {127, 0, 255}, // MAGENTA
    {255, 255, 51}, // YELLOW
    {96, 96, 96}, // GRAY
    {0, 0, 0} // BLACK (EMPTY)
};
#endif

void init_graphics();

void pre_render(SDL_Texture *display);

void draw_block(SDL_Renderer* renderer, u8 x_pos, u8 y_pos, u8 colour);

void draw_outline(SDL_Renderer *renderer, u8 x_pos, u8 y_pos, u8 colour);

void draw_playfield_border(SDL_Renderer *renderer);

void render_frame(SDL_Renderer *renderer, SDL_Texture *texture);

void update_render(SDL_Texture *display);

void cleanup_graphics();
