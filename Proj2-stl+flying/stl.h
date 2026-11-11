#ifndef STL_H
#define STL_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float x, y, z;
} STL_Vec3;

typedef struct {
    STL_Vec3 normal;
    STL_Vec3 vertices[3];
} STL_Triangle;

typedef struct {
    STL_Triangle *triangles;
    size_t triangle_count;

    STL_Vec3 bounds_min;
    STL_Vec3 bounds_max;
} STL_Mesh;

/*
 * Load an STL file (binary or ASCII).
 *
 * Returns 1 on success, 0 on failure.
 * On failure, error may receive a descriptive message.
 * The caller must initialize mesh to {0} before its first use.
 */
int stl_load(const char *filename,
             STL_Mesh *mesh,
             char *error,
             size_t error_size);

/* Release all memory owned by a mesh. */
void stl_free(STL_Mesh *mesh);

/* Recalculate the mesh bounding box. */
void stl_calculate_bounds(STL_Mesh *mesh);

#ifdef __cplusplus
}
#endif

#endif /* STL_H */