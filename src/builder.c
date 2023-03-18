#include "builder.h"
#include "camera.h"
#include "model.h"
#include "utils.h"
#include "renderers.h"
#include "picker.h"
#include "glfw.h"

extern GLFWwindow *window;

extern camera_t camera;
extern model_t model;

extern picker_t picker;

int normal_axis = 1; // Y

// fill out variables
vec3 n, p0, l0, l;

bool is_visible = true;


void init_builder() {
    // plane normal
    vec3_set(n, 0, 1, 0);
    vec3_normalize(n, n);

    // plane origin, origin relative to point p
    vec3_set(p0, 0, 0.5, 0);
}

void free_builder() {
    // TODO: ...
}

void set_building_plane(vec3 origin, vec3 normal) {
    vec3_copy(p0, origin);
    vec3_copy(n, normal);
}

void toggle_building_plane() {
    normal_axis = (normal_axis + 1) % 3;

    vec3_zero(n);
    n[normal_axis] = 1.0f;
}

void update_builder() {
    vec3_copy(p0, camera.origin);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;

    const ImGuiViewport* viewport = igGetMainViewport();
    ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
    ImVec2 work_size = viewport->WorkSize;
    ImVec2 window_pos, window_pos_pivot;
    window_pos.x = work_pos.x + work_size.x - 10.0f;
    window_pos.y = work_pos.y + 10.0f;
    window_pos_pivot.x = 1.0f;
    window_pos_pivot.y = 0.0f;
    igSetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
    igSetNextWindowBgAlpha(0.35f); // Transparent background

    if (!igBegin("Edit Verticess", NULL, window_flags))
        return;
    
    igText("Building Plane");

    igCheckbox("Visible", &is_visible);

    igInputFloat3("Origin", p0, "%0.2f", 0);
    igInputFloat3("Normal", n, "%0.2f", 0);

    igEnd();
}

void render_builder() {
    if (!is_visible)
        return;
    
    glDepthFunc(GL_LESS);

    vec4 color = {1.0f, 1.0f, 1.0f, 0.25f};
    render_control_plane(p0, n, 2.0f, 2.0f, color);
    
    glDepthFunc(GL_LEQUAL);
}

void build_vertex(float mouse_x, float mouse_y) {
    uint32_t picked_face_index = render_picker_to_face_id(&picker, &model);
    if (picked_face_index != INDEX_NOT_FOUND) {
        
        face_t *face = &model.faces[picked_face_index];
        
        vec3_copy(p0, face->midpoint);
        vec3_copy(n, face->normal);
    }

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

    if (!intersects || t > 20.f)
        return;

    vec3 point;
    point_intersect_plane(point, l0, l, t);

    add_vertex(&model, point);
}
