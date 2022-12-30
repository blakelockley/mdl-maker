#include "renderers.h"
#include "shader.h"
#include "camera.h"
#include "light.h"
#include "selection.h"

extern camera_t camera;
extern light_t light;

renderer_t * init_edge_renderer(renderer_t *renderer) {
    init_renderer(renderer, 1);
    
    renderer->shader = load_shader("shaders/static.vert", "shaders/static.frag");
    renderer->render = render_model_edges;

    // Positions
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);

    glBindVertexArray(0);

    return renderer;
}

void render_model_edges(renderer_t *renderer, model_t *model) {    
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
    get_projection_matrix(&camera, projection);
    
    GLint model_loc = glGetUniformLocation(renderer->shader, "model");
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, (float*)_model);

    GLint view_loc = glGetUniformLocation(renderer->shader, "view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, (float*)view);

    GLint projection_loc = glGetUniformLocation(renderer->shader, "projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float*)projection);
     
    GLint color_loc = glGetUniformLocation(renderer->shader, "color");
    glUniform3f(color_loc, 1.0f, 1.0f, 1.0f);
    
    glDrawArrays(GL_LINES, 0, total_vertices);
    glBindVertexArray(0);
}

void render_model_edges_selection(renderer_t *renderer, model_t *model, uint32_t *indices, uint32_t len) {
    uint32_t total_vertices = 0;
    for (int i = 0; i < len; i++)
        total_vertices += model->faces[indices[i]].len * 2; // n -> * 2
    
    vec3 positions[total_vertices];
    
    uint32_t vi = 0; // vertices index
    for (int i = 0; i < len; i++) {
        face_t* face = &(model->faces[indices[i]]);
        
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
    get_projection_matrix(&camera, projection);
    
    GLint model_loc = glGetUniformLocation(renderer->shader, "model");
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, (float*)_model);

    GLint view_loc = glGetUniformLocation(renderer->shader, "view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, (float*)view);

    GLint projection_loc = glGetUniformLocation(renderer->shader, "projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float*)projection);
     
    GLint color_loc = glGetUniformLocation(renderer->shader, "color");
    glUniform3f(color_loc, 0.20f, 0.92f, 0.34f);

    glDepthFunc(GL_ALWAYS);
    
    glDrawArrays(GL_LINES, 0, total_vertices);
    glBindVertexArray(0);
    
    glDepthFunc(GL_LEQUAL);
}
