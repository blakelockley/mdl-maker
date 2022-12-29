#include <stdlib.h>

#include "renderers.h"
#include "shader.h"
#include "camera.h"
#include "viewport.h"
#include "light.h"
#include "selection.h"

extern camera_t camera;
extern viewport_t viewport;
extern light_t light;

struct _debug_point_t {
    vec3 p, color;
};

struct _debug_line_t {
    vec3 a, b, color;
};

struct _debug_plane_t {
    vec3 origin, normal, color;
    float width, height;
};

typedef struct _debug_point_t debug_point_t;
typedef struct _debug_line_t debug_line_t;
typedef struct _debug_plane_t debug_plane_t;

static renderer_t renderer;

#define DEBUG_SHAPE_CAP 8

static uint32_t points_len;
static debug_point_t points[DEBUG_SHAPE_CAP];

static uint32_t lines_len;
static debug_line_t lines[DEBUG_SHAPE_CAP];

static uint32_t planes_len;
static debug_plane_t planes[DEBUG_SHAPE_CAP];

void init_debug_renderer() {
    init_renderer(&renderer, 1);
    
    renderer.shader = load_shader("shaders/static.vert", "shaders/static.frag");
    renderer.render = NULL;

    // Positions
    glBindBuffer(GL_ARRAY_BUFFER, renderer.vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);

    glBindVertexArray(0);
}

void render_debug_shapes() {    
    glUseProgram(renderer.shader);
    
    mat4x4 _model, view, projection;
    mat4x4_identity(_model);
    get_view_matrix(&camera, view);
    get_projection_matrix(&viewport, projection);
    
    GLint model_loc = glGetUniformLocation(renderer.shader, "model");
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, (float*)_model);

    GLint view_loc = glGetUniformLocation(renderer.shader, "view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, (float*)view);

    GLint projection_loc = glGetUniformLocation(renderer.shader, "projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float*)projection);
     
    GLint color_loc = glGetUniformLocation(renderer.shader, "color");
    glUniform3f(color_loc, 1.0f, 1.0f, 1.0f);

    glBindVertexArray(renderer.vao);

    vec3 plane_positions[planes_len * 4];
    for (int i = 0; i < planes_len; i++) {
        vec3 other;
        vec3_set(other, 0.0f, 1.0f, 0.0f);

        float dot = vec3_dot(other, planes[i].normal);
        if (fabs(fabs(dot) - 1.0f) < 0.0001f)
            vec3_set(other, 0.0f, 0.0f, -1.0f);

        vec3 x_axis;
        vec3_cross(x_axis, other, planes[i].normal);
        vec3_normalize(x_axis, x_axis);
        vec3_scale(x_axis, x_axis, planes[i].width / 2.0f);

        vec3 y_axis;
        vec3_cross(y_axis, planes[i].normal, x_axis);
        vec3_normalize(y_axis, y_axis);
        vec3_scale(y_axis, y_axis, planes[i].height / 2.0f);

        vec3 tl, tr, bl, br;
        vec3_sub(tl, planes[i].origin, x_axis);
        vec3_add(tl, tl, y_axis);
        
        vec3_add(tr, planes[i].origin, x_axis);
        vec3_add(tr, tr, y_axis);
        
        vec3_sub(bl, planes[i].origin, x_axis);
        vec3_sub(bl, bl, y_axis);
        
        vec3_add(br, planes[i].origin, x_axis);
        vec3_sub(br, br, y_axis);

        vec3_copy(plane_positions[(i * 4) + 0], tl);
        vec3_copy(plane_positions[(i * 4) + 1], tr);
        vec3_copy(plane_positions[(i * 4) + 2], br);
        vec3_copy(plane_positions[(i * 4) + 3], bl);
    }

    glBindBuffer(GL_ARRAY_BUFFER, renderer.vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane_positions), plane_positions, GL_DYNAMIC_DRAW);
    
    for (int i = 0; i < planes_len; i++) {
        glUniform3fv(color_loc, 1, planes[i].color);
        glDrawArrays(GL_TRIANGLE_FAN, (i * 4), 4);
    }

    vec3 point_positions[points_len];
    for (int i = 0; i < points_len; i++)
        vec3_copy(point_positions[i], points[i].p);

    glDepthFunc(GL_ALWAYS);
    glBindBuffer(GL_ARRAY_BUFFER, renderer.vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(point_positions), point_positions, GL_DYNAMIC_DRAW);
    
    glPointSize(10.0f);
    for (int i = 0; i < points_len; i++) {
        glUniform3fv(color_loc, 1, points[i].color);
        glDrawArrays(GL_POINTS, i, 1);
    }

    vec3 line_positions[lines_len * 2];
    for (int i = 0; i < lines_len; i++) {
        vec3_copy(line_positions[(i * 2) + 0], lines[i].a);
        vec3_copy(line_positions[(i * 2) + 1], lines[i].b);
    }

    glDepthFunc(GL_ALWAYS);
    glBindBuffer(GL_ARRAY_BUFFER, renderer.vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(line_positions), line_positions, GL_DYNAMIC_DRAW);
    
    for (int i = 0; i < lines_len; i++) {
        glUniform3fv(color_loc, 1, lines[i].color);
        glDrawArrays(GL_LINES, (i * 2), 2);
    }

    glBindVertexArray(0);
    glDepthFunc(GL_LEQUAL);
}

void clear_debug_shapes() {
    points_len = 0;
    lines_len  = 0;
    planes_len = 0;
}

void push_debug_point(vec3 p, vec3 color) {
    if (points_len >= DEBUG_SHAPE_CAP) {
        fprintf(stderr, "[WARNING] Maximum amount of debug points has been reached for this frame (%d).", DEBUG_SHAPE_CAP);
        return;
    }

    uint32_t index = points_len++;
    vec3_copy(points[index].p, p);
    vec3_copy(points[index].color, color);
}

void push_debug_line(vec3 a, vec3 b, vec3 color) {
    if (lines_len >= DEBUG_SHAPE_CAP) {
        fprintf(stderr, "[WARNING] Maximum amount of debug lines has been reached for this frame (%d).", DEBUG_SHAPE_CAP);
        return;
    }
    
    uint32_t index = lines_len++;
    vec3_copy(lines[index].a, a);
    vec3_copy(lines[index].b, b);
    vec3_copy(lines[index].color, color);
}

void push_debug_plane(vec3 origin, vec3 normal, float width, float height, vec3 color) {
    if (planes_len >= DEBUG_SHAPE_CAP) {
        fprintf(stderr, "[WARNING] Maximum amount of debug planes has been reached for this frame (%d).", DEBUG_SHAPE_CAP);
        return;
    }
    uint32_t index = planes_len++;
    vec3_copy(planes[index].origin, origin);
    vec3_copy(planes[index].normal, normal);
    vec3_copy(planes[index].color, color);
    
    planes[index].width = width;
    planes[index].height = height;
}
