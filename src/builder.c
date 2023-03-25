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

extern struct ImGuiIO* io;

// Builder variables

bool is_visible = false;
int normal_axis = 0; // X

vec3 plane_normal, plane_origin;

uint32_t *indices;
uint32_t len;
uint32_t cap;

vec3 cursor_projection;

void init_builder() {
    // plane normal
    vec3_set(plane_normal, 0, 1, 0);
    vec3_normalize(plane_normal, plane_normal);

    // plane origin, origin relative to point p
    vec3_set(plane_origin, 0, 0.5, 0);
}

void free_builder() {
    // TODO: ...
}

void set_building_plane(vec3 origin, vec3 normal) {
    vec3_copy(plane_origin, origin);
    vec3_copy(plane_normal, normal);
}

void toggle_building_plane() {
    normal_axis = (normal_axis + 1) % 3;

    vec3_zero(plane_normal);
    plane_normal[normal_axis] = 1.0f;
}

void set_plane_to_cursor_face(float mouse_x, float mouse_y) {
   uint32_t picked_face_index = render_picker_to_face_id(&picker, &model);
    if (picked_face_index != INDEX_NOT_FOUND) {
        
        face_t *face = &model.faces[picked_face_index];
        
        vec3_copy(plane_origin, face->midpoint);
        vec3_copy(plane_normal, face->normal);
    }
}

bool project_cursor_onto_plane(vec3 r, float mouse_x, float mouse_y) {
    // ray origin, ray direction
    vec3 l0, l;

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
    bool intersects = intersect_plane(&t, plane_normal, plane_origin, l0, l);

    if (!intersects || t > 20.f)
        return false;

    point_intersect_plane(r, l0, l, t);
    return true;
}

void update_builder() {
    if (io->WantCaptureMouse)
        return;

    int action = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    
    double mouse_x, mouse_y;
    glfwGetCursorPos(window, &mouse_x, &mouse_y);

    set_plane_to_cursor_face(mouse_x, mouse_y);
    
    vec3 cursor;
    if (project_cursor_onto_plane(cursor, mouse_x, mouse_y))
        vec3_copy(cursor_projection, cursor);

    // if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
    // }

}

void gui_builder() {
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

    if (igBegin("Edit Verticess", NULL, window_flags)) {
        igText("Building Plane");

        igCheckbox("Visible", &is_visible);

        igInputFloat3("Origin", plane_origin, "%0.2f", 0);
        igInputFloat3("Normal", plane_normal, "%0.2f", 0);

        igEnd();
    }
}

void render_builder() {
    if (!is_visible)
        return;
    
    glDepthFunc(GL_LESS);

    vec4 color = {1.0f, 1.0f, 1.0f, 0.25f};
    render_control_plane(plane_origin, plane_normal, 2.0f, 2.0f, color);

    render_control_point(cursor_projection, 10.0f, (vec4){0.5, 0.5f, 0.5, 1.0f});
    
    glDepthFunc(GL_LEQUAL);
}

void build_vertex(float mouse_x, float mouse_y) {
    vec3 point;
    project_cursor_onto_plane(point, mouse_x, mouse_y);

    add_vertex(&model, point);
}
