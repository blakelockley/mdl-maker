#include "verticespanel.h"

#include <stdio.h>

#include "text.h"

void render_vertices_panel(model_t* model, int width, int height) {
    glEnable(GL_SCISSOR_TEST);
    glScissor(width - 600, 0, 600, height);
    glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    glViewport(width - 600, 0, 600, height);
    render_text("VERTICES", 0, 0, 600, height);

    for (int i = 0; i < model->vertices_len; i++) {
        char text[100];
        sprintf(text, "%d:%.3f,%.3f,%.3f", i, model->vertices[i][0], model->vertices[i][1], model->vertices[i][2]);
        render_text(text, 0, -(i + 1) * 40, 600, height);
    }
}
