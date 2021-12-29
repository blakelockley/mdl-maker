#include "filemanager.h"

#include <stdio.h>

void open_file(char *filename, object_t *object) {
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

            add_index(object, v1);
            add_index(object, v2);
            add_index(object, v3);
        } else {
            vec3 pos;
            sscanf(line, "%f %f %f", &pos[0], &pos[1], &pos[2]);

            add_position(object, pos);
        }
    }

    fclose(file);
}

void save_file(char *filename, object_t *model) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        return;
    }

    for (int i = 0; i < model->positions_len; i++) {
        vec3 pos;
        vec3_copy(pos, model->positions[i]);

        fprintf(file, "%f %f %f\n", pos[0], pos[1], pos[2]);
    }

    for (int i = 0; i < model->indices_len; i += 3)
        fprintf(file, "f %d %d %d\n", model->indices[i], model->indices[i + 1], model->indices[i + 2]);

    fclose(file);
}