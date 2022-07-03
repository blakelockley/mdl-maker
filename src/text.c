#include "text.h"
#include "glfw.h"
#include "shader.h"
#include "viewport.h"

glyph_t glyphs[128];
unsigned int font_size = 24;

static GLuint vao, vbo; 
static GLuint shader;

extern viewport_t viewport;

void init_text() {
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        fprintf(stderr, "[ERROR] Could not init FreeType Library\n");
        return;
    }

    FT_Face face;
    if (FT_New_Face(ft, "assets/OpenSans-Regular.ttf", 0, &face)) {
        fprintf(stderr, "[ERROR] Failed to load font\n");
        return;
    }

    FT_Set_Pixel_Sizes(face, 0, font_size);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
  
    for (unsigned char ch = 0; ch < 128; ch++) {
        if (FT_Load_Char(face, ch, FT_LOAD_RENDER)) {
            fprintf(stderr, "[ERROR] Failed to load Glyph\n");
            continue;
        }
        
        unsigned int tex_id = -1;
        if (face->glyph->bitmap.width) {
            glGenTextures(1, &tex_id);
            glBindTexture(GL_TEXTURE_2D, tex_id);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );
        }
        
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        glyph_t glyph;
        glyph.tex_id = tex_id;
        vec2_set(glyph.size, face->glyph->bitmap.width, face->glyph->bitmap.rows);
        vec2_set(glyph.bearing, face->glyph->bitmap_left, face->glyph->bitmap_top);
        glyph.advance = face->glyph->advance.x;

        glyphs[ch] = glyph;
    }
    
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    shader = load_shader("shaders/text.vert", "shaders/text.frag");

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

void render_text(char *text, vec2 pos, vec3 color) {
    glUseProgram(shader);

    GLint color_loc = glGetUniformLocation(shader, "textColor");
    glUniform3fv(color_loc, 1, (float *)color);

    mat4x4 projection;
    mat4x4_ortho(projection, 0.0f, viewport.width, 0.0f, viewport.height, -1.0f, 1.0f);

    GLint projection_loc = glGetUniformLocation(shader, "projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float *)projection);
    
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(vao);

    float line_height = font_size;
    float baseline = line_height / 4;

    int i = 0;
    while (text[i]) {
        unsigned int ch = (unsigned int) text[i++];
        
        if (!(0 <= ch && ch <= 128))
            continue;

        glyph_t glyph = glyphs[ch];
        
        float w = glyph.size[0];
        float h = glyph.size[1];

        float xpos = pos[0] + glyph.bearing[0];
        float ypos = viewport.height - pos[1] - (line_height - baseline) - (h - glyph.bearing[1]);

        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };
        
        pos[0] += glyph.advance / 64.0f;

        if (!w) // If no width, dont render texture
            continue;

        glBindTexture(GL_TEXTURE_2D, glyph.tex_id);
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void free_text() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}
