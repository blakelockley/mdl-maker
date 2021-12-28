#ifndef COMPARE_H
#define COMPARE_H

#include "math.h"

int compare(float a, float b, float epsilon) {
    if (fabs(a - b) < epsilon) return 0;
    if (a < b) return -1;
    return 1;
}

#endif  // COMPARE_H
