
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
static GLuint modelView_location;
static GLuint projection_location;
static GLuint ctm_location;

/* Vertex counts */
static int num_vertices = 0;
static int first_index = 0;

/* Animation */
static int is_animating = 0;
static int rotateX = 1;

/* Transformation matrix */
int keys[256] = {0};

mat4 projection;
mat4 camPos;
mat4 modelView;
mat4 ctm;

mat4 perspective(GLfloat fovy, GLfloat aspect, GLfloat near, GLfloat far) {
    mat4 returner = identity();
    GLfloat rad = fovy * M_PI / 180.00;
    GLfloat t = tanf(rad * 0.5);
    returner.x.x = 1.0/(aspect * t);
    returner.y.y = 1.0/t;
    returner.z.z = -1.0*((far+near)/(far-near));
    returner.z.w = -1.0;
    returner.w.z = -1.0*((2*far*near)/(far-near));
    return returner;
}


void init(void)
{
    //Checks for valid stl
    if (mesh.triangle_count == 0 ||
        mesh.triangle_count > (size_t)INT_MAX / 3) {
        fprintf(stderr, "Invalid or excessively large STL mesh.\n");
        exit(EXIT_FAILURE);
    }

    projection = perspective(90.0, 1, 1, 500);
    camPos = identity();

    num_vertices = (int)(mesh.triangle_count * 3);

    vec4 *positions = malloc(
        (size_t)num_vertices * sizeof(*positions));

    vec4 *normals = malloc(
        (size_t)num_vertices * sizeof(*normals));

    if (!positions || !normals) {
        fprintf(stderr, "Failed to allocate vertex data.\n");
        free(positions);
        free(normals);
        exit(EXIT_FAILURE);
    }

    int k = 0;

    for (size_t i = 0; i < mesh.triangle_count; ++i) {
        for (int j = 0; j < 3; ++j) {
            STL_Vec3 v = mesh.triangles[i].vertices[j];

            positions[k] = (vec4) {
                v.x,
                v.y,
                v.z,
                1.0f
            };
            k++;
        }
        vec4 u = vecSub(positions[k-2],positions[k-3]);
        vec4 v = vecSub(positions[k-1],positions[k-2]);
        vec4 n = vecCross(u,v);
        n = vecNorm(n);
        normals[k-1] = n;
        normals[k-2] = n;
        normals[k-3] = n;
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

    size_t normal_bytes =
        (size_t)num_vertices * sizeof(*positions);

    glBufferData(GL_ARRAY_BUFFER,
                 (GLsizeiptr)(position_bytes + normal_bytes),
                 NULL,
                 GL_STATIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER,
                    0,
                    (GLsizeiptr)position_bytes,
                    positions);

    glBufferSubData(GL_ARRAY_BUFFER,
                    (GLintptr)position_bytes,
                    (GLsizeiptr)normal_bytes,
                    normals);
    free(positions);
    free(normals);
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

    GLint vNormal = glGetAttribLocation(program, "vNormal");

    if (vNormal >= 0) {
        glEnableVertexAttribArray((GLuint)vNormal);
        glVertexAttribPointer(
            (GLuint)vNormal,
            4, GL_FLOAT, GL_FALSE, 0, (void *)(uintptr_t)position_bytes);
    } else {
        fprintf(stderr, "Shader attribute vNormal not found.\n");
        exit(EXIT_FAILURE);
    }

    modelView_location = glGetUniformLocation(program, "modelView");

    if (modelView_location < 0) {
        fprintf(stderr, "Shader uniform modelView not found.\n");
        exit(EXIT_FAILURE);
    }

    ctm_location = glGetUniformLocation(program, "ctm");

    if (modelView_location < 0) {
        fprintf(stderr, "Shader uniform ctm not found.\n");
        exit(EXIT_FAILURE);
    }

    projection_location = glGetUniformLocation(program, "projection");

    if (projection_location < 0) {
        fprintf(stderr, "Shader uniform projection not found.\n");
        exit(EXIT_FAILURE);
    }

    glEnable(GL_DEPTH_TEST);
    glDepthRange(0.0, 1.0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    /* Solid front and wireframe back faces, as in your original. */
    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_FILL);

    printf("GPU vertex buffer initialized: %d vertices\n",
           num_vertices);
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(program);

    glUniformMatrix4fv(
        modelView_location,
        1,
        GL_FALSE,
        (GLfloat *)&modelView);

    glUniformMatrix4fv(
        projection_location,
        1,
        GL_FALSE,
        (GLfloat *)&projection);

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

// void keyboard_down(unsigned char key, int x, int y)
// {
//     keys[key] = 1;
// }

void keyboard_up(unsigned char key, int x, int y)
{
    keys[key] = 0;
}

void keyboard(unsigned char key, int mousex, int mousey)
{
    (void)mousex;
    (void)mousey;

    if (key == 27) {
#ifdef __APPLE__
        stl_free(&mesh);
        exit(EXIT_SUCCESS);
#else
        stl_free(&mesh);
        glutLeaveMainLoop();
#endif
    }
    else if (key == 'r') {
        rotateX = 1;
    }
    else if (key == 't') {
        is_animating = !is_animating;
    } else if (key == 'w') {
        keys[key] = 1;
    } else if (key == 's') {
        keys[key] = 1;
    } else if (key == 'a') {
        keys[key] = 1;
    } else if (key == 'd') {
        keys[key] = 1;
    } else if (key == ' ') {
        keys[key] = 1;
    } else if (key == 'c') {
        keys[key] = 1;
    }
     else if (key == 'q') {
        keys[key] = 1;
    }
     else if (key == 'e') {
        keys[key] = 1;
    } else if (key == 'p') {
        matPrint(camPos);
    }

    glutPostRedisplay();
}

void idle(void)
{
    if(rotateX == 1) {
        camPos = matMult(camPos, rotate(-90, 1));
        camPos = matMult(camPos, translate(-220,-300,-5));
        ctm = rotate(-90, 1);
        rotateX = 0;
    }
    if (is_animating) {
        modelView = matMult(modelView, rotate(5, 2));
    }

    if (keys['w']) camPos = matMult(translate(0, 0, 1), camPos);
    if (keys['s']) camPos = matMult(translate(0, 0, -1), camPos);

    if (keys['a']) camPos = matMult(translate(1, 0, 0), camPos);
    if (keys['d']) camPos = matMult(translate(-1, 0, 0), camPos);

    if (keys['q']) camPos = matMult(rotate(-1, 2), camPos);
    if (keys['e']) camPos = matMult(rotate(1, 2), camPos);

    if (keys['c']) camPos = matMult(translate(0, 1, 0), camPos);
    if (keys[' ']) camPos = matMult(translate(0, -1, 0), camPos);

    modelView = camPos;

    glutPostRedisplay();
}

void usage(void)
{
    printf("\nControls:\n");
    printf("  ESC - Quit\n");
    printf("  wasd       - movement\n");
    printf("  qe       - rotation\n");
    printf("  SPACE/c   - fly up/down\n\n");
}

//Ideas:
//ModelView will be derived from modelView and a fake camera position
//Projection matrix will just project world positions onto the viewing pane
//



//[x.x, y.x, z.x, w.x]
//[x.y, y.y, z.y, w.y]
//[x.z, y.z, z.z, w.z]
//[x.w, y.w, z.w, w.w]




int main(int argc, char **argv)
{

    if (argc < 2) {
        fprintf(stderr, "[ERROR] Missing STL filename.\n");
        fprintf(stderr, "Usage: %s model.stl\n", argv[0]);
        return EXIT_FAILURE;
    }

    char error[256];
    if (!stl_load(argv[1], &mesh, error, sizeof(error))) {
        fprintf(stderr, "[ERROR] STL loading failed: %s\n", error);
        return EXIT_FAILURE;
    }

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

#ifndef __APPLE__
    GLenum status = glewInit();

    if (status != GLEW_OK) {
        fprintf(stderr, "[ERROR] GLEW: %s\n", glewGetErrorString(status));
        stl_free(&mesh);
        return EXIT_FAILURE;
    }
#endif

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);
    glutKeyboardUpFunc(keyboard_up);
    //glutKeyboardDownFunc(keyboard_down);

    usage();

    glutMainLoop();

    stl_free(&mesh);
    return EXIT_SUCCESS;
}
