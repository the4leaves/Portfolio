#include "stl.h"

#include <stdio.h>

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("Usage: %s model.stl\n", argv[0]);
        return 1;
    }

    STL_Mesh mesh = {0};
    char error[256];

    if (!stl_load(argv[1], &mesh, error, sizeof(error))) {
        fprintf(stderr, "STL loading failed: %s\n", error);
        return 1;
    }

    printf("STL loaded successfully!\n");
    printf("Triangles: %zu\n", mesh.triangle_count);

    printf("Minimum bounds: %.3f, %.3f, %.3f\n",
           mesh.bounds_min.x,
           mesh.bounds_min.y,
           mesh.bounds_min.z);

    printf("Maximum bounds: %.3f, %.3f, %.3f\n",
           mesh.bounds_max.x,
           mesh.bounds_max.y,
           mesh.bounds_max.z);

    printf("First triangle vertices:\n");

    if (mesh.triangle_count > 0) {
        for (int i = 0; i < 3; ++i) {
            STL_Vec3 v = mesh.triangles[0].vertices[i];

            printf("  (%.3f, %.3f, %.3f)\n",
                   v.x, v.y, v.z);
        }
    }

    stl_free(&mesh);
    return 0;
}