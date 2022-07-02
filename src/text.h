#ifndef TEXT_H
#define TEXT_H

#include "ft2build.h"
#include FT_FREETYPE_H

#include "linmath.h"

struct _glyph_t {
    unsigned int tex_id;
    vec2 size;
    vec2 bearing;
    float advance;
};

typedef struct _glyph_t glyph_t;

void init_text();
void free_text();

void render_text(char *text, vec2 pos, vec3 color);

#endif  // TEXT_H
