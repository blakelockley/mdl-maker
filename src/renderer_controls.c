#include <stdlib.h>

#include "renderers.h"
#include "shader.h"
#include "camera.h"
#include "light.h"
#include "selection.h"

extern camera_t camera;
extern light_t light;

static renderer_t _control_renderer;
static renderer_t *control_renderer = &_control_renderer;

void init_control_renderer() {
    init_renderer(control_renderer, 1);

    // Positions
    glBindBuffer(GL_ARRAY_BUFFER, control_renderer->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);

    glBindVertexArray(0);
    
    control_renderer->shader = load_shader("shaders/static.vert", "shaders/static.frag");
}

void render_control_point(vec3 p, float size, vec4 color) {
    glUseProgram(control_renderer->shader);
    
    mat4x4 mvp;
    get_view_projection_matrix(&camera, mvp);
    
    GLint mvp_loc = glGetUniformLocation(control_renderer->shader, "mvp");
    glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, (float*)mvp);
     
    GLint color_loc = glGetUniformLocation(control_renderer->shader, "color");
    glUniform4fv(color_loc, 1, color);
    
    glPointSize(size);
    glBindVertexArray(control_renderer->vao);

    glBindBuffer(GL_ARRAY_BUFFER, control_renderer->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3), p, GL_DYNAMIC_DRAW);
    
    glDepthFunc(GL_ALWAYS);
    {
        glDrawArrays(GL_POINTS, 0, 1);
    }
    glDepthFunc(GL_LEQUAL);
}

void render_control_line(vec3 a, vec3 b, vec4 color) {
    vec3 vertices[2];
    vec3_copy(vertices[0], a);
    vec3_copy(vertices[1], b);
    
    glUseProgram(control_renderer->shader);
    
    mat4x4 mvp;
    get_view_projection_matrix(&camera, mvp);
    
    GLint mvp_loc = glGetUniformLocation(control_renderer->shader, "mvp");
    glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, (float*)mvp);
     
    GLint color_loc = glGetUniformLocation(control_renderer->shader, "color");
    glUniform4fv(color_loc, 1, color);
    
    glBindVertexArray(control_renderer->vao);

    glBindBuffer(GL_ARRAY_BUFFER, control_renderer->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glDepthFunc(GL_ALWAYS);
    {
        glDrawArrays(GL_LINES, 0, 2);
    }
    glDepthFunc(GL_LEQUAL);  
}

void render_control_plane(vec3 origin, vec3 normal, float width, float height, vec4 color) {
    vec3 other;
    vec3_set(other, 0.0f, 1.0f, 0.0f);

    float dot = vec3_dot(other, normal);
    if (fabs(fabs(dot) - 1.0f) < 0.0001f)
        vec3_set(other, 0.0f, 0.0f, -1.0f);

    vec3 x_axis;
    vec3_cross(x_axis, other, normal);
    vec3_normalize(x_axis, x_axis);
    vec3_scale(x_axis, x_axis, width / 2.0f);

    vec3 y_axis;
    vec3_cross(y_axis, normal, x_axis);
    vec3_normalize(y_axis, y_axis);
    vec3_scale(y_axis, y_axis, height / 2.0f);

    vec3 tl, tr, bl, br;
    vec3_sub(tl, origin, x_axis);
    vec3_add(tl, tl, y_axis);
    
    vec3_add(tr, origin, x_axis);
    vec3_add(tr, tr, y_axis);
    
    vec3_sub(bl, origin, x_axis);
    vec3_sub(bl, bl, y_axis);
    
    vec3_add(br, origin, x_axis);
    vec3_sub(br, br, y_axis);

    vec3 vertices[4];
    vec3_copy(vertices[0], tl);
    vec3_copy(vertices[1], tr);
    vec3_copy(vertices[2], br);
    vec3_copy(vertices[3], bl);

    glUseProgram(control_renderer->shader);
    
    mat4x4 mvp;
    get_view_projection_matrix(&camera, mvp);
    
    GLint mvp_loc = glGetUniformLocation(control_renderer->shader, "mvp");
    glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, (float*)mvp);
     
    GLint color_loc = glGetUniformLocation(control_renderer->shader, "color");
    glUniform4fv(color_loc, 1, color);
    
    glBindVertexArray(control_renderer->vao);

    glBindBuffer(GL_ARRAY_BUFFER, control_renderer->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void render_control_circle(vec3 origin, vec3 normal, float radius, vec4 color) {
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

    int subs = 24;
    float step = (M_PI * 2) / subs;

    vec3 vertices[subs];
    for (int i = 0; i < subs; i++) {
        float x = cosf(step * i) * radius;
        float y = sinf(step * i) * radius;

        vec3 x_aligned, y_aligned;
        vec3_scale(x_aligned, x_axis, x);
        vec3_scale(y_aligned, y_axis, y);
        
        vec3_add(vertices[i], x_aligned, y_aligned);
        vec3_add(vertices[i], vertices[i], origin);
    }

    glUseProgram(control_renderer->shader);
    
    mat4x4 mvp;
    get_view_projection_matrix(&camera, mvp);
    
    GLint mvp_loc = glGetUniformLocation(control_renderer->shader, "mvp");
    glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, (float*)mvp);
     
    GLint color_loc = glGetUniformLocation(control_renderer->shader, "color");
    glUniform4fv(color_loc, 1, color);
    
    glBindVertexArray(control_renderer->vao);

    glBindBuffer(GL_ARRAY_BUFFER, control_renderer->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glDepthFunc(GL_ALWAYS);
    {
        glDrawArrays(GL_LINE_LOOP, 0, subs);
    }
    glDepthFunc(GL_LEQUAL);
}
