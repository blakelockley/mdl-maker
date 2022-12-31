#include "renderers.h"
#include "shader.h"
#include "camera.h"

extern camera_t camera;

renderer_t *init_normal_renderer(renderer_t *renderer) {
    init_renderer(renderer, 1);
    
    renderer->shader = load_shader("shaders/static.vert", "shaders/static.frag");
    renderer->render = render_model_normals;

    // Vertices
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);

    glBindVertexArray(0);

    return renderer;
}

void render_model_normals(renderer_t *renderer, model_t *model) {
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
    
    mat4x4 mvp;
    get_view_projection_matrix(&camera, mvp);
    
    GLint mvp_loc = glGetUniformLocation(renderer->shader, "mvp");
    glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, (float*)mvp);
     
    GLint color_loc = glGetUniformLocation(renderer->shader, "color");
    glUniform3f(color_loc, 1.0f, 1.0f, 1.0f);
    
    glDrawArrays(GL_LINES, 0, model->faces_len * 2);
    glBindVertexArray(0);
}
