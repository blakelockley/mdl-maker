#include "camera.h"

#include <stdio.h>
#include <stdlib.h>

camera_t camera;
extern int width, height;

void update_camera_position();

void init_camera() {
    vec3_set(camera.scroll, 0.0f, 0.5f, 2.0f);
    vec3_set(camera.up, 0.0f, 1.0f, 0.0f);

    vec3_zero(camera.ray_start);
    vec3_zero(camera.ray);

    update_camera_position(camera);
}

void update_scroll(double xoffset, double yoffset) {
    camera.scroll[0] += xoffset;

    update_camera_position();
}

void update_zoom(double delta) {
    camera.scroll[2] += delta;
    camera.scroll[2] = fmin(fmax(camera.scroll[2], 0.5f), 10.0f);

    update_camera_position();
}

void update_camera_position() {
    camera.pos[0] = -sinf(camera.scroll[0]) * camera.scroll[2];
    camera.pos[1] = camera.scroll[1];
    camera.pos[2] = cosf(camera.scroll[0]) * camera.scroll[2];

    vec3_copy(camera.dir, (vec3){0.0f, 0.0f, 0.0f});
    camera.dir[1] = camera.pos[1];

    vec3 forward;
    vec3_sub(forward, camera.dir, camera.pos);
    vec3_normalize(forward, forward);

    mat4x4 rotate;
    mat4x4_identity(rotate);
    mat4x4_rotate_y(rotate, rotate, -M_PI / 2);

    vec4 right;
    vec4_from_vec3(right, forward, 0.0f);
    mat4x4_mul_vec4(right, rotate, right);

    vec3_copy(camera.right, right);
}

void set_ray(double mouse_x, double mouse_y, int width, int height) {
    double normal_x = (2.0f * mouse_x) / width - 1.0f;
    double normal_y = 1.0f - (2.0f * mouse_y) / height;

    vec4 ray_start = (vec4){normal_x, normal_y, -1.0f, 1.0f};
    vec4 ray_end = (vec4){normal_x, normal_y, 0.0f, 1.0f};

    mat4x4 view, projection;
    mat4x4_look_at(view, camera.pos, camera.dir, camera.up);
    mat4x4_perspective(projection, 45.0f, (float)width / (float)height, 0.1f, 100.0f);

    mat4x4_invert(view, view);
    mat4x4_invert(projection, projection);

    mat4x4_mul_vec4(ray_start, projection, ray_start);
    vec4_scale(ray_start, ray_start, 1.0f / ray_start[3]);

    mat4x4_mul_vec4(ray_start, view, ray_start);
    vec4_scale(ray_start, ray_start, 1.0f / ray_start[3]);

    mat4x4_mul_vec4(ray_end, projection, ray_end);
    vec4_scale(ray_end, ray_end, 1.0f / ray_end[3]);

    mat4x4_mul_vec4(ray_end, view, ray_end);
    vec4_scale(ray_end, ray_end, 1.0f / ray_end[3]);

    vec3 ray_dir;
    vec3_sub(ray_dir, ray_end, ray_start);
    vec3_normalize(ray_dir, ray_dir);

    vec3_copy(camera.ray_start, ray_start);
    vec3_copy(camera.ray, ray_dir);
}
