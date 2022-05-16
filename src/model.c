#include "model.h"

#include <stdio.h>
#include <stdlib.h>

#include "camera.h"
#include "viewport.h"
#include "selection.h"
#include "vertex_renderer.h"
#include "face_renderer.h"
#include "normal_renderer.h"
#include "wireframe_renderer.h"

extern selection_t selection;
extern camera_t camera;
extern viewport_t viewport;

int check_coplanar_vertices(model_t *model, uint32_t *indices, uint32_t len);
void sort_by_angle(model_t *model, vec3 midpoint, vec3 normal, uint32_t *indices, uint32_t len);

void calculate_normal(vec3 r, vec3 a, vec3 b, vec3 c);
void calculate_midpoint(model_t *model, vec3 r, uint32_t *indices, uint32_t len);

void update_model(model_t *model);

void print_vertices(model_t *model);
void print_faces(model_t *model);

void init_model(model_t *model) {
    vec3_set(model->color, 0.25f, 0.45f, 1.0f);

    model->vertices = (vec3*)malloc(sizeof(vec3) * 10);
    model->vertices_cap = 10;
    model->vertices_len = 0;

    model->faces = (face_t*)malloc(sizeof(face_t) * 10);
    model->faces_cap = 10;
    model->faces_len = 0;

    model->face_renderer = (face_renderer_t *)malloc(sizeof(face_renderer_t));
    init_face_renderer(model->face_renderer);

    model->vertex_renderer = (vertex_renderer_t *)malloc(sizeof(vertex_renderer_t));
    init_vertex_renderer(model->vertex_renderer);

    model->normal_renderer = (normal_renderer_t *)malloc(sizeof(normal_renderer_t));
    init_normal_renderer(model->normal_renderer);

    model->wireframe_renderer = (wireframe_renderer_t *)malloc(sizeof(wireframe_renderer_t));
    init_wireframe_renderer(model->wireframe_renderer);

    model->render_mode = RENDER_MODE_VERTICES | RENDER_MODE_FACES;
}

void free_model(model_t *model) {
    for (int i = 0; i < model->faces_len; i++)
        free(model->faces[i].indices);
    
    free(model->vertices);
    free(model->faces);

    free_face_renderer(model->face_renderer);
    free(model->face_renderer);

    free_vertex_renderer(model->vertex_renderer);
    free(model->vertex_renderer);

    free_normal_renderer(model->normal_renderer);
    free(model->normal_renderer);
}

void render_model(model_t *model) {
    if (model->render_mode & RENDER_MODE_VERTICES)
        render_model_vertices(model->vertex_renderer, model);
    
    if (model->render_mode & RENDER_MODE_FACES)
        render_model_faces(model->face_renderer, model);
    
    if (model->render_mode & RENDER_MODE_NORMALS)
        render_model_normals(model->normal_renderer, model);
    
    if (model->render_mode & RENDER_MODE_WIREFRAME)
        render_model_wireframe(model->wireframe_renderer, model);
}

// Update data methods

uint32_t add_vertex(model_t *model, vec3 vertex) {
    if (model->vertices_len == model->vertices_cap) {
        model->vertices_cap *= 2;
        model->vertices = (vec3*)realloc(model->vertices, sizeof(vec3) * model->vertices_cap);
    }

    vec3_copy(model->vertices[model->vertices_len++], vertex);

    return model->vertices_len - 1;
}

void move_vertices(model_t *model, uint32_t *indices, uint32_t len, vec3 delta) {
    for (int i = 0; i < len; i++)
        vec3_add(model->vertices[indices[i]], model->vertices[indices[i]], delta);

    update_model(model);
}

void scale_vertices(model_t *model, uint32_t *indices, uint32_t len, float factor) {
    vec3 midpoint;
    calculate_midpoint(model, midpoint, indices, len);

    for (int i = 0; i < len; i++) {
        vec3 vector;
        vec3_sub(vector, model->vertices[indices[i]], midpoint);
        vec3_scale(vector, vector, factor);
        vec3_add(model->vertices[indices[i]], midpoint, vector);
    }
}

void remove_vertex(model_t *model, uint32_t index) {
    model->vertices_len -= 1;
    for (int i = index; i < model->vertices_len; i++)
        vec3_copy(model->vertices[i], model->vertices[i + 1]);

    for (int i = 0; i < model->faces_len; i++) {
        face_t *face = &model->faces[i];
        
        for (int j = 0; j < face->len; j++) {
            if (face->indices[j] == index) {
                face->len = 0;
                free(face->indices);
                break;
            }
        }
    }

    int removed_faces = 0;
    for (int i = 0; i < model->faces_len; i++) {
        face_t *face = &model->faces[i];

        if (face->len == 0) {
            removed_faces++;
            for (int j = i; j < model->faces_len - 1; j++)
                model->faces[j] = model->faces[j + 1];

            continue;
        }
    }
    
    model->faces_len -= removed_faces;

    for (int i = 0; i < model->faces_len; i++) {
        face_t *face = &model->faces[i];
        
        for (int j = 0; j < face->len; j++)
            if (face->indices[j] > index)
                face->indices[j]--;
    }
}

int reverse_cmp (const void * a, const void * b) {
   return ( *(uint32_t*)b - *(uint32_t*)a );
}

void remove_vertices(model_t *model, uint32_t *indices, uint32_t len) {
    qsort(indices, len, sizeof(uint32_t), reverse_cmp);
    
    for (int i = 0; i < len; i++)
        remove_vertex(model, indices[i]);

    update_model(model);
    clear_selection(&selection);
}

face_t *add_face(model_t *model, uint32_t *indices, uint32_t len) {
    if (len < 3 || len > 1024)
        return NULL;

    if (!check_coplanar_vertices(model, indices, len))
        return NULL;

    vec3 midpoint;
    calculate_midpoint(model, midpoint, indices, len);

    vec3 normal;
    calculate_normal(normal, model->vertices[indices[0]], model->vertices[indices[1]], model->vertices[indices[2]]);

    vec3 camera_to_face;
    vec3_sub(camera_to_face, midpoint, camera.pos);

    float dot = vec3_dot(camera_to_face, normal);
    if (dot >= 0.0f)
        vec3_scale(normal, normal, -1.0f);

    sort_by_angle(model, midpoint, normal, indices, len);

    if (model->faces_len == model->faces_cap) {
        model->faces_cap *= 2;
        model->faces = (face_t*)realloc(model->faces, sizeof(face_t) * model->faces_cap);
    }

    face_t *face = &model->faces[model->faces_len++];
    face->len = len;
    
    face->indices = (uint32_t*)malloc(sizeof(uint32_t) * len);
    memcpy(face->indices, indices, sizeof(uint32_t) * len);
    
    vec3_copy(face->midpoint, midpoint);
    vec3_copy(face->normal, normal);

    return face;
}

void update_model(model_t *model) {
    for (int i = 0; i < model->faces_len; i++) {
            face_t *face = &model->faces[i];
    
        calculate_normal(face->normal,
            model->vertices[face->indices[0]],
            model->vertices[face->indices[1]],
            model->vertices[face->indices[2]]
        );

        calculate_midpoint(model, face->midpoint, face->indices, face->len);
    }
}

face_t *extend_edge(model_t *model, uint32_t *indices, uint32_t len){
    if (len != 2)
        return NULL;

    vec3 a, b;
    vec3_copy(a, model->vertices[indices[0]]);
    vec3_copy(b, model->vertices[indices[1]]);

    vec3 ab;
    vec3_sub(ab, b, a);

    vec3 normal;
    vec3_cross(normal, ab, camera.up);
    vec3_normalize(normal, normal);
    vec3_scale(normal, normal, 0.1f);

    uint32_t new_indices[4];
    for (int i = 0; i < 2; i++) {
        uint32_t vi = indices[i];
        
        vec3 new_vertex;
        vec3_copy(new_vertex, model->vertices[vi]);
        vec3_add(new_vertex, new_vertex, normal);
        
        uint32_t new_vi = add_vertex(model, new_vertex);

        new_indices[i] = indices[i];
        new_indices[2 + i] = new_vi;
    }

    face_t *face = add_face(model, new_indices, 4);

    clear_selection(&selection);
    extend_selection(&selection, new_indices[2]);
    extend_selection(&selection, new_indices[3]);

    return face;
}

face_t *extend_face(model_t *model, uint32_t *indices, uint32_t len) {
    if (len < 3 || len > 1024)
        return NULL;

    if (!check_coplanar_vertices(model, indices, len))
        return NULL;

    vec3 normal;
    calculate_normal(normal, model->vertices[indices[0]], model->vertices[indices[1]], model->vertices[indices[2]]);

    vec3 midpoint;
    calculate_midpoint(model, midpoint, indices, len);

    vec3 camera_to_face;
    vec3_sub(camera_to_face, midpoint, camera.pos);

    float dot = vec3_dot(camera_to_face, normal);
    if (dot < 0.0f)
        vec3_scale(normal, normal, -1.0f);

    sort_by_angle(model, midpoint, normal, indices, len);
    
    vec3_scale(normal, normal, 0.5f);
    vec3_add(midpoint, midpoint, normal);

    uint32_t new_indices[len];
    for (int i = 0; i < len; i++) {
        vec3 v;
        vec3_copy(v, model->vertices[indices[i]]);
        vec3_add(v, v, normal);

        uint32_t vi = add_vertex(model, v);
        new_indices[i] = vi;
    }

    uint32_t face_indices[4];
    for (int i = 0; i < len; i++) {
        face_indices[0] = indices[i];
        face_indices[1] = new_indices[i];
        face_indices[2] = indices[(i + 1) % len];
        face_indices[3] = new_indices[(i + 1) % len];

        face_t *face = add_face(model, face_indices, 4);

        vec3 midpoint_to_face;
        vec3_sub(midpoint_to_face, face->midpoint, midpoint);

        float dot = vec3_dot(midpoint_to_face, face->normal);
        if (dot < 0.0f)
            flip_face(model, face);
    }

    clear_selection(&selection);
    for (int i = 0; i < len; i++)
        extend_selection(&selection, new_indices[i]);

    return NULL;
}

face_t *get_face(model_t *model, uint32_t *indices, uint32_t len) {
    for (int i = 0; i < model->faces_len; i++) {
        face_t *face = &model->faces[i];

        if (face->len != len)
            continue;
        
        int included_indices = 0;
        for (int j = 0; j < len; j++) {
            for (int k = 0; k < face->len; k++) {
                if (face->indices[k] == indices[j]) {
                    included_indices++;
                    break;
                }
            }
        }

        if (included_indices == len)
            return face;
    }

    return NULL;
}

void flip_face(model_t *model, face_t *face) {
    if (!face)
        return;

    uint32_t reverse_indices[face->len];
    for (int k = 0; k < face->len; k++)
        reverse_indices[k] = face->indices[face->len - k - 1];

    memcpy(face->indices, reverse_indices, sizeof(reverse_indices));
    update_model(model);
}

void calculate_normal(vec3 r, vec3 a, vec3 b, vec3 c) {
    vec3 ab, ac;
    vec3_sub(ab, b, a);
    vec3_sub(ac, c, a);

    vec3_cross(r, ab, ac);
    vec3_normalize(r, r);
}

void calculate_midpoint(model_t *model, vec3 r, uint32_t *indices, uint32_t len) {
    vec3_zero(r);
    
    for (int i = 0; i < len; i++)
        vec3_add(r, r, model->vertices[indices[i]]);
    
    vec3_scale(r, r, 1.0f / (float)len);
}

// Debug print methods

void print_vertices(model_t *model) {
    for (int i = 0; i < model->vertices_len; i++) {
        vec3 v;
        vec3_copy(v, model->vertices[i]);
         
        printf("%02d: (%+.2f, %+.2f, %+.2f)\n", i, v[0], v[1], v[2]);
    }
}

void print_faces(model_t *model) {
    for (int i = 0; i < model->faces_len; i++) {
        face_t *face = &model->faces[i];

        printf("Face #%02d\n", i);
        printf("-> len:     %02d\n", face->len);
        printf("-> indices: ");
        
        for (int j = 0; j < face->len; j++)
            printf("%01d, ", face->indices[j]);
        
        printf("\n");
    }
}

int check_coplanar_vertices(model_t *model, uint32_t *indices, uint32_t len) {
    vec3 normal;
    calculate_normal(normal, model->vertices[indices[0]], model->vertices[indices[1]], model->vertices[indices[2]]);

    for (int i = 1; i < len; i++) {
        vec3 vector;
        vec3_sub(vector, model->vertices[indices[0]], model->vertices[indices[i]]);
        vec3_normalize(vector, vector);

        float dot = vec3_dot(vector, normal);
        if (fabs(dot) > 0.0001f) {
            printf("[ERROR] Cannot create face from non-planar vertices\n");
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

// Render mode

void set_render_mode(model_t *model, uint8_t mode) {
    model->render_mode = mode;
}

void toggle_render_mode(model_t *model, uint8_t mode) {
    model->render_mode ^= mode;
}
