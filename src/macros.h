#ifndef MACROS_H
#define MACROS_H

#define ABS(a)	   (((a) < 0) ? -(a) : (a))
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

#endif // MACROS_H