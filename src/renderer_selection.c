#include "renderers.h"
#include "shader.h"
#include "camera.h"
#include "light.h"
#include "selection.h"

extern GLFWwindow *window;
extern camera_t camera;
extern light_t light;

static renderer_t _renderer;
static renderer_t *renderer = &_renderer;

// Converts the coordinates from screen space: [0, width/height] to clip space: [-1.0f, 1.0f]
void normalize_screen_coords(GLFWwindow *window, float *clip_x, float *clip_y, float screen_x, float screen_y) {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    
    *clip_x = (2.0f * screen_x) / width - 1.0f;
    *clip_y = 1.0f - (2.0f * screen_y) / height;
}

void deinit_selection_renderer() {
    deinit_renderer(renderer);
}

renderer_t* init_selection_renderer() {
    init_renderer(renderer, 1);
    
    // Vertices
    glGenBuffers(1, renderer->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * 4, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void *)0);

    glBindVertexArray(0);

    renderer->shader = load_shader("shaders/gui.vert", "shaders/gui.frag");
    return renderer;
}

void render_selection_box(float ax, float ay, float bx, float by, vec3 color) {
    float clip_ax, clip_ay;
    normalize_screen_coords(window, &clip_ax, &clip_ay, ax, ay);
        
    float clip_bx, clip_by;
    normalize_screen_coords(window, &clip_bx, &clip_by, bx, by);
    
    vec2 vertices[4];
    vec2_set(vertices[0], clip_ax, clip_ay);
    vec2_set(vertices[1], clip_bx, clip_ay);
    vec2_set(vertices[2], clip_bx, clip_by);
    vec2_set(vertices[3], clip_ax, clip_by);

    glBindVertexArray(renderer->vao);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glUseProgram(renderer->shader);

    vec4 color4;
    vec4_from_vec3(color4, color, 1.0f);
    
    GLint color_loc = glGetUniformLocation(renderer->shader, "color");
    glUniform4fv(color_loc, 1, color4);

    glDrawArrays(GL_LINE_LOOP, 0, 4);
    
    glBindVertexArray(0);
}

void render_selection_handle(float x, float y, float size, vec3 color) {
    float clip_ax, clip_ay;
    normalize_screen_coords(window, &clip_ax, &clip_ay, x, y);
    
    vec2 vertices[1];
    vec2_set(vertices[0], clip_ax, clip_ay);

    glBindVertexArray(renderer->vao);

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glUseProgram(renderer->shader);

    vec4 color4;
    vec4_from_vec3(color4, color, 1.0f);
    
    GLint color_loc = glGetUniformLocation(renderer->shader, "color");
    glUniform4fv(color_loc, 1, color4);

    glPointSize(size);
    glDrawArrays(GL_POINTS, 0, 1);
    
    glBindVertexArray(0);
}
