#include "mirror.h"
#include "renderers.h"

bool is_mirror_enabled = true;

float mirrored_faces_alpha = 0.25f;

void gui_mirror() {
    if (igBegin("Edit Verticess", NULL, 0)) {
        if (igCollapsingHeader_BoolPtr("Mirror", NULL, 0)) {
            igSliderFloat("Mirrored Face Alpha", &mirrored_faces_alpha, 0.0f, 1.0f, "%0.2f", 0);
        }
        igEnd();
    }
}

void render_mirror_plane() {

}

void render_mirror_faces(model_t *model) {
    if (!is_mirror_enabled)
        return;

    mat4x4 model_matrix;
    mat4x4_identity(model_matrix);
    mat4x4_scale(model_matrix, model_matrix, -1.0f, 1.0f, 1.0f);

    render_model_faces(model, model_matrix, mirrored_faces_alpha);
}