#include "model.h"

#include <stdio.h>
#include <stdlib.h>

#include "camera.h"
#include "viewport.h"
#include "shader.h"
#include "select.h"

model_t model;
extern select_t select;

void init_model() {
    vec3_set(model.color, 0.25f, 0.45f, 1.0f);

    model.positions = (vec3*)malloc(sizeof(vec3) * 10);
    model.positions_cap = 10;
    model.positions_len = 0;

    // Position VAO, used to display model positions as points

    glGenVertexArrays(1, &model.pos_vao);
    glBindVertexArray(model.pos_vao);

    // Positions
    glGenBuffers(1, &model.pos_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, model.pos_vbo);
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);  // Positions

    model.pos_shader = load_shader("shaders/static.vert", "shaders/static.frag");
}

void free_model() {
    free(model.positions);

    glDeleteVertexArrays(1, &model.pos_vao);
    glDeleteBuffers(1, &model.pos_vbo);
}

// Update data methods

void add_position(vec3 position) {
    if (model.positions_len == model.positions_cap) {
        model.positions_cap *= 2;
        model.positions = (vec3*)realloc(model.positions, sizeof(vec3) * model.positions_cap);
    }

    vec3_copy(model.positions[model.positions_len++], position);

    glBindVertexArray(model.pos_vao);
    glBindBuffer(GL_ARRAY_BUFFER, model.pos_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * model.positions_len, model.positions, GL_DYNAMIC_DRAW);
}

void draw_model() {
    glUseProgram(model.pos_shader);
    
    mat4x4 _model, view, projection;
    mat4x4_identity(_model);
    get_view_matrix(view);
    get_projection_matrix(projection);
    
    GLint model_loc = glGetUniformLocation(model.pos_shader, "model");
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, (float*)_model);

    GLint view_loc = glGetUniformLocation(model.pos_shader, "view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, (float*)view);

    GLint projection_loc = glGetUniformLocation(model.pos_shader, "projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float*)projection);
     
    GLint color_loc = glGetUniformLocation(model.pos_shader, "color");
    
    glPointSize(10);

    glBindVertexArray(model.pos_vao);

    glUniform3f(color_loc, 0.0f, 1.0f, 0.0f);
    for (int i = 0; i < select.selection_len; i++) {
        int index = select.selection_buffer[i];
        glDrawArrays(GL_POINTS, index, 1);
    }

    glUniform3f(color_loc, 1.0f, 1.0f, 1.0f);
    glDrawArrays(GL_POINTS, 0, model.positions_len);
}
