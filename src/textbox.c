#include "textbox.h"
#include "text.h"
#include "quad.h"

extern glyph_t glyphs[];
extern unsigned int font_size;


void render_textbox(char *text, vec2 pos, vec3 text_color, vec4 bg_color) {

    float intrinsic_width = 0.0f;
    float line_height = font_size;

    int i = 0;
    while (text[i]) {
        unsigned int ch = (unsigned int) text[i++];
        
        if (!(0 <= ch && ch <= 128))
            continue;

        glyph_t glyph = glyphs[ch];
        intrinsic_width += glyph.advance / 64.0f;
    }
    
    // Padding
    float px = 12.0f;
    float py = 8.0f;

    render_quad((vec2){ pos[0], pos[1] },
                (vec2){ pos[0] + intrinsic_width + (px * 2), pos[1] + line_height + (py * 2)},
                bg_color);

    render_text(text,
                (vec2){ pos[0] + px, pos[1] + py },
                text_color);
}
