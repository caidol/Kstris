#include "defs.h"

extern SDL_Window *window;
extern SDL_Renderer *renderer;

#define NUM_BLOCK_COLOURS 8

// tetronimo colour val defs
#ifndef _BLOCK_COLOURS_
#define _BLOCK_COLOURS_
enum colours{
    RED, 
    GREEN, 
    BLUE, 
    CYAN, 
    ORANGE, 
    MAGENTA,
    YELLOW, 
    GRAY,
};

// rgb codes for each block colour
static const u8 BLOCK_COLOURS_RGB[NUM_BLOCK_COLOURS][3] = {
    {255, 0, 0}, // RED
    {0, 255, 0}, // GREEN
    {0, 0, 255}, // BLUE
    {0, 255, 255}, // CYAN
    {255, 141, 0}, // ORANGE
    {255, 0, 255}, // MAGENTA
    {255, 255, 0}, // YELLOW
    {128, 128, 128}, // GRAY
};
#endif

void init_graphics();
void draw_block(SDL_Renderer* renderer, u8 x_pos, u8 y_pos, u8 colour);
