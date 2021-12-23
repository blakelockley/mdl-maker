#include "text.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shader.h"
#include "stb_image.h"

static int has_init = 0;
static GLuint program;
static GLuint texture;
static GLuint vao, ebo, vbo[2];

static unsigned char *image;

float pos_coords[8];

float tex_coords[] = {
    0.0f, 0.0f,  // top left
    0.0f, 1.0f,  // bottom left
    1.0f, 0.0f,  // top right
    1.0f, 1.0f,  // bottom right
};

int indicies[6] = {
    0, 1, 2,
    3, 2, 1};

void init_text() {
    // Check and set init flag
    if (has_init)
        return;

    has_init = 1;
    program = load_shader("shaders/text_vert.glsl", "shaders/text_frag.glsl");

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);

    glGenBuffers(2, vbo);

    // vPos
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(pos_coords[0]) * 2, (void *)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pos_coords), pos_coords, GL_STREAM_DRAW);

    // vTex
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(tex_coords[0]) * 2, (void *)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coords), tex_coords, GL_STATIC_DRAW);

    int w, h, n;
    image = stbi_load("rsc/characters.png", &w, &h, &n, STBI_rgb_alpha);

    glGenTextures(1, &texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glPixelStorei(GL_UNPACK_ROW_LENGTH, w);
}

void free_text() {
    stbi_image_free(image);
}

void render_text(char *text, int x, int y, int width, int height) {
    const float pos_x = (x - width / 2) / (float)width * 2;
    const float pos_y = (y + height / 2) / (float)height * 2;
    const float scale = 6;

    const float char_width = 8.0f / width * scale;
    const float char_height = 8.0f / height * scale;

    glUseProgram(program);
    glBindVertexArray(vao);

    vec2 pos = {pos_x, pos_y};

    for (int i = 0; i < strlen(text); i++) {
        // Validate char

        if (text[i] == '\n') {
            pos[0] = pos_x;
            pos[1] -= char_height;
            continue;
        }

        if (!(32 <= text[i] && text[i] < 127))
            continue;

        // Set quad position

        // left, top
        pos_coords[0] = pos[0];
        pos_coords[1] = pos[1];

        // left, bottom
        pos_coords[2] = pos[0];
        pos_coords[3] = pos[1] - char_height;

        // right, top
        pos_coords[4] = pos[0] + char_width;
        pos_coords[5] = pos[1];

        // right, bottom
        pos_coords[6] = pos[0] + char_width;
        pos_coords[7] = pos[1] - char_height;

        // Set texture position
        const char ch = text[i] - 32;
        const int ch_x = (ch % 16) * 8;
        const int ch_y = (ch / 16) * 8;

        glPixelStorei(GL_UNPACK_SKIP_PIXELS, ch_x);
        glPixelStorei(GL_UNPACK_SKIP_ROWS, ch_y);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 8, 8, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);

        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(pos_coords), pos_coords, GL_STREAM_DRAW);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *)0);

        pos[0] += char_width;
    }
}