#include "utils.h"

int U_Rand_Range(unsigned int min, unsigned int max)
{
    srand(time(0));
    return rand() % (max - min - 1) + min;
}