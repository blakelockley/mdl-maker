#include "builder.h"
#include "camera.h"
#include "model.h"

extern GLFWwindow *window;

extern camera_t camera;
extern model_t model;

void init_builder() {
    // TODO: ...
}

void free_builder() {
    // TODO: ...
}

void build_vertex(float mouse_x, float mouse_y) {
    vec3 origin = { 0.0f, 0.0f, 0.0f };

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    mat4x4 mvp, inverse_mvp;
    get_view_projection_matrix(&camera, mvp);
    mat4x4_invert(inverse_mvp, mvp);

    vec3 midpoint;
    mat4x4_mul_vec3(midpoint, mvp, origin);

    float clip_x = (mouse_x / width) * 2.0f - 1.0f;
    float clip_y = 1.0f - (mouse_y / height) * 2.0f;
    float clip_z = midpoint[2];

    vec3 clip;
    vec3_set(clip, clip_x, clip_y, clip_z);

    vec3 plane_pos;
    mat4x4_mul_vec3(plane_pos, inverse_mvp, clip);
    
    add_vertex(&model, plane_pos);
}
