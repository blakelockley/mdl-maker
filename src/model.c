#include "model.h"

#include <stdlib.h>

void init_model(model_t* model) {
    model->vertices = (vec3*)malloc(sizeof(vec3) * 10);
    model->vertices_cap = 10;
    model->vertices_len = 0;

    glGenVertexArrays(1, &model->vao);
    glBindVertexArray(model->vao);

    // Edges
    glGenBuffers(1, &model->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t), NULL, GL_DYNAMIC_DRAW);

    // Vertices
    glGenBuffers(1, &model->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, model->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * 3, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);  // Attrib pointer for currently bound buffer
}

void add_vertex(model_t* model, vec3 vertex) {
    if (model->vertices_len == model->vertices_cap) {
        model->vertices_cap *= 2;
        model->vertices = (vec3*)realloc(model->vertices, sizeof(vec3) * model->vertices_cap);
    }

    vec3_set(model->vertices[model->vertices_len], vertex[0], vertex[1], vertex[2]);
    model->vertices_len++;

    uint32_t edges[model->vertices_len];
    for (int i = 0; i < model->vertices_len; i++)
        edges[i] = i;

    glBindVertexArray(model->vao);

    // Edges
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(edges), edges, GL_DYNAMIC_DRAW);

    // Vertices
    glBindBuffer(GL_ARRAY_BUFFER, model->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * model->vertices_len, model->vertices, GL_DYNAMIC_DRAW);
}

void draw_model(model_t* model, int shader) {
    GLint color_loc = glGetUniformLocation(shader, "color");
    glUniform3f(color_loc, 0.35f, 0.25f, 0.95f);

    glPointSize(10);

    glBindVertexArray(model->vao);
    glDrawElements(GL_TRIANGLES, model->vertices_len, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * 0));
}

void free_model(model_t* model) {
    free(model->vertices);

    glDeleteVertexArrays(1, &model->vao);
    glDeleteBuffers(1, &model->vbo);
    glDeleteBuffers(1, &model->ebo);
}
