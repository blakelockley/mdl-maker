#include "quad.h"
#include "glfw.h"
#include "shader.h"
#include "viewport.h"

static GLuint vao, vbo; 
static GLuint shader;

extern viewport_t viewport;

void init_quad() {
    shader = load_shader("shaders/quad.vert", "shaders/quad.frag");

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glBindVertexArray(0);  
}

void render_quad(vec2 topLeft, vec2 bottomRight, vec3 color) {
    glUseProgram(shader);

    GLint color_loc = glGetUniformLocation(shader, "color");
    glUniform3fv(color_loc, 1, (float *)color);

    mat4x4 projection;
    mat4x4_ortho(projection, 0.0f, viewport.width, 0.0f, viewport.height, -1.0f, 1.0f);

    GLint projection_loc = glGetUniformLocation(shader, "projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float *)projection);
    
    glBindVertexArray(vao);

    topLeft[1] = viewport.height - topLeft[1];
    bottomRight[1] = viewport.height - bottomRight[1];

    float vertices[6][4] = {
        { topLeft[0],     bottomRight[1],   0.0f, 0.0f },            
        { topLeft[0],     topLeft[1],       0.0f, 1.0f },
        { bottomRight[0], topLeft[1],       1.0f, 1.0f },
        { topLeft[0],     bottomRight[1],   0.0f, 0.0f },
        { bottomRight[0], topLeft[1],       1.0f, 1.0f },
        { bottomRight[0], bottomRight[1],   1.0f, 0.0f }           
    };
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
}

void free_quad() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}
