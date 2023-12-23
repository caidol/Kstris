// SDL libraries 
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_image.h> 
#include <SDL2/SDL2_gfxPrimitives.h>

static const char * WINDOW_TITLE = "Kstris";

// block is square so no need to define separate dimensions 
static const int BLOCK_SIZE = 30;

// playfield width is 2 greater than standard as this includes border blocks 
static const int PLAYFIELD_HEIGHT = 22;
static const int PLAYFIELD_WIDTH = 12;

static const int WINDOW_HEIGHT = (PLAYFIELD_HEIGHT * BLOCK_SIZE) + 40;
static const int WINDOW_WIDTH = (PLAYFIELD_WIDTH * BLOCK_SIZE) + 80;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
