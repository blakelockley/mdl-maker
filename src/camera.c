#include "camera.h"
#include "linmath.h"

#include <stdio.h>
#include <stdlib.h>

camera_t camera;
extern int width, height;

void init_camera() {
    vec3_set(camera.pos, 0.0, 1.0f, 2.0f);
    vec3_set(camera.dir, 0.0, -1.0, -1.0);
    
    vec3_set(camera.up, 0.0, 1.0, 0.0);
    vec3_set(camera.right, 1.0, 0.0, 0.0);
}

void get_view_matrix(mat4x4 m) { 
    mat4x4_look_at(m, camera.pos, (vec3){0.0f, 0.0f, 0.0f}, camera.up);
}

void set_camera_position(vec3 pos) {
    vec3_copy(camera.pos, pos);

    vec3_sub(camera.dir, (vec3){0.0f, 0.0f, 0.0f}, camera.pos);
    vec3_normalize(camera.dir, camera.dir);

    vec3_cross(camera.right, camera.dir, camera.up);
    vec3_normalize(camera.right, camera.right);
}
