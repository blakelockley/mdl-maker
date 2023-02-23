#include "renderers.h"
#include "shader.h"
#include "camera.h"
#include "light.h"

extern camera_t camera;
extern light_t light;

bool is_mirror_enabled = true;
float mirror_alpha = 0.25f;

static renderer_t _renderer;
static renderer_t *renderer = &_renderer;

renderer_t *init_face_renderer() {
    init_renderer(renderer, 3);
    
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

    renderer->shader = load_shader("shaders/face.vert", "shaders/face.frag");
    return renderer;
}

void deinit_face_renderer() {
    deinit_renderer(renderer);
}


void render_model_faces(model_t *model) {
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
    
    glUseProgram(renderer->shader);
    
    mat4x4 mvp;
    get_view_projection_matrix(&camera, mvp);
    
    GLint mvp_loc = glGetUniformLocation(renderer->shader, "mvp");
    glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, (float*)mvp);

    GLint light_pos_loc = glGetUniformLocation(renderer->shader, "light_pos");
    glUniform3fv(light_pos_loc, 1, (float*)light.pos);

    GLint light_color_loc = glGetUniformLocation(renderer->shader, "light_color");
    glUniform3fv(light_color_loc, 1, (float*)light.color);

    GLint filter_color_loc = glGetUniformLocation(renderer->shader, "filter_color");
    glUniform4fv(filter_color_loc, 1, (float[]){1.0f, 1.0f, 1.0f, 1.0f});

    glBindVertexArray(renderer->vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_DYNAMIC_DRAW);

    glDrawArrays(GL_TRIANGLES, 0, total_vertices);

    if (is_mirror_enabled) {
        mat4x4 scale;
        mat4x4_identity(scale);
        mat4x4_scale(scale, scale, -1.0f, 1.0f, 1.0f);

        mat4x4_mul(mvp, mvp, scale);
        glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, (float*)mvp);

        glUniform4fv(filter_color_loc, 1, (float[]){1.0f, 1.0f, 1.0f, mirror_alpha});

        glDrawArrays(GL_TRIANGLES, 0, total_vertices);
    }
    
    glBindVertexArray(0);
}
