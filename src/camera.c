#include <math.h>

#include "camera.h"
#include "linmath.h"
#include "macros.h"
#include "glfw.h"

extern GLFWwindow *window;

void adjust_camera(camera_t *camera, float theta, float phi);
void derive_camera(camera_t *camera);

void init_camera(camera_t *camera) {
    vec3_zero(camera->origin);
    
    camera->theta = M_PI_2;
    camera->phi = M_PI_4;
    camera->radius = 2.0f;
    
    // zoom in degrees
    camera->zoom = CAMERA_ZOOM_INITIAL;
    
    derive_camera(camera);
}

void free_camera(camera_t *camera) {
    // no-op
}

void show_camera_gui(camera_t *camera) {
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;

    const ImGuiViewport* viewport = igGetMainViewport();
    ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
    ImVec2 window_pos, window_pos_pivot;
    window_pos.x = work_pos.x + 10.0f;
    window_pos.y = work_pos.y + 10.0f;
    window_pos_pivot.x = 0.0f;
    window_pos_pivot.y = 0.0f;
    igSetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
    igSetNextWindowBgAlpha(0.35f); // Transparent background

    static char *cardinal;
    if (fabs(camera->dir[0]) > fabs(camera->dir[2])) {
        if (camera->dir[0] > 0.0f)
            cardinal = "East (+X)";
        else
            cardinal = "West (-X)";
    } else {
        if (camera->dir[2] > 0.0f)
            cardinal = "North (+Z)";
        else
            cardinal = "South (-Z)";
    }

    static char *vertical;
    if (camera->dir[1] > 0.0f)
        vertical = "Up (+Y)";
    else
        vertical = "Down (-Y)";

    bool horizontal = fmaxf(fabs(camera->dir[0]), fabs(camera->dir[2])) >= fabs(camera->dir[1]);
    
    if (igBegin("Camera", NULL, window_flags)) {
        igText("Camera");
        
        igText("Origin:  %+.2f, %+.2f, %+.2f", camera->origin[0], camera->origin[1], camera->origin[2]);
        
        igText("Theta:  %+.2f", camera->theta);
        igText("Phi:    %+.2f", camera->phi);
        igText("Radius: %+.2f", camera->radius);
        
        igText("Set Orbit");
        if (igButton("X+", (ImVec2){0, 0}))
            adjust_camera(camera, 0.0f, 0.0f);

        igSameLine(0, 10);
        if (igButton("X-", (ImVec2){0, 0}))
            adjust_camera(camera, M_PI, 0.0f);

        igSameLine(0, 20);
        if (igButton("Z+", (ImVec2){0, 0}))
            adjust_camera(camera, M_PI_2, 0.0f);

        igSameLine(0, 10);
        if (igButton("Z-", (ImVec2){0, 0}))
            adjust_camera(camera, M_PI + M_PI_2, 0.0f);

        igSameLine(0, 20);
        if (igButton("Y+", (ImVec2){0, 0}))
            adjust_camera(camera, 0.0f, M_PI_2);

        igSameLine(0, 10);
        if (igButton("Y-", (ImVec2){0, 0}))
            adjust_camera(camera, 0.0f, -M_PI_2);
        
        igSeparator();
        
        igText("Position:  %+.2f, %+.2f, %+.2f", camera->pos[0], camera->pos[1], camera->pos[2]);
        igText("Direction: %+.2f, %+.2f, %+.2f", camera->dir[0], camera->dir[1], camera->dir[2]);
        igText("Up:        %+.2f, %+.2f, %+.2f", camera->up[0], camera->up[1], camera->up[2]);
        
        igSeparator();

        igText("Heading");
        igText("Cardinal: %s%s", cardinal, horizontal ? "*" : "");
        igText("Vertical: %s%s", vertical, horizontal ? "" : "*");
        
        igEnd();
    }
}

// Getters

void get_view_matrix(camera_t *camera, mat4x4 m) { 
    vec3 ahead;
    vec3_add(ahead, camera->pos, camera->dir);

    mat4x4_look_at(m, camera->pos, ahead, camera->up);
}

void get_projection_matrix(camera_t *camera, mat4x4 m) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    
    float fov = CLAMP(camera->zoom, 1, 120) * (M_PI / 180); // convert to radians
    mat4x4_perspective(m, fov, (float)width / (float)height, 0.1f, 100.0f);
}

void get_view_projection_matrix(camera_t *camera, mat4x4 m) {
    mat4x4 view, projection;
    get_view_matrix(camera, view);
    get_projection_matrix(camera, projection);

    mat4x4_mul(m, projection, view);
}

void update_origin(camera_t *camera, float delta_x, float delta_y) {
    
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

void update_orbit(camera_t *camera, float delta_x, float delta_y) {    
    camera->phi += delta_y * 0.01f;
    camera->phi = CLAMP(camera->phi, -M_PI_2, M_PI_2);
    
    camera->theta += delta_x * 0.01f;

    derive_camera(camera);
}

void update_radius(camera_t *camera, float delta) {
    camera->radius += delta * -0.1f;
    camera->radius = CLAMP(camera->radius, 0.1f, 10.0f);
    
    derive_camera(camera);
}

void adjust_camera(camera_t *camera, float theta, float phi) {
    camera->theta = theta;
    camera->phi = phi;

    derive_camera(camera);
}

void derive_camera(camera_t *camera) {
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
