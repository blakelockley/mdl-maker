#include "model.h"

#include <stdio.h>
#include <stdlib.h>

#include "camera.h"
#include "viewport.h"
#include "shader.h"
#include "select.h"
#include "face_renderer.h"

model_t model;
extern select_t select;

void init_model() {
    vec3_set(model.color, 0.25f, 0.45f, 1.0f);

    model.vertices = (vec3*)malloc(sizeof(vec3) * 10);
    model.vertices_cap = 10;
    model.vertices_len = 0;

    model.faces = (face_t*)malloc(sizeof(face_t) * 10);
    model.faces_cap = 10;
    model.faces_len = 0;

    // Position VAO, used to display model vertices as points

    glGenVertexArrays(1, &model.pos_vao);
    glBindVertexArray(model.pos_vao);

    // vertices
    glGenBuffers(1, &model.pos_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, model.pos_vbo);
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);  // vertices

    model.pos_shader = load_shader("shaders/static.vert", "shaders/static.frag");
}

void free_model() {
    free(model.vertices);

    glDeleteVertexArrays(1, &model.pos_vao);
    glDeleteBuffers(1, &model.pos_vbo);
}

// Update data methods

void add_vertex(vec3 vertex) {
    if (model.vertices_len == model.vertices_cap) {
        model.vertices_cap *= 2;
        model.vertices = (vec3*)realloc(model.vertices, sizeof(vec3) * model.vertices_cap);
    }

    vec3_copy(model.vertices[model.vertices_len++], vertex);
}

void add_face() {
    if (select.selection_len < 3)
        return;

    if (model.faces_len == model.faces_cap) {
        model.faces_cap *= 2;
        model.faces = (face_t*)realloc(model.faces, sizeof(face_t) * model.faces_cap);
    }

    face_t face;
    face.len = select.selection_len;
    face.indices = (uint32_t*)malloc(sizeof(uint32_t) * face.len);

    for (int i = 0; i < select.selection_len; i++)
        face.indices[i] = select.selection_buffer[i];

    model.faces[model.faces_len++] = face;
}

void draw_model() {
    glBindVertexArray(model.pos_vao);
    glBindBuffer(GL_ARRAY_BUFFER, model.pos_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * model.vertices_len, model.vertices, GL_DYNAMIC_DRAW);

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
    glDrawArrays(GL_POINTS, 0, model.vertices_len);

    render_model_faces(&model);
}
