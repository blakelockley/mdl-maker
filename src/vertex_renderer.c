#include "renderers.h"
#include "shader.h"
#include "camera.h"
#include "viewport.h"
#include "light.h"

extern camera_t camera;
extern viewport_t viewport;
extern light_t light;

renderer_t *init_vertex_renderer(renderer_t *renderer) {
    init_renderer(renderer, 1);
    
    renderer->shader = load_shader("shaders/static.vert", "shaders/static.frag");
    renderer->render = render_model_vertices;
    
    // Positions
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);

    glBindVertexArray(0);

    return renderer;
}

void render_model_vertices(renderer_t *renderer, model_t *model) {    
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

    glBindVertexArray(0);
}
