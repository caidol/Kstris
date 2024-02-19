#include "util.h"

int count_digits(int num){
    int count = 0;

    do {
	num /= 10;
	count++;

    } while (num != 0);

    return count;
} 
