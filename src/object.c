#include "object.h"

#include <stdlib.h>

#include "camera.h"
#include "compare.h"
#include "light.h"
#include "shader.h"

extern camera_t camera;
extern light_t light;
extern int width, height;
extern int show_lines;
extern int show_points;
extern int selection_len;
extern int selection_buffer[];

void calc_vertices(object_t* object);
void calc_face_normal(vec3 normal, const vec3 a, const vec3 b, const vec3 c);

void init_object(object_t* object) {
    object->positions = (vec3*)malloc(sizeof(vec3) * 10);
    object->positions_cap = 10;
    object->positions_len = 0;

    object->indices = (uint32_t*)malloc(sizeof(uint32_t) * 10);
    object->indices_cap = 10;
    object->indices_len = 0;

    object->vertices = (vertex_t*)malloc(sizeof(vertex_t) * 10);
    object->vertices_cap = 10;
    object->vertices_len = 0;

    // Position VAO, used to display object positions as points

    glGenVertexArrays(1, &object->pos_vao);
    glBindVertexArray(object->pos_vao);

    // Positions
    glGenBuffers(1, &object->pos_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, object->pos_vbo);
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);  // Positions

    object->pos_shader = load_shader("shaders/static.vert", "shaders/static.frag");

    // Vertices VAO, used to display object faces as triangles

    glGenVertexArrays(1, &object->obj_vao);
    glBindVertexArray(object->obj_vao);

    // Vertices
    glGenBuffers(1, &object->obj_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, object->obj_vbo);
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)0);  // Positions

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(vertex_t), (void*)(sizeof(vec3) * 1));  // Normals

    object->obj_shader = load_shader("shaders/object.vert", "shaders/object.frag");
}

void free_object(object_t* object) {
    free(object->positions);
    free(object->indices);
    free(object->vertices);

    glDeleteVertexArrays(1, &object->pos_vao);
    glDeleteBuffers(1, &object->pos_vbo);

    glDeleteVertexArrays(1, &object->obj_vao);
    glDeleteBuffers(1, &object->obj_vbo);
}

// Update data methods

void add_position(object_t* object, vec3 position) {
    if (object->positions_len == object->positions_cap) {
        object->positions_cap *= 2;
        object->positions = (vec3*)realloc(object->positions, sizeof(vec3) * object->positions_cap);
    }

    vec3_copy(object->positions[object->positions_len++], position);
}

void add_index(object_t* object, uint32_t index) {
    if (object->indices_len == object->indices_cap) {
        object->indices_cap *= 2;
        object->indices = (uint32_t*)realloc(object->indices, sizeof(uint32_t) * object->indices_cap);
    }

    object->indices[object->indices_len++] = index;
}

void add_vertex(object_t* object, vertex_t vertex) {
    if (object->vertices_len == object->vertices_cap) {
        object->vertices_cap *= 2;
        object->vertices = (vertex_t*)realloc(object->vertices, sizeof(vertex_t) * object->vertices_cap);
    }

    object->vertices[object->vertices_len++] = vertex;
}

// Selection methods

void add_point_selection(object_t* object) {
    add_position(object, (vec3){0.0f, 0.5f, 0.0f});
    selection_buffer[selection_len++] = object->positions_len - 1;

    buffer_object(object);
}

void remove_selection(object_t* object) {
    for (int i = 0; i < selection_len; i++) {
        int index = selection_buffer[i];

        for (int i = index; i < object->positions_len - 1; i++)
            vec3_copy(object->positions[i], object->positions[i + 1]);

        object->positions_len--;

        // Mark no longer valid face indices with -1
        for (int idx = 0; idx < object->indices_len; idx += 3) {
            int remove_face = object->indices[idx] == index;
            remove_face = remove_face || object->indices[idx + 1] == index;
            remove_face = remove_face || object->indices[idx + 2] == index;

            if (!remove_face)
                continue;

            object->indices[idx + 0] = -1;
            object->indices[idx + 1] = -1;
            object->indices[idx + 2] = -1;
        }
    }

    // Remove invalid face indices
    int index_count = 0;
    for (int i = 0; i < object->indices_len; i++) {
        if (object->indices[i] != -1)
            object->indices[index_count++] = object->indices[i];
    }

    object->indices_len = index_count;
    selection_len = 0;

    buffer_object(object);
}

void move_selection(object_t* object, vec3 delta) {
    float epsilon = 0.001f;

    for (int i = 0; i < selection_len; i++) {
        int index = selection_buffer[i];

        vec3 pos;
        vec3_copy(pos, object->positions[index]);

        // Check bounds
        if ((delta[0] < 0) && compare(pos[0] + delta[0], -0.5f + delta[0], epsilon) <= 0)
            return;
        if ((delta[0] > 0) && compare(pos[0] + delta[0], +0.5f + delta[0], epsilon) >= 0)
            return;

        if ((delta[1] < 0) && compare(pos[1] + delta[1], -0.5f + delta[1], epsilon) <= 0) return;
        if ((delta[1] > 0) && compare(pos[1] + delta[1], +0.5f + delta[1], epsilon) >= 0) return;

        if ((delta[2] < 0) && compare(pos[2] + delta[2], -0.5f + delta[2], epsilon) <= 0) return;
        if ((delta[2] > 0) && compare(pos[2] + delta[2], +0.5f + delta[2], epsilon) >= 0) return;
    }

    for (int i = 0; i < selection_len; i++) {
        int index = selection_buffer[i];
        vec3_add(object->positions[index], object->positions[index], delta);
    }

    buffer_object(object);
}

void position_selection(object_t* object, vec3 origin) {
    vec3 midpoint;
    vec3_zero(midpoint);

    for (int i = 0; i < selection_len; i++)
        vec3_add(midpoint, midpoint, object->positions[selection_buffer[i]]);

    vec3_scale(midpoint, midpoint, 1.0f / (float)selection_len);

    vec3 deltas[selection_len];
    for (int i = 0; i < selection_len; i++)
        vec3_sub(deltas[i], midpoint, object->positions[selection_buffer[i]]);

    for (int i = 0; i < selection_len; i++)
        vec3_add(object->positions[selection_buffer[i]], origin, deltas[i]);

    buffer_object(object);
}

void add_face_selection(object_t* object) {
    if (selection_len != 3)
        return;

    for (int i = 0; i < selection_len; i++)
        add_index(object, selection_buffer[i]);

    buffer_object(object);
}

// Drawing methods

void buffer_object(object_t* object) {
    calc_vertices(object);

    // Positions
    glBindVertexArray(object->pos_vao);
    glBindBuffer(GL_ARRAY_BUFFER, object->pos_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * object->positions_len, object->positions, GL_DYNAMIC_DRAW);

    // Vertices
    glBindVertexArray(object->obj_vao);
    glBindBuffer(GL_ARRAY_BUFFER, object->obj_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_t) * object->vertices_len, object->vertices, GL_DYNAMIC_DRAW);
}

void load_uniforms(int shader) {
    mat4x4 model, view, projection;
    mat4x4_identity(model);
    mat4x4_look_at(view, camera.pos, camera.dir, camera.up);
    mat4x4_perspective(projection, 45.0f, (float)width / (float)height, 0.1f, 100.0f);

    GLint model_loc = glGetUniformLocation(shader, "model");
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, (float*)model);

    GLint view_loc = glGetUniformLocation(shader, "view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, (float*)view);

    GLint projection_loc = glGetUniformLocation(shader, "projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float*)projection);

    GLint lightpos_loc = glGetUniformLocation(shader, "lightPos");
    glUniform3fv(lightpos_loc, 1, (float*)light.pos);

    GLint lightcolor_loc = glGetUniformLocation(shader, "lightColor");
    glUniform3fv(lightcolor_loc, 1, (float*)light.color);
}

void draw_positions(object_t* object) {
    glUseProgram(object->pos_shader);

    load_uniforms(object->pos_shader);
    GLint color_loc = glGetUniformLocation(object->pos_shader, "color");

    glPointSize(20);
    glUniform3f(color_loc, 0.0f, 1.0f, 0.0f);

    glBindVertexArray(object->pos_vao);

    for (int i = 0; i < selection_len; i++) {
        int index = selection_buffer[i];
        glDrawArrays(GL_POINTS, index, 1);
    }

    glUniform3f(color_loc, 1.0f, 1.0f, 1.0f);
    glDrawArrays(GL_POINTS, 0, object->positions_len);
}

void draw_object(object_t* object) {
    if (show_points)
        draw_positions(object);

    glUseProgram(object->obj_shader);
    load_uniforms(object->obj_shader);

    GLint color_loc = glGetUniformLocation(object->obj_shader, "color");
    glUniform3f(color_loc, 1.0f, 1.0f, 1.0f);

    if (show_lines) {
        glDisable(GL_CULL_FACE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    glBindVertexArray(object->obj_vao);
    glDrawArrays(GL_TRIANGLES, 0, object->vertices_len);

    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

// Ray methods

int find_intercept(object_t* object) {
    vec3 ray_start, ray_dir;
    vec3_copy(ray_start, camera.ray_start);
    vec3_copy(ray_dir, camera.ray);

    for (float t = 0.0f; t < 10.0f; t += 0.01f) {
        vec3 point;
        vec3_scale(point, ray_dir, t);
        vec3_add(point, point, ray_start);

        for (int i = 0; i < object->positions_len; i++) {
            vec3 pos, tmp;
            vec3_copy(pos, object->positions[i]);

            float dist;
            vec3_sub(tmp, point, pos);
            dist = vec3_len(tmp);

            if (dist < 0.025f)
                return i;
        }
    }

    return -1;
}

//  Calculation methods

void calc_face_normal(vec3 normal, const vec3 a, const vec3 b, const vec3 c) {
    vec3 ab, ac;
    vec3_sub(ab, b, a);
    vec3_sub(ac, c, a);

    vec3_cross(normal, ab, ac);
    vec3_normalize(normal, normal);
}

void calc_vertices(object_t* object) {
    object->vertices_len = 0;

    vec3 origin;
    vec3_zero(origin);

    for (int i = 0; i < object->positions_len; i++)
        vec3_add(origin, origin, object->positions[i]);

    vec3_scale(origin, origin, 1.0f / (float)object->positions_len);

    for (int i = 0; i < object->indices_len; i += 3) {
        vec3 a, b, c, mid, normal;
        vec3_copy(a, object->positions[object->indices[i + 0]]);
        vec3_copy(b, object->positions[object->indices[i + 1]]);
        vec3_copy(c, object->positions[object->indices[i + 2]]);
        calc_face_normal(normal, a, b, c);

        vec3_add(mid, a, b);
        vec3_add(mid, mid, c);
        vec3_scale(mid, mid, 1.0f / 3.0f);
        vec3_sub(mid, mid, origin);

        float dot = vec3_dot(normal, mid);
        if (dot < 0) {
            object->indices[i + 1] ^= object->indices[i + 2];
            object->indices[i + 2] ^= object->indices[i + 1];
            object->indices[i + 1] ^= object->indices[i + 2];
        }

        vertex_t va, vb, vc;
        vec3_copy(va.position, object->positions[object->indices[i + 0]]);
        vec3_copy(vb.position, object->positions[object->indices[i + 1]]);
        vec3_copy(vc.position, object->positions[object->indices[i + 2]]);

        calc_face_normal(normal, va.position, vb.position, vc.position);
        vec3_copy(va.normal, normal);
        vec3_copy(vb.normal, normal);
        vec3_copy(vc.normal, normal);

        add_vertex(object, va);
        add_vertex(object, vb);
        add_vertex(object, vc);
    }
}
