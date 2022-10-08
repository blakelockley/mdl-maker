#include "normal_renderer.h"
#include "shader.h"
#include "camera.h"
#include "viewport.h"

extern camera_t camera;
extern viewport_t viewport;

void init_normal_renderer(normal_renderer_t *renderer) {
    renderer->shader = load_shader("shaders/static.vert", "shaders/static.frag");

    glGenVertexArrays(1, &renderer->vao);
    glBindVertexArray(renderer->vao);

    glGenBuffers(1, renderer->vbo);

    // Vertices
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);

    glBindVertexArray(0);
}

void free_normal_renderer(normal_renderer_t *renderer) {
    glDeleteVertexArrays(1, &renderer->vao);
    glDeleteBuffers(1, renderer->vbo);
}

void render_model_normals(normal_renderer_t *renderer, model_t *model) {
    vec3 vertices[model->faces_len * 2];
    
    for (int i = 0; i < model->faces_len; i++) {
        face_t *face = &model->faces[i];

        vec3 local_normal;
        vec3_scale(local_normal, face->normal, 0.1f);
        vec3_add(local_normal, face->midpoint, local_normal);

        vec3_copy(vertices[(i * 2) + 0], face->midpoint);
        vec3_copy(vertices[(i * 2) + 1], local_normal);
    }
    
    glBindVertexArray(renderer->vao);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glUseProgram(renderer->shader);
    
    mat4x4 _model, view, projection;
    mat4x4_identity(_model);
    get_view_matrix(&camera, view);
    get_projection_matrix(&viewport, projection);
    
    GLint model_loc = glGetUniformLocation(renderer->shader, "model");
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, (float*)_model);

    GLint view_loc = glGetUniformLocation(renderer->shader, "view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, (float*)view);

    GLint projection_loc = glGetUniformLocation(renderer->shader, "projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float*)projection);
     
    GLint color_loc = glGetUniformLocation(renderer->shader, "color");
    glUniform3f(color_loc, 1.0f, 1.0f, 1.0f);
    
    glDrawArrays(GL_LINES, 0, model->faces_len * 2);
    glBindVertexArray(0);
}
