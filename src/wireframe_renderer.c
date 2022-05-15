#include "wireframe_renderer.h"
#include "shader.h"
#include "camera.h"
#include "viewport.h"
#include "light.h"
#include "selection.h"

extern camera_t camera;
extern viewport_t viewport;
extern light_t light;
extern selection_t selection;

void init_wireframe_renderer(wireframe_renderer_t *renderer) {
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

void free_wireframe_renderer(wireframe_renderer_t *renderer) {
    glDeleteVertexArrays(1, &renderer->vao);
    glDeleteBuffers(1, renderer->vbo);
}

void render_model_wireframe(wireframe_renderer_t *renderer, model_t *model) {    
    uint32_t total_vertices = 0;
    for (int i = 0; i < model->faces_len; i++)
        total_vertices += model->faces[i].len * 2; // n -> * 2

    vec3 positions[total_vertices];
    
    uint32_t vi = 0; // vertices index
    for (int i = 0; i < model->faces_len; i++) {
        face_t* face = &(model->faces[i]);
        
        for (int j = 0; j < face->len; j++) {
            vec3_copy(positions[vi++], model->vertices[face->indices[j]]);
            vec3_copy(positions[vi++], model->vertices[face->indices[(j + 1) % face->len]]);
        }
    }

    glBindVertexArray(renderer->vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_DYNAMIC_DRAW);

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

    GLint light_pos_loc = glGetUniformLocation(renderer->shader, "light_pos");
    glUniform3fv(light_pos_loc, 1, (float*)light.pos);

    GLint light_color_loc = glGetUniformLocation(renderer->shader, "light_color");
    glUniform3fv(light_color_loc, 1, (float*)light.color);
     
    GLint color_loc = glGetUniformLocation(renderer->shader, "color");
    glUniform3f(color_loc, 0.1f, 0.2f, 1.0f);
    
    glDrawArrays(GL_LINES, 0, total_vertices);
}
