#ifndef _MYLIB_H_

#define _MYLIB_H_

typedef struct {
    float x;
    float y;
    float z;
    float w;
} vec4;

typedef struct {
    vec4 x;
    vec4 y;
    vec4 z;
    vec4 w;
} mat4;

// Insert function signatures after this line
//Lab2 Func
//TO-DO fix functions from Lab2 response
void vecPrint(vec4);
vec4 vecScale(float, vec4);
vec4 vecAdd(vec4, vec4);
vec4 vecSub(vec4, vec4);
float vecMag(vec4);
vec4 vecNorm(vec4);
float vecDot(vec4, vec4);
vec4 vecCross(vec4, vec4);
void matPrint(mat4);
mat4 matScale(float, mat4);
mat4 matAdd(mat4, mat4);
mat4 matSub(mat4, mat4);
mat4 matMult(mat4, mat4);
mat4 matInv(mat4);
float matDet(mat4);
mat4 matTrans(mat4);
vec4 matVecMult(vec4, mat4);

//Lab4 funcs
mat4 identity();
mat4 translate(float, float, float);
mat4 scale(float, float, float);
mat4 rotate(float, int);

// Do not put anything after this line

#endif
