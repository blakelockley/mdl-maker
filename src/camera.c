#include <math.h>

#include "camera.h"
#include "linmath.h"
#include "macros.h"
#include "glfw.h"

extern GLFWwindow *window;

camera_t _camera;
camera_t *camera = &_camera;

void adjust_camera(float theta, float phi);
void derive_camera();

void init_camera() {
    vec3_zero(camera->origin);
    
    camera->theta = M_PI_2;
    camera->phi = M_PI_4;
    camera->radius = 2.0f;
    
    // zoom in degrees
    camera->zoom = CAMERA_ZOOM_INITIAL;
    
    derive_camera(camera);
}

void free_camera() {
    // no-op
}

// Getters

void get_view_matrix(mat4x4 m) { 
    vec3 ahead;
    vec3_add(ahead, camera->pos, camera->dir);

    mat4x4_look_at(m, camera->pos, ahead, camera->up);
}

void get_projection_matrix(mat4x4 m) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    
    float fov = CLAMP(camera->zoom, 1, 120) * (M_PI / 180); // convert to radians
    mat4x4_perspective(m, fov, (float)width / (float)height, 0.1f, 100.0f);
}

void get_view_projection_matrix(mat4x4 m) {
    mat4x4 view, projection;
    get_view_matrix(view);
    get_projection_matrix(projection);

    mat4x4_mul(m, projection, view);
}

void update_camera_origin(float delta_x, float delta_y) {
    
    vec3 xz;
    vec3_copy(xz, camera->dir);
    
    xz[1] = 0.0f;
    vec3_normalize(xz, xz);
    
    // xz
    vec3 right;
    vec3_cross(right, xz, camera->up);
    vec3_normalize(right, right);
    
    vec3 xz_vector;
    vec3_scale(xz_vector, right, delta_x * -0.01f);
    vec3_add(camera->origin, camera->origin, xz_vector);

    // y
    camera->origin[1] += delta_y * 0.01f;

    derive_camera(camera);
}

void update_camera_orbit(float delta_x, float delta_y) {    
    camera->phi += delta_y * 0.01f;
    camera->phi = CLAMP(camera->phi, -M_PI_2, M_PI_2);
    
    camera->theta += delta_x * 0.01f;

    derive_camera(camera);
}

void update_camera_radius(float delta) {
    camera->radius += delta * -0.1f;
    camera->radius = CLAMP(camera->radius, 0.1f, 10.0f);
    
    derive_camera(camera);
}

void adjust_camera(float theta, float phi) {
    camera->theta = theta;
    camera->phi = phi;

    derive_camera(camera);
}

void derive_camera() {
    float phi = camera->phi;
    float theta = camera->theta;

    if (sinf(phi) == 1.0f) {
        // top down
        
        // pos
        vec3_set(camera->pos, 0.0f, camera->radius, 0.0f);

        // up
        camera->up[0] = -cosf(theta);
        camera->up[1] = 0.0f;
        camera->up[2] = -sinf(theta);
    }

    if (sinf(phi) == -1.0f) {
        // bottom up

        // pos
        vec3_set(camera->pos, 0.0f, -camera->radius, 0.0f);

        // up
        camera->up[0] = cosf(theta);
        camera->up[1] = 0.0f;
        camera->up[2] = sinf(theta);
    }

    if (fabs(sinf(phi)) < 1.0f) {
        // orbital
        
        // pos.xz
        camera->pos[0] = camera->origin[0] + cosf(phi) * cosf(theta) * camera->radius;
        camera->pos[2] = camera->origin[2] + cosf(phi) * sinf(theta) * camera->radius;

        // pos.y
        camera->pos[1] = camera->origin[1] + sinf(phi) * camera->radius;

        // up
        vec3_set(camera->up, 0.0f, 1.0f, 0.0f);
    }

    // dir    
    vec3_sub(camera->dir, camera->origin, camera->pos);
    vec3_normalize(camera->dir, camera->dir);
}
