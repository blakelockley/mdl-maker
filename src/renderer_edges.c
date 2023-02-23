#include "renderers.h"
#include "shader.h"
#include "camera.h"
#include "light.h"
#include "selection.h"

extern camera_t camera;
extern light_t light;

static renderer_t _renderer;
static renderer_t *renderer = &_renderer;

renderer_t * init_edge_renderer() {
    init_renderer(renderer, 1);

    // Positions
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);

    glBindVertexArray(0);

    renderer->shader = load_shader("shaders/static.vert", "shaders/static.frag");
    return renderer;
}

void deinit_edge_renderer() {
    deinit_renderer(renderer);
}

void render_model_edges(model_t *model) {    
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

    mat4x4 mvp;
    get_view_projection_matrix(&camera, mvp);
    
    GLint mvp_loc = glGetUniformLocation(renderer->shader, "mvp");
    glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, (float*)mvp);    
     
    GLint color_loc = glGetUniformLocation(renderer->shader, "color");
    glUniform3f(color_loc, 1.0f, 1.0f, 1.0f);
    
    glDrawArrays(GL_LINES, 0, total_vertices);
    glBindVertexArray(0);
}

void render_model_edges_selection(model_t *model, uint32_t *indices, uint32_t len) {
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
    
    mat4x4 mvp;
    get_view_projection_matrix(&camera, mvp);
    
    GLint mvp_loc = glGetUniformLocation(renderer->shader, "mvp");
    glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, (float*)mvp);
     
    GLint color_loc = glGetUniformLocation(renderer->shader, "color");
    glUniform3f(color_loc, 0.20f, 0.92f, 0.34f);

    glDepthFunc(GL_ALWAYS);
    
    glDrawArrays(GL_LINES, 0, total_vertices);
    glBindVertexArray(0);
    
    glDepthFunc(GL_LEQUAL);
}
