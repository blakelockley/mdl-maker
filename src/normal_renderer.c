#include "normal_renderer.h"
#include "shader.h"
#include "camera.h"
#include "viewport.h"

extern camera_t camera;
extern viewport_t viewport;
normal_renderer_t normal_renderer;

void init_normal_renderer() {
    normal_renderer.shader = load_shader("shaders/static.vert", "shaders/static.frag");

    glGenVertexArrays(1, &normal_renderer.vao);
    glBindVertexArray(normal_renderer.vao);

    // Vertices
    glGenBuffers(1, &normal_renderer.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, normal_renderer.vbo);
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);  // Positions
}

void render_model_normals(model_t *model) {
    vec3 vertices[model->faces_len * 2];
    
    for (int i = 0; i < model->faces_len; i++) {
        face_t *face = &model->faces[i];

        vec3 local_normal;
        vec3_scale(local_normal, face->normal, 0.1f);
        vec3_add(local_normal, face->midpoint, local_normal);

        vec3_copy(vertices[(i * 2) + 0], face->midpoint);
        vec3_copy(vertices[(i * 2) + 1], local_normal);
    }
    
    glBindVertexArray(normal_renderer.vao);
    glBindBuffer(GL_ARRAY_BUFFER, normal_renderer.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glUseProgram(normal_renderer.shader);
    
    mat4x4 _model, view, projection;
    mat4x4_identity(_model);
    get_view_matrix(&camera, view);
    get_projection_matrix(&viewport, projection);
    
    GLint model_loc = glGetUniformLocation(normal_renderer.shader, "model");
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, (float*)_model);

    GLint view_loc = glGetUniformLocation(normal_renderer.shader, "view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, (float*)view);

    GLint projection_loc = glGetUniformLocation(normal_renderer.shader, "projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float*)projection);
     
    GLint color_loc = glGetUniformLocation(normal_renderer.shader, "color");
    
    glUniform3f(color_loc, 0.1f, 1.0f, 0.2f);
    glDrawArrays(GL_LINES, 0, model->faces_len * 2);
}
