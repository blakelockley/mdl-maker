#include "filemanager.h"

#include <stdio.h>
#include <stdlib.h>

enum _flag_t {
    FLAG_POSITIONS = 0,
    FLAG_INDICES = 1,
    FLAG_COLOR = 2,
};

typedef enum _flag_t flag_t;

void open_file(char *filename, object_t *object) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        return;
    }

    size_t buffer_len = 1024;
    char *buffer = malloc(sizeof(char) * buffer_len);

    uint8_t flag;
    uint32_t len;

    while (fread(buffer, sizeof(uint8_t), 1, file) == 1) {
        flag = (uint8_t)buffer[0];

        if (flag == FLAG_POSITIONS) {
            fread(buffer, sizeof(uint32_t), 1, file);
            len = *(uint32_t *)&buffer[0];

            if (len * sizeof(vec3) > buffer_len)
                buffer = realloc(buffer, len * sizeof(vec3));

            fread(buffer, sizeof(vec3), len, file);
            for (int i = 0; i < len; i++) {
                vec3 position;
                vec3_copy(position, *(vec3 *)&buffer[i * sizeof(vec3)]);

                add_position(object, position);
            }
        }

        if (flag == FLAG_INDICES) {
            fread(buffer, sizeof(uint32_t), 1, file);
            len = *(uint32_t *)&buffer[0];

            if (len * sizeof(uint32_t) > buffer_len)
                buffer = realloc(buffer, len * sizeof(uint32_t));

            fread(buffer, sizeof(uint32_t), len, file);
            for (int i = 0; i < len; i++) {
                uint32_t index = *(uint32_t *)&buffer[i * sizeof(uint32_t)];

                add_index(object, index);
            }
        }
    }

    fclose(file);
}

void save_file(char *filename, object_t *model) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        return;
    }

    uint8_t flag;

    flag = FLAG_POSITIONS;
    fwrite(&flag, sizeof(uint8_t), 1, file);
    fwrite(&model->positions_len, sizeof(uint32_t), 1, file);

    for (int i = 0; i < model->positions_len; i++)
        fwrite(model->positions[i], sizeof(float), 3, file);

    flag = FLAG_INDICES;
    fwrite(&flag, sizeof(uint8_t), 1, file);
    fwrite(&model->indices_len, sizeof(uint32_t), 1, file);

    for (int i = 0; i < model->indices_len; i++)
        fwrite(&model->indices[i], sizeof(uint32_t), 1, file);

    fclose(file);
}