#ifndef COMPARE_H
#define COMPARE_H

#include "math.h"

int compare(float a, float b, float epsilon) {
    if (fabs(a - b) < epsilon) return 0;
    if (a < b) return -1;
    return 1;
}

int uint32_cmp(const void* a, const void* b) {
    return *(uint32_t*)a - *(uint32_t*)b;
}

#endif  // COMPARE_H
