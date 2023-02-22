#include "builder.h"
#include "camera.h"
#include "model.h"
#include "utils.h"
#include "renderers.h"

extern GLFWwindow *window;

extern camera_t camera;
extern model_t model;

// fill out variables
vec3 n, p0, l0, l;

void init_builder() {
    // TODO: ...
}

void free_builder() {
    // TODO: ...
}

void render_builder() {
    vec4 color;
    vec4_set(color, 0.5f, 0.5f, 0.5f, 0.5f);

    render_control_plane(p0, n, 2.0f, 2.0f, color);
    
    vec3 p0n;
    vec3_add(p0n, p0, n);
    
    render_control_line(p0, p0n, color);
}

void build_vertex(float mouse_x, float mouse_y) {
    // plane normal
    vec3_set(n, 0, 1, 0);
    vec3_normalize(n, n);

    // plane origin, origin relative to point p
    vec3_set(p0, 0, 0.5, 0);
    
    // ray origin l0 is at camera
    vec3_copy(l0, camera.pos);
    
    // ray direction l

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    mat4x4 mvp, inverse_mvp;
    get_view_projection_matrix(&camera, mvp);
    mat4x4_invert(inverse_mvp, mvp);

    vec3 clip;
    clip[0] = (mouse_x / width) * 2.0f - 1.0f;
    clip[1] = 1.0f - (mouse_y / height) * 2.0f;
    clip[2] = 0.0f; // near plane

    vec3 near_pos;
    mat4x4_mul_vec3(near_pos, inverse_mvp, clip);

    vec3_sub(l, near_pos, l0);
    vec3_normalize(l, l);

    // find intersection

    float t; // written to by intersect_plane
    bool intersects = intersect_plane(&t, n, p0, l0, l);

    if (!intersects)
        return;

    vec3 point;
    point_intersect_plane(point, l0, l, t);
    
    add_vertex(&model, point);
}
