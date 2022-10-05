#include "primitives.h"

void _build_plane(model_t *model, vec3 pos, float side) {
    vec3 vertex;
    uint32_t indices[4];
    
    vec3_add(vertex, pos, (vec3){side, 0.0f, side});
    indices[0] = add_vertex(model, vertex);
    
    vec3_add(vertex, pos, (vec3){side, 0.0f, -side});
    indices[1] = add_vertex(model, vertex);
    
    vec3_add(vertex, pos, (vec3){-side, 0.0f, -side});
    indices[2] = add_vertex(model, vertex);
    
    vec3_add(vertex, pos, (vec3){-side, 0.0f, side});
    indices[3] = add_vertex(model, vertex);

    add_face(model, indices, 4);
}

void build_plane(model_t *model, vec3 pos, float side, int n_subdivisions){
    int n_side_vertices = 2 + n_subdivisions;
    float step = side / (n_side_vertices - 1);

    int n_indices = n_side_vertices * n_side_vertices;
    uint32_t indices[n_indices];

    vec3 top_left;
    vec3_add(top_left, pos, (vec3){ -side / 2.0f, 0.0f, -side / 2.0f });

    vec3 vertex;
    for (int i = 0; i < n_side_vertices; i++) {
        for (int j = 0; j < n_side_vertices; j++) {
            vec3_add(vertex, top_left, (vec3){step * i, 0.0f, step * j});
            indices[(i * n_side_vertices) + j] = add_vertex(model, vertex);
        }
    }

    int n_row_faces = (n_side_vertices - 1);
    uint32_t face_indices[3];
    
    for (int i = 0; i < n_row_faces; i++) {
        for (int j = 0; j < n_row_faces; j++) {
            face_indices[0] = indices[(i * n_side_vertices) + j];
            face_indices[1] = indices[(i * n_side_vertices) + j + 1];
            face_indices[2] = indices[((i + 1) * n_side_vertices) + j];
            add_face(model, face_indices, 3);
            
            face_indices[0] = indices[((i + 1) * n_side_vertices) + j + 1];
            face_indices[1] = indices[((i + 1) * n_side_vertices) + j];
            face_indices[2] = indices[(i * n_side_vertices) + j + 1];
            add_face(model, face_indices, 3);
        }
    }
}

void build_circle(model_t *model, vec3 pos, float radius, int n_vertices, bool fill){

}

void build_cube(model_t *model, vec3 pos, float side){

}

void build_sphere(model_t *model, vec3 pos, float radius, int n_slices, int n_sectors){

}

// https://observablehq.com/@mourner/fast-icosphere-mesh
// http://blog.andreaskahler.com/2009/06/creating-icosphere-mesh-in-code.html
void build_icosphere(model_t *model, vec3 pos, float radius, int order) {
    uint32_t n_vertices = 10 * ((uint32_t) powf(4.0f, order)) + 2;
    uint32_t n_faces = n_vertices - 2;

    float r = radius;
    float t = ((1.0f + sqrtf(5.0f)) / 2.0f) * r;

    uint32_t indicies[n_vertices];
    indicies[0]  = add_vertex(model, (vec3){-r,  t,  0});
    indicies[1]  = add_vertex(model, (vec3){ r,  t,  0});
    indicies[2]  = add_vertex(model, (vec3){-r, -t,  0});
    indicies[3]  = add_vertex(model, (vec3){ r, -t,  0});

    indicies[4]  = add_vertex(model, (vec3){ 0, -r,  t});
    indicies[5]  = add_vertex(model, (vec3){ 0,  r,  t});
    indicies[6]  = add_vertex(model, (vec3){ 0, -r, -t});
    indicies[7]  = add_vertex(model, (vec3){ 0,  r, -t});

    indicies[8]  = add_vertex(model, (vec3){ t,  0, -r});
    indicies[9]  = add_vertex(model, (vec3){ t,  0,  r});
    indicies[10] = add_vertex(model, (vec3){-t,  0, -r});
    indicies[11] = add_vertex(model, (vec3){-t,  0,  r});

    face_t *faces[n_faces];
    uint32_t face_indices[3];

    face_indices[0] = 0; face_indices[1] = 1; face_indices[2] = 5;
    faces[0] = add_face(model, face_indices, 3);

    face_indices[0] = 0; face_indices[1] = 1; face_indices[2] = 7;
    faces[1] = add_face(model, face_indices, 3);

    face_indices[0] = 7; face_indices[1] = 0; face_indices[2] = 10;
    faces[2] = add_face(model, face_indices, 3);

    face_indices[0] = 4; face_indices[1] = 3; face_indices[2] = 2;
    faces[3] = add_face(model, face_indices, 3);

    face_indices[0] = 11; face_indices[1] = 10; face_indices[2] = 0;
    faces[4] = add_face(model, face_indices, 3);

    face_indices[0] = 0; face_indices[1] = 5; face_indices[2] = 11;
    faces[5] = add_face(model, face_indices, 3);

    face_indices[0] = 7; face_indices[1] = 1; face_indices[2] = 8;
    faces[6] = add_face(model, face_indices, 3);

    face_indices[0] = 5; face_indices[1] = 1; face_indices[2] = 9;
    faces[7] = add_face(model, face_indices, 3);

    face_indices[0] = 9; face_indices[1] = 1; face_indices[2] = 8;
    faces[8] = add_face(model, face_indices, 3);

    face_indices[0] = 5; face_indices[1] = 11; face_indices[2] = 4;
    faces[9] = add_face(model, face_indices, 3);

    face_indices[0] = 5; face_indices[1] = 9; face_indices[2] = 4;
    faces[10] = add_face(model, face_indices, 3);

    face_indices[0] = 4; face_indices[1] = 9; face_indices[2] = 3;
    faces[11] = add_face(model, face_indices, 3);

    face_indices[0] = 3; face_indices[1] = 8; face_indices[2] = 9;
    faces[12] = add_face(model, face_indices, 3);

    face_indices[0] = 8; face_indices[1] = 3; face_indices[2] = 6;
    faces[13] = add_face(model, face_indices, 3);

    face_indices[0] = 7; face_indices[1] = 6; face_indices[2] = 8;
    faces[14] = add_face(model, face_indices, 3);

    face_indices[0] = 7; face_indices[1] = 6; face_indices[2] = 10;
    faces[15] = add_face(model, face_indices, 3);

    face_indices[0] = 6; face_indices[1] = 10; face_indices[2] = 2;
    faces[16] = add_face(model, face_indices, 3);

    face_indices[0] = 10; face_indices[1] = 11; face_indices[2] = 2;
    faces[17] = add_face(model, face_indices, 3);

    face_indices[0] = 3; face_indices[1] = 6; face_indices[2] = 2;
    faces[18] = add_face(model, face_indices, 3);

    face_indices[0] = 11; face_indices[1] = 4; face_indices[2] = 2;
    faces[19] = add_face(model, face_indices, 3);

    for (int level = 0; level < order; level++) {
        for (int i = 0; i < 20; i++) {
            face_t *face = faces[i];

            uint32_t ia = face->indices[0];
            uint32_t ib = face->indices[1];
            uint32_t ic = face->indices[2];

            vec3 a, b, c;
            vec3_copy(a, model->vertices[ia]);
            vec3_copy(b, model->vertices[ib]);
            vec3_copy(c, model->vertices[ic]);

            vec3 ab, ac, bc;
            vec3_add(ab, a, b);
            vec3_scale(ab, ab, 0.5);
            vec3_normalize(ab, ab);
            vec3_scale(ab, ab, radius * 2);
            uint32_t iab = add_vertex(model, ab);
            
            vec3_add(ac, a, c);
            vec3_scale(ac, ac, 0.5);
            vec3_normalize(ac, ac);
            vec3_scale(ac, ac, radius * 2);
            uint32_t iac = add_vertex(model, ac);
            
            vec3_add(bc, b, c);
            vec3_scale(bc, bc, 0.5);
            vec3_normalize(bc, bc);
            vec3_scale(bc, bc, radius * 2);
            uint32_t ibc = add_vertex(model, bc);

            add_face_tri(model, ia, iab, iac);
            add_face_tri(model, ib, iab, ibc);
            add_face_tri(model, ic, iac, ibc);

            face->indices[0] = iab;
            face->indices[1] = iac;
            face->indices[2] = ibc;
        }
    }

}

