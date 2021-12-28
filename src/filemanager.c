#include "filemanager.h"

#include <stdio.h>

void open_file(char *filename, model_t *model) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == 'f') {
            int v1, v2, v3;
            sscanf(line, "f %d %d %d", &v1, &v2, &v3);

            add_index(model, v1);
            add_index(model, v2);
            add_index(model, v3);
        } else {
            vec3 vertex;
            sscanf(line, "%f %f %f", &vertex[0], &vertex[1], &vertex[2]);

            add_vertex(model, vertex);
        }
    }

    fclose(file);
}

void save_file(char *filename, model_t *model) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        return;
    }

    for (int i = 0; i < model->vertices_len; i++) {
        vec3 vertex;
        vec3_copy(vertex, model->vertices[i]);

        fprintf(file, "%f %f %f\n", vertex[0], vertex[1], vertex[2]);
    }

    for (int i = 0; i < model->indices_len; i += 3)
        fprintf(file, "f %d %d %d\n", model->indices[i], model->indices[i + 1], model->indices[i + 2]);

    fclose(file);
}