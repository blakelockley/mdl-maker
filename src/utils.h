#ifndef UTILS_H
#define UTILS_H

#include "linmath.h"
#include "macros.h"

void point_intersect_plane(vec3 p, vec3 l0, vec3 l, float t) {
    vec3_scale(p, l, t);
    vec3_add(p, p, l0);
}

bool intersect_plane(float *t, const vec3 n, const vec3 p0, const vec3 l0, const vec3 l) {
    // assumes n and l are normalised

    float denom = vec3_dot(n, l);
    if (fabs(denom) > 1e-6) {
        vec3 delta;
        vec3_sub(delta, p0, l0);
        
        *t = vec3_dot(delta, n) / denom; 
        return (*t >= 0);
    }

    return false;
}

#endif  // UTILS_H
