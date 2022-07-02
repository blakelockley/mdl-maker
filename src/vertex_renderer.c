#include "vertex_renderer.h"
#include "shader.h"
#include "camera.h"
#include "viewport.h"
#include "light.h"
#include "selection.h"

extern camera_t camera;
extern viewport_t viewport;
extern light_t light;
extern selection_t selection;

void init_vertex_renderer(vertex_renderer_t *renderer) {
    renderer->shader = load_shader("shaders/static.vert", "shaders/static.frag");

    glGenVertexArrays(1, &renderer->vao);
    glBindVertexArray(renderer->vao);

    glGenBuffers(2, renderer->vbo);
    
    // Positions
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
}

void free_vertex_renderer(vertex_renderer_t *renderer) {
    glDeleteVertexArrays(1, &renderer->vao);
    glDeleteBuffers(1, renderer->vbo);
}

void render_model_vertices(vertex_renderer_t *renderer, model_t *model) {    
    glBindVertexArray(renderer->vao);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * model->vertices_len, model->vertices, GL_DYNAMIC_DRAW);

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
    
    glBindVertexArray(renderer->vao);
    glPointSize(10);

    glUniform3f(color_loc, 1.0f, 1.0f, 1.0f);
    glDrawArrays(GL_POINTS, 0, model->vertices_len);
    
    glUniform3f(color_loc, 0.0f, 1.0f, 0.0f);
    for (int i = 0; i < selection.len; i++)
        glDrawArrays(GL_POINTS, selection.indices[i], 1);
}
