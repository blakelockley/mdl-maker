#include "model.h"

#include <stdlib.h>

void init_model(model_t* model) {
    model->vertices = (vec3*)malloc(sizeof(vec3) * 10);
    model->vertices_cap = 10;
    model->vertices_len = 0;

    model->indices = (uint32_t*)malloc(sizeof(uint32_t) * 10);
    model->indices_cap = 10;
    model->indices_len = 0;

    glGenVertexArrays(1, &model->vao);
    glBindVertexArray(model->vao);

    // Vertices
    glGenBuffers(1, &model->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, model->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * 3, NULL, GL_DYNAMIC_DRAW);

    // Indices
    glGenBuffers(1, &model->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t), NULL, GL_DYNAMIC_DRAW);

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

    // Vertices
    glBindBuffer(GL_ARRAY_BUFFER, model->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * model->vertices_len, model->vertices, GL_DYNAMIC_DRAW);
}

void move_vertex(model_t* model, int index, vec3 delta) {
    vec3_add(model->vertices[index], model->vertices[index], delta);

    // Vertices
    glBindBuffer(GL_ARRAY_BUFFER, model->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * model->vertices_len, model->vertices, GL_DYNAMIC_DRAW);
}

void add_face(model_t* model, int* index_buffer, int index_len) {
    for (int i = 0; i < index_len; i++) {
        if (model->indices_len == model->indices_cap) {
            model->indices_cap *= 2;
            model->indices = (uint32_t*)realloc(model->indices, sizeof(uint32_t) * model->indices_cap);
        }

        model->indices[model->indices_len++] = index_buffer[i];
    }

    // Indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * model->indices_len, model->indices, GL_DYNAMIC_DRAW);
}

void draw_model(model_t* model, int* indices, int index_len, int shader) {
    GLint color_loc = glGetUniformLocation(shader, "color");
    glUniform3f(color_loc, 0.35f, 0.25f, 0.95f);

    glBindVertexArray(model->vao);
    glDrawElements(GL_TRIANGLES, model->indices_len, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * 0));

    glPointSize(20);

    // if (0 <= current_index && current_index < model->vertices_len) {
    //     glUniform3f(color_loc, 0.0f, 1.0f, 0.0f);
    //     glDrawElements(GL_POINTS, 1, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * current_index));
    // }

    for (int i = 0; i < index_len; i++) {
        glUniform3f(color_loc, 0.0f, 1.0f, 0.0f);
        glDrawArrays(GL_POINTS, indices[i], 1);
    }

    glUniform3f(color_loc, 1.0f, 0.75f, 0.5f);
    glDrawArrays(GL_POINTS, 0, model->vertices_len);
}

void free_model(model_t* model) {
    free(model->vertices);

    glDeleteVertexArrays(1, &model->vao);
    glDeleteBuffers(1, &model->vbo);
    glDeleteBuffers(1, &model->ebo);
}

int find_intercept(model_t* model, camera_t* camera) {
    vec3 ray_start, ray_dir;
    vec3_copy(ray_start, camera->ray_start);
    vec3_copy(ray_dir, camera->ray);

    for (float t = 0.0f; t < 10.0f; t += 0.01f) {
        vec3 point;
        vec3_scale(point, ray_dir, t);
        vec3_add(point, point, ray_start);

        for (int i = 0; i < model->vertices_len; i++) {
            vec3 vertex, tmp;
            vec3_copy(vertex, model->vertices[i]);

            float dist;
            vec3_sub(tmp, point, vertex);
            dist = vec3_len(tmp);

            if (dist < 0.025f)
                return i;
        }
    }

    return -1;
}