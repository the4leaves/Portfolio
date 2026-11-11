#include "stl.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <errno.h>

/* ---------- Error handling ---------- */

static void set_error(char *error, size_t size, const char *message)
{
    if (error && size > 0) {
        snprintf(error, size, "%s", message);
    }
}

static STL_Vec3 make_vec3(float x, float y, float z)
{
    STL_Vec3 v = { x, y, z };
    return v;
}

/* ---------- Little-endian binary readers ---------- */

static uint32_t read_u32_le(const unsigned char *p)
{
    return ((uint32_t)p[0]) |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static uint16_t read_u16_le(const unsigned char *p)
{
    return (uint16_t)p[0] |
           ((uint16_t)p[1] << 8);
}

static float read_f32_le(const unsigned char *p)
{
    uint32_t bits = read_u32_le(p);
    float value;
    memcpy(&value, &bits, sizeof(value));
    return value;
}

/* ---------- Mesh bounds ---------- */

void stl_calculate_bounds(STL_Mesh *mesh)
{
    if (!mesh || !mesh->triangles || mesh->triangle_count == 0) {
        if (mesh) {
            mesh->bounds_min = make_vec3(0, 0, 0);
            mesh->bounds_max = make_vec3(0, 0, 0);
        }
        return;
    }

    STL_Vec3 first = mesh->triangles[0].vertices[0];

    mesh->bounds_min = first;
    mesh->bounds_max = first;

    for (size_t i = 0; i < mesh->triangle_count; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            STL_Vec3 v = mesh->triangles[i].vertices[j];

            if (v.x < mesh->bounds_min.x) mesh->bounds_min.x = v.x;
            if (v.y < mesh->bounds_min.y) mesh->bounds_min.y = v.y;
            if (v.z < mesh->bounds_min.z) mesh->bounds_min.z = v.z;

            if (v.x > mesh->bounds_max.x) mesh->bounds_max.x = v.x;
            if (v.y > mesh->bounds_max.y) mesh->bounds_max.y = v.y;
            if (v.z > mesh->bounds_max.z) mesh->bounds_max.z = v.z;
        }
    }
}

/* ---------- Binary STL ---------- */

static int load_binary(const unsigned char *data,
                       size_t size,
                       STL_Mesh *mesh)
{
    if (size < 84)
        return 0;

    uint32_t count = read_u32_le(data + 80);

    /* Each binary STL triangle occupies exactly 50 bytes. */
    if ((uint64_t)count * 50u != (uint64_t)(size - 84))
        return 0;

    if (count != 0 && sizeof(STL_Triangle) > SIZE_MAX / (size_t)count)
        return 0;

    STL_Triangle *triangles = NULL;

    if (count != 0) {
        triangles = malloc((size_t)count * sizeof(*triangles));
        if (!triangles)
            return 0;
    }

    const unsigned char *p = data + 84;

    for (uint32_t i = 0; i < count; ++i, p += 50) {
        STL_Triangle *t = &triangles[i];

        t->normal = make_vec3(
            read_f32_le(p),
            read_f32_le(p + 4),
            read_f32_le(p + 8)
        );

        for (int j = 0; j < 3; ++j) {
            const unsigned char *v = p + 12 + j * 12;

            t->vertices[j] = make_vec3(
                read_f32_le(v),
                read_f32_le(v + 4),
                read_f32_le(v + 8)
            );
        }

        /* The final two bytes are the attribute byte count. */
        (void)read_u16_le(p + 48);
    }

    mesh->triangles = triangles;
    mesh->triangle_count = (size_t)count;
    stl_calculate_bounds(mesh);

    return 1;
}

/* ---------- ASCII STL ---------- */

static int append_triangle(STL_Mesh *mesh,
                           size_t *capacity,
                           const STL_Triangle *triangle)
{
    if (mesh->triangle_count == *capacity) {
        size_t next = (*capacity == 0) ? 64 : *capacity * 2;

        if (next < *capacity ||
            next > SIZE_MAX / sizeof(STL_Triangle)) {
            return 0;
        }

        STL_Triangle *p = realloc(
            mesh->triangles, next * sizeof(STL_Triangle));

        if (!p)
            return 0;

        mesh->triangles = p;
        *capacity = next;
    }

    mesh->triangles[mesh->triangle_count++] = *triangle;
    return 1;
}

static int parse_vec3_line(const char *line,
                           const char *keyword,
                           STL_Vec3 *v)
{
    char word[32];
    float x, y, z;
    int n = sscanf(line, " %31s %f %f %f", word, &x, &y, &z);

    if (n != 4 || strcmp(word, keyword) != 0)
        return 0;

    v->x = x;
    v->y = y;
    v->z = z;
    return 1;
}

static int load_ascii(const unsigned char *data,
                      size_t size,
                      STL_Mesh *mesh)
{
    if (size == SIZE_MAX)
        return 0;

    char *text = malloc(size + 1);
    if (!text)
        return 0;

    memcpy(text, data, size);
    text[size] = '\0';

    /* Embedded NUL bytes are not valid ASCII STL text. */
    if (memchr(text, '\0', size) != NULL) {
        free(text);
        return 0;
    }

    size_t capacity = 0;
    STL_Triangle current;
    int have_normal = 0;
    int vertex_count = 0;
    int success = 1;

    char *line = text;

    while (*line) {
        char *end = strchr(line, '\n');
        if (end)
            *end = '\0';

        /* Trim leading whitespace. */
        while (isspace((unsigned char)*line))
            ++line;

        if (strncmp(line, "facet", 5) == 0 &&
            isspace((unsigned char)line[5])) {

            if (!parse_vec3_line(line + 5, "normal",
                                 &current.normal)) {
                success = 0;
                break;
            }

            have_normal = 1;
            vertex_count = 0;

        } else if (strncmp(line, "vertex", 6) == 0 &&
                   isspace((unsigned char)line[6])) {

            if (!have_normal || vertex_count >= 3) {
                success = 0;
                break;
            }

            if (!parse_vec3_line(line, "vertex",
                                 &current.vertices[vertex_count])) {
                success = 0;
                break;
            }

            ++vertex_count;

        } else if (strncmp(line, "endfacet", 8) == 0) {

            if (!have_normal || vertex_count != 3 ||
                !append_triangle(mesh, &capacity, &current)) {
                success = 0;
                break;
            }

            have_normal = 0;
            vertex_count = 0;
        }

        if (!end)
            break;

        line = end + 1;
    }

    if (have_normal || vertex_count != 0)
        success = 0;

    free(text);

    if (!success || mesh->triangle_count == 0)
        return 0;

    stl_calculate_bounds(mesh);
    return 1;
}

/* ---------- Public API ---------- */

void stl_free(STL_Mesh *mesh)
{
    if (!mesh)
        return;

    free(mesh->triangles);
    memset(mesh, 0, sizeof(*mesh));
}

int stl_load(const char *filename,
             STL_Mesh *mesh,
             char *error,
             size_t error_size)
{
    if (!filename || !mesh) {
        set_error(error, error_size, "Invalid argument");
        return 0;
    }

    /* Start with a clean output object. */
    memset(mesh, 0, sizeof(*mesh));

    FILE *file = fopen(filename, "rb");
    if (!file) {
        set_error(error, error_size, "Could not open STL file");
        return 0;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        set_error(error, error_size, "Could not seek in STL file");
        return 0;
    }

    long file_length = ftell(file);

    if (file_length < 0 ||
        fseek(file, 0, SEEK_SET) != 0) {
        fclose(file);
        set_error(error, error_size, "Could not determine file size");
        return 0;
    }

    size_t size = (size_t)file_length;
    unsigned char *data = malloc(size ? size : 1);

    if (!data) {
        fclose(file);
        set_error(error, error_size, "Out of memory");
        return 0;
    }

    size_t bytes_read = fread(data, 1, size, file);
    int read_error = ferror(file);
    fclose(file);

    if (bytes_read != size || read_error) {
        free(data);
        set_error(error, error_size, "Could not read STL file");
        return 0;
    }

    int ok = load_binary(data, size, mesh);

    if (!ok) {
        stl_free(mesh);
        ok = load_ascii(data, size, mesh);
    }

    free(data);

    if (!ok) {
        stl_free(mesh);
        set_error(error, error_size,
                  "Invalid STL, unsupported format, or out of memory");
        return 0;
    }

    set_error(error, error_size, "");
    return 1;
}