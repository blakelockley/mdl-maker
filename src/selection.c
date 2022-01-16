#include "selection.h"

#include "camera.h"
#include "shader.h"

selection_t selection;
extern camera_t camera;

void init_selection() {
    // clang-format off
    uint32_t indices[] = {
        0, 1, 1, 2, 2, 3, 3, 0,
        0, 4, 1, 5, 2, 6, 3, 7,
        4, 5, 5, 6, 6, 7, 7, 4,
    };
    // clang-format on

    glGenVertexArrays(1, &selection.vao);
    glBindVertexArray(selection.vao);

    // Vertices
    glGenBuffers(1, &selection.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, selection.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * 8, NULL, GL_DYNAMIC_DRAW);

    // Indices
    glGenBuffers(1, &selection.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, selection.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *)0);  // Attrib pointer for currently bound buffer

    selection.shader = load_shader("shaders/static.vert", "shaders/static.frag");
}

void draw_selection() {
    glUseProgram(selection.shader);

    mat4x4 model, view, projection;
    mat4x4_identity(model);
    mat4x4_look_at(view, camera.pos, camera.dir, camera.up);
    get_projection_matrix(projection);

    GLint model_loc = glGetUniformLocation(selection.shader, "model");
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, (float *)model);

    GLint view_loc = glGetUniformLocation(selection.shader, "view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, (float *)view);

    GLint projection_loc = glGetUniformLocation(selection.shader, "projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float *)projection);

    GLint color_loc = glGetUniformLocation(selection.shader, "color");
    glUniform3f(color_loc, 0.25f, 0.15f, 0.75f);

    glBindVertexArray(selection.vao);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
}

void free_selection() {
    glDeleteVertexArrays(1, &selection.vao);
    glDeleteBuffers(1, &selection.vbo);
    glDeleteBuffers(1, &selection.ebo);
}

void buffer_selection() {
    vec3 vertices[8];

    vec3 front_tl_ray, front_br_ray;
    vec3_scale(front_tl_ray, selection.tl_dir, 0.1f);
    vec3_scale(front_br_ray, selection.br_dir, 0.1f);

    vec3 front_tl, front_br;
    vec3_add(front_tl, selection.tl_pos, front_tl_ray);
    vec3_add(front_br, selection.br_pos, front_br_ray);

    // Front
    vec3_copy(vertices[0], front_tl);
    vec3_copy(vertices[1], front_tl);
    vertices[1][1] = front_br[1];

    vec3_copy(vertices[2], front_br);
    vec3_copy(vertices[3], front_br);
    vertices[3][1] = front_tl[1];

    vec3 back_tl_ray, back_br_ray;
    vec3_scale(back_tl_ray, selection.tl_dir, 5.0f);
    vec3_scale(back_br_ray, selection.br_dir, 5.0f);

    vec3 back_tl, back_br;
    vec3_add(back_tl, selection.tl_pos, back_tl_ray);
    vec3_add(back_br, selection.br_pos, back_br_ray);

    vec3_copy(vertices[4], back_tl);
    vec3_copy(vertices[5], back_tl);
    vertices[5][1] = back_br[1];

    vec3_copy(vertices[6], back_br);
    vec3_copy(vertices[7], back_br);
    vertices[7][1] = back_tl[1];

    glBindVertexArray(selection.vao);

    glBindBuffer(GL_ARRAY_BUFFER, selection.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

void calculate_ray(vec3 start, vec3 dir, double mouse_x, double mouse_y, int width, int height) {
    double normal_x = (2.0f * mouse_x) / width - 1.0f;
    double normal_y = 1.0f - (2.0f * mouse_y) / height;

    vec4 ray_start = (vec4){(float)normal_x, (float)normal_y, -1.0f, 1.0f};
    vec4 ray_end = (vec4){(float)normal_x, (float)normal_y, 0.0f, 1.0f};

    mat4x4 view, projection;
    mat4x4_look_at(view, camera.pos, camera.dir, camera.up);
    get_projection_matrix(projection);

    mat4x4_invert(view, view);
    mat4x4_invert(projection, projection);

    mat4x4_mul_vec4(ray_start, projection, ray_start);
    vec4_scale(ray_start, ray_start, 1.0f / ray_start[3]);

    mat4x4_mul_vec4(ray_start, view, ray_start);
    vec4_scale(ray_start, ray_start, 1.0f / ray_start[3]);

    mat4x4_mul_vec4(ray_end, projection, ray_end);
    vec4_scale(ray_end, ray_end, 1.0f / ray_end[3]);

    mat4x4_mul_vec4(ray_end, view, ray_end);
    vec4_scale(ray_end, ray_end, 1.0f / ray_end[3]);

    vec3 ray_dir;
    vec3_sub(ray_dir, ray_end, ray_start);
    vec3_normalize(ray_dir, ray_dir);

    vec3_copy(start, ray_start);
    vec3_copy(dir, ray_dir);
}

void set_selection_start(double mouse_x, double mouse_y, int width, int height) {
    vec3 start, dir;
    calculate_ray(start, dir, mouse_x, mouse_y, width, height);

    vec3_copy(selection.tl_pos, start);
    vec3_copy(selection.tl_dir, dir);

    vec3_copy(selection.br_pos, start);
    vec3_copy(selection.br_dir, dir);

    buffer_selection();
}

void set_selection_end(double mouse_x, double mouse_y, int width, int height) {
    vec3 start, dir;
    calculate_ray(start, dir, mouse_x, mouse_y, width, height);

    vec3_copy(selection.br_pos, start);
    vec3_copy(selection.br_dir, dir);

    buffer_selection();
}
