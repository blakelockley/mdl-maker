#include "picker.h"

#include "camera.h"
#include "viewport.h"
#include "shader.h"

extern GLFWwindow *window;

extern camera_t camera;
extern viewport_t viewport;

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
        int r = (i & 0x000000FF) >>  0;
        int g = (i & 0x0000FF00) >>  8;
        int b = (i & 0x00FF0000) >> 16;

        vec3 id_color;
        vec3_set(id_color, r/255.0f, g/255.0f, b/255.0f);
        
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
    get_projection_matrix(&viewport, projection);
    
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