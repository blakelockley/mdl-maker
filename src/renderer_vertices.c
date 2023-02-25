#include "renderers.h"
#include "shader.h"
#include "camera.h"
#include "light.h"

extern camera_t camera;
extern light_t light;

static renderer_t _renderer;
static renderer_t *renderer = &_renderer;

renderer_t *init_vertex_renderer() {
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

void deinit_vertex_renderer() {
    deinit_renderer(renderer);
}

void render_model_vertices(model_t *model) {    
    glBindVertexArray(renderer->vao);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * model->vertices_len, model->vertices, GL_DYNAMIC_DRAW);

    glUseProgram(renderer->shader);
    
    mat4x4 mvp;
    get_view_projection_matrix(&camera, mvp);
    
    GLint mvp_loc = glGetUniformLocation(renderer->shader, "mvp");
    glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, (float*)mvp);
     
    GLint color_loc = glGetUniformLocation(renderer->shader, "color");
    glUniform4f(color_loc, 1.0f, 1.0f, 1.0f, 1.0f);
    
    glBindVertexArray(renderer->vao);

    glPointSize(10);
    glDrawArrays(GL_POINTS, 0, model->vertices_len);

    glBindVertexArray(0);
}

void render_model_vertices_selection(model_t *model, uint32_t *indices, uint32_t len) {
    vec3 positions[len];
    for (int i = 0; i < len; i++)
        vec3_copy(positions[i], model->vertices[indices[i]]);

    glBindVertexArray(renderer->vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_DYNAMIC_DRAW);

    glUseProgram(renderer->shader);
    
    mat4x4 mvp;
    get_view_projection_matrix(&camera, mvp);
    
    GLint mvp_loc = glGetUniformLocation(renderer->shader, "mvp");
    glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, (float*)mvp);
     
    GLint color_loc = glGetUniformLocation(renderer->shader, "color");
    glUniform4f(color_loc, 0.20f, 0.92f, 0.34f, 1.0f);

    glDepthFunc(GL_ALWAYS);
    glBindVertexArray(renderer->vao);
    
    glPointSize(10);
    glDrawArrays(GL_POINTS, 0, len);

    glBindVertexArray(0);

    glDepthFunc(GL_LEQUAL);
}
