// #ifdef __APPLE__  // include Mac OS verions of headers
// #include <OpenGL/OpenGL.h>
// #include <GLUT/glut.h>
// #else             // non-Mac OS operating systems
// #include <GL/glew.h>
// #include <GL/freeglut.h>
// #include <GL/freeglut_ext.h>
// #endif            // __APPLE__

// #include <stdio.h>
// #include <stdlib.h>
// #include <math.h>
// #include "initShader.h"
// #include "myLib.h"

// int num_vertices;    // The number of vertices
// int first_index;     // The first index to draw
// int total_vert = 33;
// int triangleNum = 1;

// GLuint ctm_location; // The index of the uniform variable named ctm
// // ALWAYS initialize a transformation matrix to the identity matrix
// mat4 ctm = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};

// GLfloat degree = 0.0;     // The current degree
// int is_animating = 0;     // Whether the program is currently animating


// void init(void) {
//     num_vertices = total_vert;
//     vec4 *positions = (vec4*) malloc(sizeof(vec4) * num_vertices);
//     vec4 *colors = (vec4*) malloc(sizeof(vec4) * num_vertices);
    


    

//     GLuint program = initShader("vshader.glsl", "fshader.glsl");
//     glUseProgram(program);

//     GLuint vao;
// #ifdef __APPLE__                                     // include Mac OS verions of headers
//     glGenVertexArraysAPPLE(1, &vao);
//     glBindVertexArrayAPPLE(vao);
// #else                                                // Non-mac operating system
//     glGenVertexArrays(1, &vao);
//     glBindVertexArray(vao);
// #endif                                               // __APPLE__

//     GLuint buffer;
//     glGenBuffers(1, &buffer);
//     glBindBuffer(GL_ARRAY_BUFFER, buffer);
//     glBufferData(GL_ARRAY_BUFFER,
// 		 sizeof(vec4) * num_vertices * 2,    // The number of bytes to allocate
// 		 NULL,
// 		 GL_STATIC_DRAW);
//     glBufferSubData(GL_ARRAY_BUFFER,
// 		    0,                               // The starting offset of the allocated memory
// 		    sizeof(vec4) * num_vertices,     // The number of bytes to transfer
// 		    positions);                      // The array of data to transfer
//     glBufferSubData(GL_ARRAY_BUFFER,
// 		    sizeof(vec4) * num_vertices,     // The starting offset of the allocated memory
// 		    sizeof(vec4) * num_vertices,     // The number of bytes to transfer
// 		    colors);                         // The array of data to transfer

//     free(positions);                                 // positions is no longer needed
//     free(colors);                                    // colors is no longer needed
    
//     GLuint vPosition = glGetAttribLocation(program, "vPosition");
//     if(vPosition == -1)
// 	printf("[WARNING] Cannot allocate the attribute 'vPosition' or it is not used in a glsl.\n");
//     glEnableVertexAttribArray(vPosition);
//     glVertexAttribPointer(vPosition,                 // The index
// 			  4,                         // The number of components in each data
// 			  GL_FLOAT,                  // The type of each component
// 			  GL_FALSE,                  // Whether to normalize
// 			  0,                         // Number of byte offset between data
// 			  (GLvoid *) 0);             // The offset of the first data

//     GLuint vColor = glGetAttribLocation(program, "vColor");
//     if(vColor == -1)
// 	printf("[WARNING] Cannot allocate the attribute 'vColor' or it is not used in a glsl.\n");
//     glEnableVertexAttribArray(vColor);
//     glVertexAttribPointer(vColor,                    // The index
// 			  4,                         // The number of components in each data
// 			  GL_FLOAT,                  // The type of each component
// 			  GL_FALSE,                  // Whether to normalize
// 			  0,                         // Number of byte offset between data
// 			  (GLvoid *) (sizeof(vec4) * num_vertices)); // The offset of the first data

//     ctm_location = glGetUniformLocation(program, "ctm");
//     if(ctm_location == -1)                           // Check ctm_location
// 	printf("[WARNING] Cannot allocate the uniform variable 'ctm' or it is not used in a glsl.\n");
    
//     glEnable(GL_DEPTH_TEST);                         // Enable hidden surface removal
//     glClearColor(0.0, 0.0, 0.0, 1.0);                // The color to be used for glClear()
//     glDepthRange(1,0);                               // Make the near plane maps to 1.0

//     glPolygonMode(GL_FRONT, GL_FILL);                // Fill a front face with a solid color
//     glPolygonMode(GL_BACK, GL_LINE);                 // Draw a back face with just lines
// }

// void display(void) {
//     glClear(GL_COLOR_BUFFER_BIT |                    // Clear the color buffer
// 	    GL_DEPTH_BUFFER_BIT);                    // Clear the depth buffer

//     glUniformMatrix4fv(ctm_location,                 // The index of the uniform variable
// 		       1,                            // The number of components to be sent (1 matrix)
// 		       GL_FALSE,                     // Do not transpose
// 		       (GLfloat *) &ctm);             // The data to be sent
    
//     glDrawArrays(GL_TRIANGLES,                       // Type of primitive mode to draw
// 		 first_index,                        // The first index
// 		 num_vertices);                      // The number of vertices to draw

//     glutSwapBuffers();                               // Swap the drawing buffer with the display buffer
// }

// void keyboard(unsigned char key, int mousex, int mousey) {
//     if(key == 'q') {
// #ifdef __APPLE__
// 	exit(0);                // Exit the program
// #else
//     	glutLeaveMainLoop();    // Terminate the main loop (to exit the program)
// #endif
//     }
//     else if(key == 'f') {
// 	first_index = 0;
// 	num_vertices = 3;
//     }
//     else if(key == 'b') {
// 	first_index = 3;
// 	num_vertices = 3;
//     }
//     else if(key == 'a') {
// 	first_index = 0;
// 	num_vertices = total_vert;
//     }
//     else if(key == ' ') {
// 	is_animating ^= 0x1;
//     }

//     glutPostRedisplay();        // Redraw the whole scene
// }

// void idle(void) {
//     if(is_animating) {
// 	degree += 1.0;
// 	if(degree == 360)
// 	    degree = 0;
// 	GLfloat radian = degree * M_PI / 180.0;   // Convert degree to radian
// 	ctm.x.x = cos(radian);
// 	ctm.x.y = sin(radian);
// 	ctm.y.x = -ctm.x.y;
// 	ctm.y.y = ctm.x.x;
//     }

//     glutPostRedisplay();
// }

// void usage(void) {
//     printf("Usage:\n");
//     printf(" q - quit\n");
//     printf(" f - the front triangle only\n");
//     printf(" b - the back triangle only\n");
//     printf(" a - both triangles\n");
//     printf(" SPACE - start/stop animation\n");
// }

// int main(int argc, char **argv) {
//     glutInit(&argc, argv);
//     glutInitDisplayMode(GLUT_RGBA |         // Use Red, Green, Blue, and Alpha
// 			GLUT_DOUBLE |       // Double buffers
// 			GLUT_DEPTH);        // Allow depth
//     glutInitWindowSize(512, 512);           // Width and height of the window in number of pixels
//     glutInitWindowPosition(100,100);        // The position from the top-left corner
//     glutCreateWindow("Triangle");           // The name of the window
// #ifndef __APPLE__                           // If running on a non-mac OS
//     glewInit();
// #endif                                      // End if
//     init();                                 // Call the init function (user-defined above)
//     glutDisplayFunc(display);               // The function to call when redrawing is required
//     glutKeyboardFunc(keyboard);             // The function to call when a key is pressed
//     glutIdleFunc(idle);                     // The function to call when idle
//     usage();                                // Print the usage on the screen
//     glutMainLoop();                         // Enter an infinite loop

//     return 0;
// }

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

#include "initShader.h"
#include "myLib.h"
#include "stl.h"


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
/* Imported model */
static STL_Mesh mesh = {0};

/* OpenGL resources */
static GLuint program;
static GLuint vao;
static GLuint buffer;
static GLuint ctm_location;

/* Vertex counts */
static int num_vertices = 0;
static int first_index = 0;

/* Animation */
static GLfloat degree = 0.0f;
static int is_animating = 0;

/* Transformation matrix */
mat4 ctm = {
    {1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 1, 0},
    {0, 0, 0, 1}
};

void init(void)
{
    if (mesh.triangle_count == 0 ||
        mesh.triangle_count > (size_t)INT_MAX / 3) {
        fprintf(stderr, "Invalid or excessively large STL mesh.\n");
        exit(EXIT_FAILURE);
    }

    num_vertices = (int)(mesh.triangle_count * 3);

    vec4 *positions = malloc(
        (size_t)num_vertices * sizeof(*positions));

    vec4 *colors = malloc(
        (size_t)num_vertices * sizeof(*colors));

    if (!positions || !colors) {
        fprintf(stderr, "Failed to allocate vertex data.\n");
        free(positions);
        free(colors);
        exit(EXIT_FAILURE);
    }

    /*
     * Center the model and scale its largest dimension
     * to 1.5 units, keeping it inside the clip-space view.
     */
    float cx = (mesh.bounds_min.x + mesh.bounds_max.x) * 0.5f;
    float cy = (mesh.bounds_min.y + mesh.bounds_max.y) * 0.5f;
    float cz = (mesh.bounds_min.z + mesh.bounds_max.z) * 0.5f;

    float dx = mesh.bounds_max.x - mesh.bounds_min.x;
    float dy = mesh.bounds_max.y - mesh.bounds_min.y;
    float dz = mesh.bounds_max.z - mesh.bounds_min.z;

    float extent = fmaxf(dx, fmaxf(dy, dz));
    float scale = (extent > 0.0f) ? 1.5f / extent : 1.0f;

    int k = 0;

    for (size_t i = 0; i < mesh.triangle_count; ++i) {
        for (int j = 0; j < 3; ++j) {
            STL_Vec3 v = mesh.triangles[i].vertices[j];

            positions[k] = (vec4) {
                (v.x - cx) * scale,
                (v.y - cy) * scale,
                (v.z - cz) * scale,
                1.0f
            };

            /* Light blue-gray model color */
            colors[k] = (vec4) {
                0.75f, 0.80f, 0.90f, 1.0f
            };

            ++k;
        }
    }

    /* Initialize the existing shaders. */
    program = initShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);

#ifdef __APPLE__
    glGenVertexArraysAPPLE(1, &vao);
    glBindVertexArrayAPPLE(vao);
#else
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
#endif

    /* Upload positions and colors to one GPU buffer. */
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    size_t position_bytes =
        (size_t)num_vertices * sizeof(*positions);

    size_t color_bytes =
        (size_t)num_vertices * sizeof(*colors);

    glBufferData(GL_ARRAY_BUFFER,
                 (GLsizeiptr)(position_bytes + color_bytes),
                 NULL,
                 GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER,
                    0,
                    (GLsizeiptr)position_bytes,
                    positions);

    glBufferSubData(GL_ARRAY_BUFFER,
                    (GLintptr)position_bytes,
                    (GLsizeiptr)color_bytes,
                    colors);

    free(positions);
    free(colors);

    /* Position attribute */
    GLint vPosition = glGetAttribLocation(program, "vPosition");

    if (vPosition >= 0) {
        glEnableVertexAttribArray((GLuint)vPosition);
        glVertexAttribPointer(
            (GLuint)vPosition,
            4, GL_FLOAT, GL_FALSE, 0, (void *)0);
    } else {
        fprintf(stderr, "Shader attribute vPosition not found.\n");
        exit(EXIT_FAILURE);
    }

    /* Color attribute */
    GLint vColor = glGetAttribLocation(program, "vColor");

    if (vColor >= 0) {
        glEnableVertexAttribArray((GLuint)vColor);
        glVertexAttribPointer(
            (GLuint)vColor,
            4, GL_FLOAT, GL_FALSE, 0,
            (void *)(uintptr_t)position_bytes);
    } else {
        fprintf(stderr, "Shader attribute vColor not found.\n");
        exit(EXIT_FAILURE);
    }

    ctm_location = glGetUniformLocation(program, "ctm");

    if (ctm_location < 0) {
        fprintf(stderr, "Shader uniform ctm not found.\n");
        exit(EXIT_FAILURE);
    }

    glEnable(GL_DEPTH_TEST);
    glDepthRange(0.0, 1.0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    /* Solid front and wireframe back faces, as in your original. */
    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_LINE);

    printf("GPU vertex buffer initialized: %d vertices\n",
           num_vertices);
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(program);

    glUniformMatrix4fv(
        ctm_location,
        1,
        GL_FALSE,
        (GLfloat *)&ctm);

    glDrawArrays(
        GL_TRIANGLES,
        first_index,
        num_vertices);

    glutSwapBuffers();
}

void keyboard(unsigned char key, int mousex, int mousey)
{
    (void)mousex;
    (void)mousey;

    if (key == 'q' || key == 27) {
#ifdef __APPLE__
        stl_free(&mesh);
        exit(EXIT_SUCCESS);
#else
        stl_free(&mesh);
        glutLeaveMainLoop();
#endif
    }
    else if (key == 'f') {
        /* First triangle */
        first_index = 0;
        num_vertices = mesh.triangle_count >= 1 ? 3 : 0;
    }
    else if (key == 'b') {
        /* Second triangle */
        first_index = mesh.triangle_count >= 2 ? 3 : 0;
        num_vertices = mesh.triangle_count >= 2 ? 3 : 0;
    }
    else if (key == 'a') {
        /* Entire model */
        first_index = 0;
        num_vertices = (int)(mesh.triangle_count * 3);
    }
    else if (key == ' ') {
        is_animating = !is_animating;
    }

    glutPostRedisplay();
}

void idle(void)
{
    if (is_animating) {
        degree += 1.0f;

        if (degree >= 360.0f)
            degree -= 360.0f;

        GLfloat radian = degree * (GLfloat)M_PI / 180.0f;

        ctm.x.x = cosf(radian);
        ctm.x.y = sinf(radian);
        ctm.y.x = -sin(radian);
        ctm.y.y = cosf(radian);
    }

    glutPostRedisplay();
}

void usage(void)
{
    printf("\nControls:\n");
    printf("  q / ESC - Quit\n");
    printf("  f       - Show first triangle\n");
    printf("  b       - Show second triangle\n");
    printf("  a       - Show entire STL model\n");
    printf("  SPACE   - Start/stop rotation\n\n");
}


int main(int argc, char **argv)
{
    fprintf(stderr, "[1] Program started\n");

    if (argc < 2) {
        fprintf(stderr, "[ERROR] Missing STL filename.\n");
        fprintf(stderr, "Usage: %s model.stl\n", argv[0]);
        return EXIT_FAILURE;
    }

    fprintf(stderr, "[2] Loading STL: %s\n", argv[1]);

    char error[256];
    if (!stl_load(argv[1], &mesh, error, sizeof(error))) {
        fprintf(stderr, "[ERROR] STL loading failed: %s\n", error);
        return EXIT_FAILURE;
    }

    fprintf(stderr, "[3] STL loaded: %zu triangles\n",
            mesh.triangle_count);

    fprintf(stderr, "[4] Initializing GLUT\n");

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(512, 512);
    glutInitWindowPosition(100, 100);

    int window = glutCreateWindow("STL Viewer");

    if (window <= 0) {
        fprintf(stderr, "[ERROR] Window creation failed\n");
        stl_free(&mesh);
        return EXIT_FAILURE;
    }

    fprintf(stderr, "[5] Window created\n");

#ifndef __APPLE__
    GLenum status = glewInit();

    if (status != GLEW_OK) {
        fprintf(stderr, "[ERROR] GLEW: %s\n",
                glewGetErrorString(status));
        stl_free(&mesh);
        return EXIT_FAILURE;
    }
#endif

    fprintf(stderr, "[6] Initializing shaders and buffers\n");

    init();

    fprintf(stderr, "[7] Registering callbacks\n");

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);

    usage();

    fprintf(stderr, "[8] Entering GLUT main loop\n");

    glutMainLoop();

    stl_free(&mesh);
    return EXIT_SUCCESS;
}
