#include "mirror.h"
#include "renderers.h"

bool is_mirror_enabled = true;
bool is_mirror_visible = true;

float mirrored_faces_alpha = 0.25f;

int mirror_axis = 0;

void limit_mirror_source_vertex(vec3 v) {
    v[mirror_axis] = fmaxf(0, v[mirror_axis]);
}

void gui_mirror() {
    
    if (igBegin("Edit Verticess", NULL, 0)) {
        igText("Mirror");
        igCheckbox("Mirror Visible", &is_mirror_visible);
        igCheckbox("Mirror Enabled", &is_mirror_enabled);
        igSliderFloat("Mirrored Face Alpha", &mirrored_faces_alpha, 0.0f, 1.0f, "%0.2f", 0);
        igEnd();
    }
}

void render_mirror_plane() {
    if (!is_mirror_visible)
        return;
    
    vec3 normal;
    vec3_zero(normal);
    normal[mirror_axis] = 1.0f;

    render_control_plane((vec3){0.0f, 1.0f, 0.0f}, normal, 2.0f, 2.0f, (vec4){1.0f, 1.0f, 1.0f, 0.5f});
}

void render_mirror_faces(model_t *model) {
    if (!is_mirror_enabled)
        return;

    mat4x4 model_matrix;
    mat4x4_identity(model_matrix);
    mat4x4_scale(model_matrix, model_matrix, -1.0f, 1.0f, 1.0f);

    render_model_faces(model, model_matrix, mirrored_faces_alpha);
}