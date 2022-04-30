#include "model.h"

#include <stdio.h>
#include <stdlib.h>

#include "camera.h"
#include "viewport.h"
#include "shader.h"
#include "selection.h"
#include "face_renderer.h"
#include "normal_renderer.h"

model_t model;
extern selection_t selection;
extern camera_t camera;
extern viewport_t viewport;

void calculate_normal(vec3 r, vec3 a, vec3 b, vec3 c);
void calculate_midpoint(vec3 r, uint32_t *indices, uint32_t len);

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
    for (int i = 0; i < model.faces_len; i++)
        free(model.faces[i].indices);
    
    free(model.vertices);
    free(model.faces);

    glDeleteVertexArrays(1, &model.pos_vao);
    glDeleteBuffers(1, &model.pos_vbo);
}

// Update data methods

uint32_t add_vertex(vec3 vertex) {
    if (model.vertices_len == model.vertices_cap) {
        model.vertices_cap *= 2;
        model.vertices = (vec3*)realloc(model.vertices, sizeof(vec3) * model.vertices_cap);
    }

    vec3_copy(model.vertices[model.vertices_len++], vertex);

    return model.vertices_len - 1;
}

face_t *add_face() {
    if (selection.len < 3 || selection.len > 1024)
        return NULL;

    if (model.faces_len == model.faces_cap) {
        model.faces_cap *= 2;
        model.faces = (face_t*)realloc(model.faces, sizeof(face_t) * model.faces_cap);
    }

    uint32_t len = selection.len;

    uint32_t indices[len];
    for (int i = 0; i < len; i++)
        indices[i] = selection.indices[i];

    vec3 midpoint;
    calculate_midpoint(midpoint, indices, len);
        
    vec3 normal;
    calculate_normal(normal, model.vertices[indices[0]], model.vertices[indices[1]], model.vertices[indices[2]]);

    vec3 other;
    if (fabs(normal[2]) == 1.0f)
        vec3_set(other, 0.0f, 1.0f, 0.0f);
    else
        vec3_set(other, 0.0f, 0.0f, 1.0f);

    vec3 x_axis;
    vec3_cross(x_axis, other, normal);
    vec3_normalize(x_axis, x_axis);

    vec3 y_axis;
    vec3_cross(y_axis, normal, x_axis);
    vec3_normalize(y_axis, y_axis);

    float angles[len];
    for (int i = 0; i < len; i++) {
        vec3 vector; 
        vec3_sub(vector, model.vertices[indices[i]], midpoint);

        float x = vec3_dot(x_axis, vector);
        float y = vec3_dot(y_axis, vector);

        float theta = atan2(x, y);
        angles[i] = theta;
    }

    float last_angle = -M_PI;
    uint32_t sorted_indices[len];

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

    face_t *face = &model.faces[model.faces_len++];
    face->len = len;
    
    face->indices = (uint32_t*)malloc(sizeof(sorted_indices));
    memcpy(face->indices, sorted_indices, sizeof(sorted_indices));

    vec3_copy(face->midpoint, midpoint);
    
    // Re-calculate normal after sorting indices
    calculate_normal(normal, model.vertices[face->indices[0]], model.vertices[face->indices[1]], model.vertices[face->indices[2]]);
    vec3_copy(face->normal, normal);

    return face;
}

void update_faces() {
    for (int i = 0; i < model.faces_len; i++) {
            face_t *face = &model.faces[i];
    
        calculate_normal(face->normal,
            model.vertices[face->indices[0]],
            model.vertices[face->indices[1]],
            model.vertices[face->indices[2]]
        );

        calculate_midpoint(face->midpoint, face->indices, face->len);
    }
}

void flip_face() {
    for (int i = 0; i < model.faces_len; i++) {
        face_t *face = &model.faces[i];
        int included_indices = 0;
        
        for (int j = 0; j < selection.len; j++) {
            uint32_t index = selection.indices[j];
            
            for (int k = 0; k < face->len; k++) {
                if (face->indices[k] == index) {
                    included_indices++;
                    break;
                }
            }
        }

        if (included_indices != selection.len)
            continue;

        uint32_t reverse_indices[face->len];
        for (int k = 0; k < face->len; k++) {
            reverse_indices[k] = face->indices[face->len - k - 1];
        }

        memcpy(face->indices, reverse_indices, sizeof(reverse_indices));
    }

    update_faces();
}

void extend_face() {
    if (selection.len != 2)
        return;

    vec3 a, b;
    vec3_copy(a, model.vertices[selection.indices[0]]);
    vec3_copy(b, model.vertices[selection.indices[1]]);

    vec3 ab;
    vec3_sub(ab, a, b);

    vec3 normal;
    vec3_cross(normal, ab, camera.up);
    vec3_normalize(normal, normal);
    vec3_scale(normal, normal, 0.1f);

    uint32_t new_incides[2];

    for (int i = 0; i < 2; i++) {
        uint32_t vi = selection.indices[i];
        
        vec3 new_vertex;
        vec3_copy(new_vertex, model.vertices[vi]);
        vec3_add(new_vertex, new_vertex, normal);
        
        uint32_t new_vi = add_vertex(new_vertex);

        new_incides[i] = new_vi;
        extend_selection(&selection, new_vi);
    }

    add_face();

    selection.len = 0;
    extend_selection(&selection, new_incides[0]);
    extend_selection(&selection, new_incides[1]);
}

void calculate_normal(vec3 r, vec3 a, vec3 b, vec3 c) {
    vec3 ab, ac;
    vec3_sub(ab, b, a);
    vec3_sub(ac, c, a);

    vec3_cross(r, ab, ac);
    vec3_normalize(r, r);
}

void calculate_midpoint(vec3 r, uint32_t *indices, uint32_t len) {
    vec3_zero(r);
    
    for (int i = 0; i < len; i++)
        vec3_add(r, r, model.vertices[indices[i]]);
    
    vec3_scale(r, r, 1.0f / (float)len);
}

void draw_model() {
    glBindVertexArray(model.pos_vao);
    glBindBuffer(GL_ARRAY_BUFFER, model.pos_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * model.vertices_len, model.vertices, GL_DYNAMIC_DRAW);

    glUseProgram(model.pos_shader);
    
    mat4x4 _model, view, projection;
    mat4x4_identity(_model);
    get_view_matrix(&camera, view);
    get_projection_matrix(&viewport, projection);
    
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
    for (int i = 0; i < selection.len; i++) {
        int index = selection.indices[i];
        glDrawArrays(GL_POINTS, index, 1);
    }

    glUniform3f(color_loc, 1.0f, 1.0f, 1.0f);
    glDrawArrays(GL_POINTS, 0, model.vertices_len);

    render_model_faces(&model);
    render_model_normals(&model);
}
