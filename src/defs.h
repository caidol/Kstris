// Standard libraries 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <stdbool.h> 
#include <assert.h>
#include <unistd.h>
#include <time.h>

// SDL libraries 
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_scancode.h>

#ifndef _GLOBAL_CONSTANTS
#define _GLOBAL_CONSTANTS

#define default_alpha 255

// unsigned integers 
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

// signed integers 
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;

static const char * WINDOW_TITLE = "Kstris";

// block is square so no need to define separate dimensions 
static const u8 BLOCK_SIZE = 40;

// playfield width is 2 greater than standard as this includes border blocks 
static const u8 PLAYFIELD_HEIGHT = 22;
static const u8 PLAYFIELD_WIDTH = 12;

static const int WINDOW_HEIGHT = (PLAYFIELD_HEIGHT * BLOCK_SIZE) + 60;
static const int WINDOW_WIDTH = (PLAYFIELD_WIDTH * BLOCK_SIZE) + 600;

extern bool render_changed;

#endif
