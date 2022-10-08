#include <stdlib.h>
#include "model.h"

// Model

void init_model(model_t *model) {
    model->vertices = (vec3*)malloc(sizeof(vec3) * 1024);
    model->vertices_cap = 1024;
    model->vertices_len = 0;

    model->faces = (face_t*)malloc(sizeof(face_t) * 1024);
    model->faces_cap = 1024;
    model->faces_len = 0;

}

void free_model(model_t *model) {
    for (int i = model->faces_len - 1; i >= 0; i--)
        remove_face(model, i);
    
    free(model->vertices);
    free(model->faces);
}

// Vertices

uint32_t add_vertex(model_t *model, vec3 vertex) {
    if (model->vertices_len == model->vertices_cap) {
        model->vertices_cap *= 2;
        model->vertices = (vec3*)realloc(model->vertices, sizeof(vec3) * model->vertices_cap);
    }

    uint32_t index = model->vertices_len;
    vec3_copy(model->vertices[index], vertex);
    
    model->vertices_len++;

    return index;
}

void update_vertex(model_t *model, uint32_t index, vec3 vertex) {
    vec3_copy(model->vertices[index], vertex);
}

void remove_vertex(model_t *model, uint32_t index) {
    for (int i = index; i < model->vertices_len - 1; i++)
        vec3_copy(model->vertices[i],  model->vertices[i + 1]);

    model->vertices_len--;
}

// Face

uint32_t add_face(model_t *model, uint32_t *indices, uint32_t len) {
    if (len < 3 || len > 12)
        return INDEX_NOT_FOUND;

    if (!check_coplanar_vertices(model, indices, len)) {
        printf("[ERROR]: Face vertices are not coplanar.\n");
        return INDEX_NOT_FOUND;
    }

    if (model->faces_len == model->faces_cap) {
        model->faces_cap *= 2;
        model->faces = (face_t*)realloc(model->faces, sizeof(face_t) * model->faces_cap);
    }

    uint32_t index = model->faces_len++;

    face_t *face = &model->faces[index];
    face->len = len;
    
    face->indices = (uint32_t*)malloc(sizeof(uint32_t) * len);
    memcpy(face->indices, indices, sizeof(uint32_t) * len);

    calculate_midpoint(model, face->midpoint, indices, len);
    calculate_normal(model, face->normal, indices, len);
    
    vec3_copy(face->color, (vec3){0.25f, 0.25f, 0.25f});

    return index;
}

uint32_t add_face_tri(model_t *model, uint32_t ia, uint32_t ib, uint32_t ic) {
    uint32_t indices[3] = { ia, ib, ic };
    return add_face(model, indices, 3);
}

uint32_t add_face_quad(model_t *model, uint32_t ia, uint32_t ib, uint32_t ic, uint32_t id) {
    uint32_t indices[4] = { ia, ib, ic, id };
    return add_face(model, indices, 4);
}

void flip_face(model_t *model, uint32_t index) {
    face_t *face = &model->faces[index];

    uint32_t reverse_indices[face->len];
    for (int k = 0; k < face->len; k++)
        reverse_indices[k] = face->indices[face->len - k - 1];

    memcpy(face->indices, reverse_indices, sizeof(reverse_indices));
    calculate_normal(model, face->normal, face->indices, face->len);
}

void set_face_color(model_t *model, uint32_t index, vec3 color) {
    vec3_copy(model->vertices[index], color);
}

void remove_face(model_t *model, uint32_t index) {
    free(model->faces[index].indices);

    for (int i = index; i < model->faces_len - 1; i++)
        model->faces[i] = model->faces[i + 1];

    model->faces_len--;
}

// Helper

void calculate_normal(model_t *model, vec3 normal, uint32_t *indices, uint32_t len) {
    if (len < 3)
        return;

    vec3 a, b, c;
    vec3_copy(a, model->vertices[indices[0]]);
    vec3_copy(b, model->vertices[indices[1]]);
    vec3_copy(c, model->vertices[indices[2]]);

    // TODO: Check cosine of vertices to ensure proper triangle?

    vec3 ab, ac;
    vec3_sub(ab, b, a);
    vec3_sub(ac, c, a);

    vec3_cross(normal, ab, ac);
    vec3_normalize(normal, normal);
}

void calculate_midpoint(model_t *model, vec3 midpoint, uint32_t *indices, uint32_t len) {
    if (len < 1)
        return;
    
    vec3_zero(midpoint);
    for (int i = 0; i < len; i++)
        vec3_add(midpoint, midpoint, model->vertices[indices[i]]);
    vec3_scale(midpoint, midpoint, 1.0f / (float)len);
}

void recalculate_faces(model_t *model) {
    for (int i = 0; i < model->faces_len; i++) {
        face_t *face = &model->faces[i];
    
        calculate_normal(model, face->normal, face->indices, face->len);
        calculate_midpoint(model, face->midpoint, face->indices, face->len);
    }
}

int check_coplanar_vertices(model_t *model, uint32_t *indices, uint32_t len) {
    vec3 normal;
    calculate_normal(model, normal, indices, len);

    for (int i = 1; i < len; i++) {
        vec3 vector;
        vec3_sub(vector, model->vertices[indices[0]], model->vertices[indices[i]]);
        vec3_normalize(vector, vector);

        float dot = vec3_dot(vector, normal);
        if (fabs(dot) > 0.0001f) {
            return 0;
        }
    }

    return 1;
}

void sort_by_angle(model_t *model, vec3 midpoint, vec3 normal, uint32_t *indices, uint32_t len) {
    if (len < 3)
        return;
        
    vec3 other;
    vec3_set(other, 0.0f, 1.0f, 0.0f);

    float dot = vec3_dot(other, normal);
    if (fabs(fabs(dot) - 1.0f) < 0.0001f)
        vec3_set(other, 0.0f, 0.0f, -1.0f);

    vec3 x_axis;
    vec3_cross(x_axis, other, normal);
    vec3_normalize(x_axis, x_axis);

    vec3 y_axis;
    vec3_cross(y_axis, normal, x_axis);
    vec3_normalize(y_axis, y_axis);

    uint32_t sorted_indices[len];

    float angles[len];
    for (int i = 0; i < len; i++) {
        vec3 vector; 
        vec3_sub(vector, model->vertices[indices[i]], midpoint);

        float x = vec3_dot(x_axis, vector);
        float y = vec3_dot(y_axis, vector);

        float theta = atan2(y, x);
        angles[i] = theta;
    }

    float last_angle = -M_PI;
    for (int i = 0; i < len; i++) {
        
        float current_angle = M_PI; 
        for (int j = 0; j < len; j++) {
            float angle = angles[j];
            
            if (angle > last_angle && angle <= current_angle) {
                sorted_indices[i] = indices[j];
                current_angle = angle;
            }
        }

        last_angle = current_angle;
    }

    for (int i = 0; i < len; i++)
        indices[i] = sorted_indices[i];
}
