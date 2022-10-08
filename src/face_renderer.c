#include "face_renderer.h"
#include "shader.h"
#include "camera.h"
#include "viewport.h"
#include "light.h"

extern camera_t camera;
extern viewport_t viewport;
extern light_t light;

void init_face_renderer(face_renderer_t *renderer) {
    renderer->shader = load_shader("shaders/face.vert", "shaders/face.frag");

    glGenVertexArrays(1, &renderer->vao);
    glBindVertexArray(renderer->vao);

    glGenBuffers(3, renderer->vbo);
    
    // Positions
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
    
    // Normals
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
    
    // Colors
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);

    glBindVertexArray(0);
}

void free_face_renderer(face_renderer_t *renderer) {
    glDeleteVertexArrays(1, &renderer->vao);
    glDeleteBuffers(3, renderer->vbo);
}

void render_model_faces(face_renderer_t *renderer, model_t *model) {
    uint32_t total_vertices = 0;
    for (int i = 0; i < model->faces_len; i++)
        total_vertices += (model->faces[i].len - 2) * 3; // n -> (n - 2) * 3

    vec3 positions[total_vertices];
    vec3 normals[total_vertices];
    vec3 colors[total_vertices];
    
    uint32_t vi = 0; // vertices index
    for (int i = 0; i < model->faces_len; i++) {
        for (int j = 2; j < model->faces[i].len; j++) {
            uint32_t *indices = model->faces[i].indices;

            vec3_copy(positions[vi], model->vertices[indices[0]]);
            vec3_copy(normals[vi], model->faces[i].normal);
            vec3_copy(colors[vi], model->faces[i].color);
            vi++;
            
            vec3_copy(positions[vi], model->vertices[indices[j - 1]]);
            vec3_copy(normals[vi], model->faces[i].normal);
            vec3_copy(colors[vi], model->faces[i].color);
            vi++;
            
            vec3_copy(positions[vi], model->vertices[indices[j - 0]]);
            vec3_copy(normals[vi], model->faces[i].normal);
            vec3_copy(colors[vi], model->faces[i].color);
            vi++;
        }
    }

    glBindVertexArray(renderer->vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_DYNAMIC_DRAW);

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
    
    glDrawArrays(GL_TRIANGLES, 0, total_vertices);
    glBindVertexArray(0);
}
