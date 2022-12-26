#include "file.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum _section_t {
    SECTION_VERTICES = 0,
    SECTION_FACES = 1,
    SECTION_COLORS = 2,
};

typedef enum _section_t section_t;

void open_file(char *filename, model_t *model) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("[ERROR]: Could not open file \"%s\".\n", filename);
        return;
    }

    size_t buffer_size = 1024;
    char *buffer = malloc(buffer_size);

    uint32_t face_len;
    uint32_t face_buffer[32];

    uint8_t section = SECTION_VERTICES;

    while (fgets(buffer, buffer_size, file) != NULL) {
        if (buffer[0] == '\0' || buffer[0] == '#')
            continue;

        // check for section header
        
        if (strncmp(buffer, "[vertices]", strlen("[vertices]")) == 0) {
            section = SECTION_VERTICES;
            continue;
        }

        if (strncmp(buffer, "[faces]", strlen("[faces]")) == 0) {
            section = SECTION_FACES;
            continue;
        }

        if (strncmp(buffer, "[colors]", strlen("[colors]")) == 0) {
            section = SECTION_COLORS;
            continue;
        }

        // parse section data

        if (section == SECTION_VERTICES) {
            vec3 vec;
            sscanf(buffer, "%f %f %f", &vec[0], &vec[1], &vec[2]);

            add_vertex(model, vec);
        }
        
        if (section == SECTION_FACES) {
            face_len = 0;
            
            char *token = strtok(buffer, " ");
            face_buffer[face_len++] = atoi(token);
            
            while ((token = strtok(NULL, " ")) && face_len < 32)
                face_buffer[face_len++] = atoi(token);

            add_face(model, face_buffer, face_len);
        }
        
        if (section == SECTION_COLORS) {
            static int face_index = 0;
            if (face_index >= model->faces_len)
                continue;
            
            vec3 color;
            sscanf(buffer, "%f %f %f", &color[0], &color[1], &color[2]);

            set_face_color(model, face_index++, color);
        }
    }

    fclose(file);
}

void open_file_binary(char *filename, model_t *model) {
    FILE *file = fopen(filename, "r");
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
    // load_vertices(model, (vec3 *)buffer, len);

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
        // load_face(model, (uint32_t *)buffer, face_len);
    }

    fread(buffer, sizeof(vec3), len, file);

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
        
        // Color
        fwrite(&face->color, sizeof(float), 3, file);
    }

    fclose(file);
}