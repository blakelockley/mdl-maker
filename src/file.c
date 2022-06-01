#include "file.h"

#include <stdio.h>
#include <stdlib.h>

enum _section_t {
    SECTION_VERTICES = 0,
    SECTION_FACES = 1,
};

typedef enum _section_t section_t;

void open_file(char *filename, model_t *model) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        return;
    }

    size_t buffer_len = 1024;
    char *buffer = malloc(sizeof(char) * buffer_len);

    uint8_t section;
    uint32_t len;

    while (fread(buffer, sizeof(uint8_t), 1, file) == 1) {
        section = (uint8_t)buffer[0];

        if (section == SECTION_VERTICES) {
            fread(buffer, sizeof(uint32_t), 1, file);
            len = *(uint32_t *)&buffer[0];

            if (len * sizeof(vec3) > buffer_len)
                buffer = realloc(buffer, len * sizeof(vec3));

            fread(buffer, sizeof(vec3), len, file);
            for (int i = 0; i < len; i++)
                add_vertex(model, *(vec3 *)&buffer[i * sizeof(vec3)]);
        }

        if (section == SECTION_FACES) {
            fread(buffer, sizeof(uint32_t), 1, file);
            len = *(uint32_t *)&buffer[0];

            if (len * sizeof(uint32_t) > buffer_len)
                buffer = realloc(buffer, len * sizeof(uint32_t));

            fread(buffer, sizeof(uint32_t), len, file);
            for (int i = 0; i < len;) {
                uint32_t len = *(uint32_t *)&buffer[i * sizeof(uint32_t)];
                uint32_t *indices = (uint32_t *)&buffer[(i + 1) * sizeof(uint32_t)];
                
                add_face(model, indices, len);
                i += (1 + len);
            }
        }
    }

    fclose(file);
}

void save_file(char *filename, model_t *model) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        return;
    }

    uint8_t section;

    section = SECTION_VERTICES;
    fwrite(&section, sizeof(uint8_t), 1, file);
    fwrite(&model->vertices_len, sizeof(uint32_t), 1, file);

    for (int i = 0; i < model->vertices_len; i++)
        fwrite(model->vertices[i], sizeof(float), 3, file);

    section = SECTION_FACES;
    fwrite(&section, sizeof(uint8_t), 1, file);

    uint32_t total_len = 0; 
    for (int i = 0; i < model->faces_len; i++)
        total_len += (model->faces[i].len + 1); // +1 for the length of the face
    
    fwrite(&total_len, sizeof(uint32_t), 1, file);

    for (int i = 0; i < model->faces_len; i++) {
        face_t *face = &model->faces[i];
        fwrite(&face->len, sizeof(uint32_t), 1, file);
        fwrite(face->indices, sizeof(uint32_t), face->len, file);
    }

    fclose(file);
}