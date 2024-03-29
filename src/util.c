#include "util.h"

int count_digits(int num){
    int count = 0;

    do {
	num /= 10;
	count++;

    } while (num != 0);

    return count;
} 

// Using the Knuth Shuffle algorithm, thanks to https://github.com/dashed/tetris-sdl-c 
// for code to use 

int rrand(int m) {
    return (int)((double)m * ( rand() / (RAND_MAX+1.0) ));
}

#define BYTE(X) ((unsigned char *)(X))
void shuffle(void *obj, size_t nmemb, size_t size) {

    // random seed
    srand(time(NULL));

    void *temp = malloc(size);
    size_t n = nmemb;
    while ( n > 1 ) {
        size_t k = rrand(n--);
        memcpy(temp, BYTE(obj) + n*size, size);
        memcpy(BYTE(obj) + n*size, BYTE(obj) + k*size, size);
        memcpy(BYTE(obj) + k*size, temp, size);
    }
    free(temp);
}
