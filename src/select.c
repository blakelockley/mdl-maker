#include "select.h"

#include <stdio.h>

#include "camera.h"
#include "shader.h"

select_t select;
extern camera_t camera;

void init_select() {
    select.is_visible = 0;

    glGenVertexArrays(1, &select.vao);
    glBindVertexArray(select.vao);

    // Vertices
    glGenBuffers(1, &select.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, select.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * 4, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void *)0);  // Attrib pointer for currently bound buffer

    select.shader = load_shader("shaders/gui.vert", "shaders/gui.frag");
}

void draw_select() {
    if (!select.is_visible)
        return;

    glUseProgram(select.shader);

    GLint color_loc = glGetUniformLocation(select.shader, "color");
    glUniform3f(color_loc, 0.8f, 0.4f, 0.2f);

    glBindVertexArray(select.vao);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
}

void free_select() {
    glDeleteVertexArrays(1, &select.vao);
    glDeleteBuffers(1, &select.vbo);
}

void buffer_select() {
    vec2 vertices[4];

    vec2_set(vertices[0], select.ax, select.ay);
    vec2_set(vertices[1], select.bx, select.ay);
    vec2_set(vertices[2], select.bx, select.by);
    vec2_set(vertices[3], select.ax, select.by);

    glBindVertexArray(select.vao);

    glBindBuffer(GL_ARRAY_BUFFER, select.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
}

void set_select_start(double x, double y) {
    select.is_visible = 0;
    select.ax = x;
    select.ay = y;

    buffer_select();
}

void set_select_move(double x, double y) {
    select.is_visible = 1;
    select.bx = x;
    select.by = y;

    buffer_select();  
}

void set_select_end(double x, double y) {
    select.is_visible = 0;
    buffer_select();  
}