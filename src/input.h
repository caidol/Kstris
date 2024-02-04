#include "defs.h"

#ifndef _INPUT_ACTIONS_ 
#define _INPUT_ACTIONS_

typedef enum{
    NONE,
    MOVE_LEFT,
    MOVE_RIGHT, 
    ROTATE, 
    SOFT_DROP, 
    HARD_DROP,
    AUTO_DROP,
} Tetromino_Actions;

extern Tetromino_Actions TETROMINO_ACTION;

#endif

void process_input();
