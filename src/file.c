#include "file.h"

#include <stdio.h>
#include <stdlib.h>

enum _section_t {
    SECTION_VERTICES = 0,
    SECTION_FACES = 1,
    SECTION_PALETTE = 2,
};

typedef enum _section_t section_t;

void open_file(char *filename, model_t *model) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("[ERROR]: Could not open file \"%s\".\n", filename);
        return;
    }

    size_t buffer_size = 1024;
    uint8_t *buffer = malloc(buffer_size);

    uint8_t section;
    uint32_t len;

    // Vertices

    fread(buffer, sizeof(uint8_t), 1, file);
    section = (uint8_t)buffer[0];

    if (section != SECTION_VERTICES) {
        printf("[ERROR]: Expected section \"%d\", got \"%d\".\n", SECTION_VERTICES, section);
        return;
    }
     
    fread(buffer, sizeof(uint32_t), 1, file);
    len = *(uint32_t *)&buffer[0];

    if (len * sizeof(vec3) > buffer_size) {
        buffer_size = len * sizeof(vec3);
        buffer = realloc(buffer, buffer_size);
    }

    fread(buffer, sizeof(vec3), len, file);
    load_vertices(model, (vec3 *)buffer, len);

    // Faces

    fread(buffer, sizeof(uint8_t), 1, file);
    section = (uint8_t)buffer[0];

    if (section != SECTION_FACES) {
        printf("[ERROR]: Expected section \"%d\", got \"%d\".\n", SECTION_FACES, section);
        return;
    }

    fread(buffer, sizeof(uint32_t), 1, file);
    len = *(uint32_t *)&buffer[0]; // Number of faces in model. NOTE: faces are not uniformly sized

    for (int i = 0; i < len; i++) {
        fread(buffer, sizeof(uint32_t), 1, file);
        uint32_t face_len = *(uint32_t *)&buffer[0];

        if (face_len * sizeof(uint32_t) > buffer_size) {
            buffer_size = face_len * sizeof(uint32_t);
            buffer = realloc(buffer, buffer_size);
        }
    
        fread(buffer, sizeof(uint32_t), face_len, file);
        face_t *face = load_face(model, (uint32_t *)buffer, face_len);

        fread(buffer, sizeof(uint8_t), 1, file);
        face->color_index = *(uint8_t *)&buffer[0];
    }

    // Palette

    fread(buffer, sizeof(uint8_t), 1, file);
    section = (uint8_t)buffer[0];

    if (section != SECTION_PALETTE) {
        printf("[ERROR]: Expected section \"%d\", got \"%d\".\n", SECTION_PALETTE, section);
        return;
    }

    fread(buffer, sizeof(uint8_t), 1, file);
    len = *(uint8_t *)&buffer[0];

    if (len * sizeof(vec3) > buffer_size) {
        buffer_size = len * sizeof(vec3);
        buffer = realloc(buffer, buffer_size);
    }

    fread(buffer, sizeof(vec3), len, file);
    load_palette(model, (vec3 *)buffer, len);

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
    fwrite(&model->faces_len, sizeof(uint32_t), 1, file);

    for (int i = 0; i < model->faces_len; i++) {
        face_t *face = &model->faces[i];
        
        // Indices
        fwrite(&face->len, sizeof(uint32_t), 1, file);
        fwrite(face->indices, sizeof(uint32_t), face->len, file);
        
        // Color index
        fwrite(&face->color_index, sizeof(uint8_t), 1, file);
    }

    section = SECTION_PALETTE;
    fwrite(&section, sizeof(uint8_t), 1, file);
    fwrite(&model->palette_len, sizeof(uint8_t), 1, file);

    for (int i = 0; i < model->palette_len; i++)
        fwrite(model->palette[i], sizeof(float), 3, file);

    fclose(file);
}