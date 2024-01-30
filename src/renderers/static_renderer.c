#include <stdlib.h>

#include "renderers.h"
#include "linmath.h"

#include "camera.h"
#include "shader.h"

static renderer_t _static_renderer;
static renderer_t *static_renderer = &_static_renderer;

void init_static_renderer() {
    init_renderer(static_renderer, 1);

    // Positions
    glBindBuffer(GL_ARRAY_BUFFER, static_renderer->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);

    glBindVertexArray(0);
    
    static_renderer->shader = load_shader("shaders/static.vert", "shaders/static.frag");
}

void render_plane(vec3 origin, vec3 normal, float width, float height, vec4 color) {
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

    glUseProgram(static_renderer->shader);
    
    mat4x4 mvp;
    get_view_projection_matrix(mvp);
    
    GLint mvp_loc = glGetUniformLocation(static_renderer->shader, "mvp");
    glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, (float*)mvp);
     
    GLint color_loc = glGetUniformLocation(static_renderer->shader, "color");
    glUniform4fv(color_loc, 1, color);
    
    glBindVertexArray(static_renderer->vao);

    glBindBuffer(GL_ARRAY_BUFFER, static_renderer->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}
