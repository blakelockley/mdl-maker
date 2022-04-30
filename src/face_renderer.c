#include "face_renderer.h"
#include "shader.h"
#include "camera.h"
#include "viewport.h"
#include "light.h"

extern camera_t camera;
extern viewport_t viewport;
extern light_t light;
face_renderer_t face_renderer;

void init_face_renderer() {
    face_renderer.shader = load_shader("shaders/face.vert", "shaders/face.frag");

    glGenVertexArrays(1, &face_renderer.vao);
    glBindVertexArray(face_renderer.vao);

    // Vertices
    glGenBuffers(1, &face_renderer.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, face_renderer.vbo);
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);

    // Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3) * 2, (void*)0);

    // Normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vec3) * 2, (void*)sizeof(vec3));  // Positions
}

void render_model_faces(model_t *model) {
    uint32_t total_vertices = 0;
    for (int i = 0; i < model->faces_len; i++)
        total_vertices += (model->faces[i].len - 2) * 3; // n -> (n - 2) * 3

    // Normals
    total_vertices *= 2;

    vec3 vertices[total_vertices];
    uint32_t vi = 0; // vertices index

    for (int i = 0; i < model->faces_len; i++) {
        for (int j = 2; j < model->faces[i].len; j++) {
            vec3 normal;
            vec3_copy(normal, model->faces[i].normal);
            
            uint32_t *indices = model->faces[i].indices;

            vec3_copy(vertices[vi++], model->vertices[indices[0]]);
            vec3_copy(vertices[vi++], normal);
            
            vec3_copy(vertices[vi++], model->vertices[indices[j - 1]]);
            vec3_copy(vertices[vi++], normal);
            
            vec3_copy(vertices[vi++], model->vertices[indices[j - 0]]);
            vec3_copy(vertices[vi++], normal);
        }
    }
    
    glBindVertexArray(face_renderer.vao);
    glBindBuffer(GL_ARRAY_BUFFER, face_renderer.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glUseProgram(face_renderer.shader);
    
    mat4x4 _model, view, projection;
    mat4x4_identity(_model);
    get_view_matrix(&camera, view);
    get_projection_matrix(&viewport, projection);
    
    GLint model_loc = glGetUniformLocation(face_renderer.shader, "model");
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, (float*)_model);

    GLint view_loc = glGetUniformLocation(face_renderer.shader, "view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, (float*)view);

    GLint projection_loc = glGetUniformLocation(face_renderer.shader, "projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float*)projection);

    GLint light_pos_loc = glGetUniformLocation(face_renderer.shader, "light_pos");
    glUniform3fv(light_pos_loc, 1, (float*)light.pos);

    GLint light_color_loc = glGetUniformLocation(face_renderer.shader, "light_color");
    glUniform3fv(light_color_loc, 1, (float*)light.color);
     
    GLint color_loc = glGetUniformLocation(face_renderer.shader, "color");
    glUniform3f(color_loc, 0.1f, 0.2f, 1.0f);
    
    glDrawArrays(GL_TRIANGLES, 0, total_vertices);
}
