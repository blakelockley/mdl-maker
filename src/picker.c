#include "picker.h"

#include "camera.h"
#include "shader.h"

#include <stdlib.h>

extern GLFWwindow *window;

extern camera_t camera;

extern picker_t picker;
extern model_t model;

void init_picker(picker_t *picker) {
    picker->shader = load_shader("shaders/picker.vert", "shaders/picker.frag");

    glGenVertexArrays(1, &picker->vao);
    glBindVertexArray(picker->vao);

    glGenBuffers(2, picker->vbo);

    // Positions
    glBindBuffer(GL_ARRAY_BUFFER, picker->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
    
    // Face ID converted to float[3] as RGB
    glBindBuffer(GL_ARRAY_BUFFER, picker->vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);

    glBindVertexArray(0);
}

void free_picker(picker_t *picker) {
    glDeleteVertexArrays(1, &picker->vao);
    glDeleteBuffers(2, picker->vbo);
}

void id_to_color(vec3 ret, uint32_t id) {
    int r = (id & 0x000000FF) >>  0;
    int g = (id & 0x0000FF00) >>  8;
    int b = (id & 0x00FF0000) >> 16;

    vec3_set(ret, r/255.0f, g/255.0f, b/255.0f);
}

uint32_t render_picker_to_face_id(picker_t *picker, model_t *model) {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    uint32_t total_vertices = 0;
    for (int i = 0; i < model->faces_len; i++)
        total_vertices += (model->faces[i].len - 2) * 3; // n -> (n - 2) * 3

    vec3 positions[total_vertices];
    vec3 id_colors[total_vertices];
    
    uint32_t vi = 0; // vertices index
    for (int i = 0; i < model->faces_len; i++) {
        vec3 id_color;
        id_to_color(id_color, i);
        
        for (int j = 2; j < model->faces[i].len; j++) {
            uint32_t *indices = model->faces[i].indices;

            vec3_copy(positions[vi], model->vertices[indices[0]]);
            vec3_copy(id_colors[vi], id_color);
            vi++;
            
            vec3_copy(positions[vi], model->vertices[indices[j - 1]]);
            vec3_copy(id_colors[vi], id_color);
            vi++;
            
            vec3_copy(positions[vi], model->vertices[indices[j - 0]]);
            vec3_copy(id_colors[vi], id_color);
            vi++;
        }
    }

    glBindVertexArray(picker->vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, picker->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, picker->vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(id_colors), id_colors, GL_DYNAMIC_DRAW);

    glUseProgram(picker->shader);
    
    mat4x4 _model, view, projection;
    mat4x4_identity(_model);
    get_view_matrix(&camera, view);
    get_projection_matrix(&camera, projection);
    
    GLint model_loc = glGetUniformLocation(picker->shader, "model");
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, (float*)_model);

    GLint view_loc = glGetUniformLocation(picker->shader, "view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, (float*)view);

    GLint projection_loc = glGetUniformLocation(picker->shader, "projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float*)projection);
    
    glDrawArrays(GL_TRIANGLES, 0, total_vertices);
    glBindVertexArray(0);

    glFlush();
    glFinish();

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    float xscale, yscale;
    glfwGetWindowContentScale(window, &xscale, &yscale);

    int pixel_x = (int) floorf(xpos * xscale);
    int pixel_y = height - ((int) floorf(ypos * yscale));

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    unsigned char data[4];
    glReadPixels(pixel_x, pixel_y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

    uint32_t picked_id = data[0] + data[1] * 256 + data[2] * 256 * 256;
    
    if (picked_id == 0xFFFFFF)
        return INDEX_NOT_FOUND;

    return picked_id;
}

void render_picker_to_vertex_ids(picker_t *picker, model_t *model) {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(picker->shader);

    mat4x4 _model, view, projection;
    mat4x4_identity(_model);
    get_view_matrix(&camera, view);
    get_projection_matrix(&camera, projection);
    
    GLint model_loc = glGetUniformLocation(picker->shader, "model");
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, (float*)_model);

    GLint view_loc = glGetUniformLocation(picker->shader, "view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, (float*)view);

    GLint projection_loc = glGetUniformLocation(picker->shader, "projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float*)projection);

    glBindVertexArray(picker->vao);

    // Buffer and draw opaque faces

    uint32_t total_face_vertices = 0;
    for (int i = 0; i < model->faces_len; i++)
        total_face_vertices += (model->faces[i].len - 2) * 3; // n -> (n - 2) * 3

    vec3 face_positions[total_face_vertices];
    vec3 face_id_colors[total_face_vertices];
    
    vec3 face_id_color; // id converted to color
    vec3_set(face_id_color, 1.0f, 1.0f, 1.0f); // blank for opaque faces

    uint32_t vi = 0; // vertices index
    for (int i = 0; i < model->faces_len; i++) {
        for (int j = 2; j < model->faces[i].len; j++) {
            uint32_t *indices = model->faces[i].indices;

            vec3_copy(face_positions[vi], model->vertices[indices[0]]);
            vec3_copy(face_id_colors[vi], face_id_color);
            vi++;
            
            vec3_copy(face_positions[vi], model->vertices[indices[j - 1]]);
            vec3_copy(face_id_colors[vi], face_id_color);
            vi++;
            
            vec3_copy(face_positions[vi], model->vertices[indices[j - 0]]);
            vec3_copy(face_id_colors[vi], face_id_color);
            vi++;
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, picker->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(face_positions), face_positions, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, picker->vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(face_id_colors), face_id_colors, GL_DYNAMIC_DRAW);
    
    glDrawArrays(GL_TRIANGLES, 0, total_face_vertices);
    
    // Buffer and draw vertices with id colours

    uint32_t total_vertices = model->vertices_len;

    vec3 vertex_positions[total_vertices];
    vec3 vertex_color_ids[total_vertices];
    
    vec3 vertex_id_color; // id converted to color
    for (int i = 0; i < model->vertices_len; i++) {
        id_to_color(vertex_id_color, i);
        
        vec3_copy(vertex_positions[i], model->vertices[i]);
        vec3_copy(vertex_color_ids[i], vertex_id_color);
    }

    glBindBuffer(GL_ARRAY_BUFFER, picker->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_positions), vertex_positions, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, picker->vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_color_ids), vertex_color_ids, GL_DYNAMIC_DRAW);

    glPointSize(10);
    glDrawArrays(GL_POINTS, 0, model->vertices_len);

    glBindVertexArray(0);
}

void select_ids_in_rect(selection_t *selection, vec2 tl, vec2 br) {
    int frame_width, frame_height;
    glfwGetFramebufferSize(window, &frame_width, &frame_height);

    float fscale_x, fscale_y;
    glfwGetWindowContentScale(window, &fscale_x, &fscale_y);

    int scale_x = (int) fscale_x;
    int scale_y = (int) fscale_y;
    
    int scaled_tl_x = tl[0] * scale_x;
    int scaled_tl_y = tl[1] * scale_y;
    int scaled_br_x = br[0] * scale_x;
    int scaled_br_y = br[1] * scale_y;

    int data_bl_x = scaled_tl_x;
    int data_bl_y = frame_height - scaled_br_y;
    
    int data_width  = scaled_br_x - scaled_tl_x;
    int data_height = scaled_br_y - scaled_tl_y;
    
    glFlush();
    glFinish();

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    uint8_t data[4 * data_width * data_height];
    glReadPixels(data_bl_x, data_bl_y, data_width, data_height, GL_RGBA, GL_UNSIGNED_BYTE, data);

    uint32_t sparse_map[model.vertices_len];
    memset(sparse_map, 0, sizeof(sparse_map));

    for (int y = 0; y < data_height; y += scale_y) {
        for (int x = 0; x < data_width; x += scale_x) {
            unsigned char *pixel = data + ((y * data_width) + x) * 4;
                        
            uint32_t picked_id = pixel[0] + pixel[1] * 256 + pixel[2] * 256 * 256;
            if (picked_id >= model.vertices_len)
                continue; // ignore blank pixel

            sparse_map[picked_id] = 1;
        }
    }
    
    selection->len = 0;

    for (int i = 0; i < model.vertices_len; i++) {
        if (!sparse_map[i])
            continue;

        if (selection->len == selection->cap) {
            selection->cap *= 2;
            selection->indices = (uint32_t *)realloc(selection->indices, sizeof(uint32_t) * selection->cap);
        }

        selection->indices[selection->len++] = i;
    }
}