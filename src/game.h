#include "defs.h"
#include "main.h"

// Tetromino struct which defines its color and rotation schemes 
//	--> per tetromino type

typedef struct{
    // Follow standard RGBA convention 0xRRGGBBAA
    u32 colour;

    // The following array contains the rotation scheme 
    // for each of the four max possible rotations. 
    //
    // The rotation system being used is the Nintendo rotation system,
    // where each element a 16 bit hex form of a 4x4 matrix for the rotation 
    
    u16 rotations[4];

} Tetromino;

// Tetromino struct which defines it's blueprint 
//	--> per type of tetromino

typedef struct {
    // tetromino shape to inherit
    Tetromino shape;

    // used to select correct rotation from Tetromino.rotations array
    u8 rotation_id; 
 
    u8 x;
    u8 y;

} Tetromino_state;
