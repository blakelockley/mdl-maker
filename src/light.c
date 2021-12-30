#include "light.h"

#include "camera.h"
#include "shader.h"

light_t light;
extern camera_t camera;
extern int width, height;

void init_light() {
    vec3_set(light.pos, 0.0f, 1.0f, 0.0f);
    vec3_set(light.color, 1.0f, 0.0f, 1.0f);

    glGenVertexArrays(1, &light.vao);
    glBindVertexArray(light.vao);

    // Positions
    glGenBuffers(1, &light.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, light.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3), light.pos, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);  // Positions

    light.shader = load_shader("shaders/static.vert", "shaders/static.frag");
}

void draw_light() {
    glUseProgram(light.shader);

    mat4x4 model, view, projection;
    mat4x4_identity(model);
    mat4x4_look_at(view, camera.pos, camera.dir, camera.up);
    mat4x4_perspective(projection, 45.0f, (float)width / (float)height, 0.1f, 100.0f);

    GLint model_loc = glGetUniformLocation(light.shader, "model");
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, (float*)model);

    GLint view_loc = glGetUniformLocation(light.shader, "view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, (float*)view);

    GLint projection_loc = glGetUniformLocation(light.shader, "projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float*)projection);

    GLint color_loc = glGetUniformLocation(light.shader, "color");
    glUniform3fv(color_loc, 1, (float*)light.color);

    glBindVertexArray(light.vao);

    glPointSize(40);
    glDrawArrays(GL_POINTS, 0, 1);
}

void free_light() {
    glDeleteVertexArrays(1, &light.vao);
    glDeleteBuffers(1, &light.vbo);
}

void set_light_position(vec3 position) {
    vec3_copy(light.pos, position);

    glBindVertexArray(light.vao);

    glBindBuffer(GL_ARRAY_BUFFER, light.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3), light.pos, GL_DYNAMIC_DRAW);
}

int check_light_intercept() {
    vec3 ray_start, ray_dir;
    vec3_copy(ray_start, camera.ray_start);
    vec3_copy(ray_dir, camera.ray);

    for (float t = 0.0f; t < 10.0f; t += 0.01f) {
        vec3 point;
        vec3_scale(point, ray_dir, t);
        vec3_add(point, point, ray_start);

        vec3 tmp;
        float dist;
        vec3_sub(tmp, point, light.pos);
        dist = vec3_len(tmp);

        if (dist < 0.025f)
            return 1;
    }

    return 0;
}