#ifndef _TEXT_H
#define _TEXT_H

#include "glfw.h"
#include "linmath.h"

/**
 * Initialse text shader and renderer.
 */
void init_text();
void free_text();

/**
 * Render text to the screen at given position.
 * 
 * @param text {char *} Text to be rendered.
 * @param pos {vec2} Position in screen coords to render text.
 */
void render_text(char *text, int x, int y, int width, int height);

#endif