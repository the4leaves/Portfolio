#include "myLib.h"
#include <stdio.h>
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
void vecPrint(vec4 v) {
    printf("[ %7.3f %7.3f %7.3f %7.3f ]", v.x, v.y, v.z, v.w);
}

vec4 vecScale(float s, vec4 v) {
    v.x = v.x*s;
    v.y = v.y*s;
    v.z = v.z*s;
    v.w = v.w*s;
    return v;
}

vec4 vecAdd(vec4 v1, vec4 v2) {
    vec4 r;
    r.x = v1.x+v2.x;
    r.y = v1.y+v2.y;
    r.z = v1.z+v2.z;
    r.w = v1.w+v2.w;
    return r;
}

vec4 vecSub(vec4 v1, vec4 v2) {
    vec4 r;
    r.x = v1.x-v2.x;
    r.y = v1.y-v2.y;
    r.z = v1.z-v2.z;
    r.w = v1.w-v2.w;
    return r;
}

float vecMag(vec4 v) {
    float r = sqrt((v.x*v.x)+(v.y*v.y)+(v.z*v.z)+(v.w*v.w));
    return r;
}

vec4 vecNorm(vec4 v) {
    v = vecScale(1.0/vecMag(v), v);
    return v;
}

float vecDot(vec4 v1, vec4 v2) {
    float r = (v1.x*v2.x)+(v1.y*v2.y)+(v1.z*v2.z)+(v1.w*v2.w);
    return r;
}

vec4 vecCross(vec4 v1, vec4 v2) {
    vec4 r;
    r.w = (float) 0;
    r.x = (v1.y*v2.z)-(v1.z*v2.y);
    r.y = (v1.z*v2.x)-(v1.x*v2.z);
    r.z = (v1.x*v2.y)-(v1.y*v2.x);
    return r;
}

void matPrint(mat4 m) {
    vecPrint(m.x);
    vecPrint(m.y);
    vecPrint(m.z);
    vecPrint(m.w);
}

mat4 matScale(float s, mat4 m) {
    m.x = vecScale(s, m.x);
    m.y = vecScale(s, m.y);
    m.z = vecScale(s, m.z);
    m.w = vecScale(s, m.w);
    return m;
}

mat4 matAdd(mat4 m1, mat4 m2) {
    mat4 r;
    r.x = vecAdd(m1.x,m2.x);
    r.y = vecAdd(m1.y,m2.y);
    r.z = vecAdd(m1.z,m2.z);
    r.w = vecAdd(m1.w,m2.w);
    return r;
}

mat4 matSub(mat4 m1, mat4 m2) {
    mat4 r;
    r.x = vecSub(m1.x,m2.x);
    r.y = vecSub(m1.y,m2.y);
    r.z = vecSub(m1.z,m2.z);
    r.w = vecSub(m1.w,m2.w);
    return r;
}

mat4 matMult(mat4 m1, mat4 m2) {
    mat4 r;

    r.x.x = vecDot((vec4){m1.x.x, m1.y.x, m1.z.x, m1.w.x}, m2.x);
    r.x.y = vecDot((vec4){m1.x.y, m1.y.y, m1.z.y, m1.w.y}, m2.x);
    r.x.z = vecDot((vec4){m1.x.z, m1.y.z, m1.z.z, m1.w.z}, m2.x);
    r.x.w = vecDot((vec4){m1.x.w, m1.y.w, m1.z.w, m1.w.w}, m2.x);

    r.y.x = vecDot((vec4){m1.x.x, m1.y.x, m1.z.x, m1.w.x}, m2.y);
    r.y.y = vecDot((vec4){m1.x.y, m1.y.y, m1.z.y, m1.w.y}, m2.y);
    r.y.z = vecDot((vec4){m1.x.z, m1.y.z, m1.z.z, m1.w.z}, m2.y);
    r.y.w = vecDot((vec4){m1.x.w, m1.y.w, m1.z.w, m1.w.w}, m2.y);

    r.z.x = vecDot((vec4){m1.x.x, m1.y.x, m1.z.x, m1.w.x}, m2.z);
    r.z.y = vecDot((vec4){m1.x.y, m1.y.y, m1.z.y, m1.w.y}, m2.z);
    r.z.z = vecDot((vec4){m1.x.z, m1.y.z, m1.z.z, m1.w.z}, m2.z);
    r.z.w = vecDot((vec4){m1.x.w, m1.y.w, m1.z.w, m1.w.w}, m2.z);

    r.w.x = vecDot((vec4){m1.x.x, m1.y.x, m1.z.x, m1.w.x}, m2.w);
    r.w.y = vecDot((vec4){m1.x.y, m1.y.y, m1.z.y, m1.w.y}, m2.w);
    r.w.z = vecDot((vec4){m1.x.z, m1.y.z, m1.z.z, m1.w.z}, m2.w);
    r.w.w = vecDot((vec4){m1.x.w, m1.y.w, m1.z.w, m1.w.w}, m2.w);
    return r;
}

mat4 matInv(mat4 m) {
    float det = matDet(m);
    mat4 adj;
    adj.x.x = (m.y.y*m.z.z*m.w.w)+(m.z.y*m.w.z*m.y.w)+(m.w.y*m.y.z*m.z.w)-(m.w.y*m.z.z*m.y.w)-(m.z.y*m.y.z*m.w.w)-(m.y.y*m.w.z*m.z.w);
    adj.y.x = (m.x.y*m.z.z*m.w.w)+(m.z.y*m.w.z*m.x.w)+(m.w.y*m.x.z*m.z.w)-(m.w.y*m.z.z*m.x.w)-(m.z.y*m.x.z*m.w.w)-(m.x.y*m.w.z*m.z.w);
    adj.z.x = (m.x.y*m.y.z*m.w.w)+(m.y.y*m.w.z*m.x.w)+(m.w.y*m.x.z*m.y.w)-(m.w.y*m.y.z*m.x.w)-(m.y.y*m.x.z*m.w.w)-(m.x.y*m.w.z*m.y.w);
    adj.w.x = (m.x.y*m.y.z*m.z.w)+(m.y.y*m.z.z*m.x.w)+(m.z.y*m.x.z*m.y.w)-(m.z.y*m.y.z*m.x.w)-(m.y.y*m.x.z*m.z.w)-(m.x.y*m.z.z*m.y.w);
    adj.x.y = (m.y.x*m.z.z*m.w.w)+(m.z.x*m.w.z*m.y.w)+(m.w.x*m.y.z*m.z.w)-(m.w.x*m.z.z*m.y.w)-(m.z.x*m.y.z*m.w.w)-(m.y.x*m.w.z*m.z.w);
    adj.y.y = (m.x.x*m.z.z*m.w.w)+(m.z.x*m.w.z*m.x.w)+(m.w.x*m.x.z*m.z.w)-(m.w.x*m.z.z*m.x.w)-(m.z.x*m.x.z*m.w.w)-(m.x.x*m.w.z*m.z.w);
    adj.z.y = (m.x.x*m.y.z*m.w.w)+(m.y.x*m.w.z*m.x.w)+(m.w.x*m.x.z*m.y.w)-(m.w.x*m.y.z*m.x.w)-(m.y.x*m.x.z*m.w.w)-(m.x.x*m.w.z*m.y.w);
    adj.w.y = (m.x.x*m.y.z*m.z.w)+(m.y.x*m.z.z*m.x.w)+(m.z.x*m.x.z*m.y.w)-(m.z.x*m.y.z*m.x.w)-(m.y.x*m.x.z*m.z.w)-(m.x.x*m.z.z*m.y.w);
    adj.x.z = (m.y.x*m.z.y*m.w.w)+(m.z.x*m.w.y*m.y.w)+(m.w.x*m.y.y*m.z.w)-(m.w.x*m.z.y*m.y.w)-(m.z.x*m.y.y*m.w.w)-(m.y.x*m.w.y*m.z.w);
    adj.y.z = (m.x.x*m.z.y*m.w.w)+(m.z.x*m.w.y*m.x.w)+(m.w.x*m.x.y*m.z.w)-(m.w.x*m.z.y*m.x.w)-(m.z.x*m.x.y*m.w.w)-(m.x.x*m.w.y*m.z.w);
    adj.z.z = (m.x.x*m.y.y*m.w.w)+(m.y.x*m.w.y*m.x.w)+(m.w.x*m.x.y*m.y.w)-(m.w.x*m.y.y*m.x.w)-(m.y.x*m.x.y*m.w.w)-(m.x.x*m.w.y*m.y.w);
    adj.w.z = (m.x.x*m.y.y*m.z.w)+(m.y.x*m.z.y*m.x.w)+(m.z.x*m.x.y*m.y.w)-(m.z.x*m.y.y*m.x.w)-(m.y.x*m.x.y*m.z.w)-(m.x.x*m.z.y*m.y.w);
    adj.x.w = (m.y.x*m.z.y*m.w.z)+(m.z.x*m.w.y*m.y.z)+(m.w.x*m.y.y*m.z.z)-(m.w.x*m.z.y*m.y.z)-(m.z.x*m.y.y*m.w.z)-(m.y.x*m.w.y*m.z.z);
    adj.y.w = (m.x.x*m.z.y*m.w.z)+(m.z.x*m.w.y*m.x.z)+(m.w.x*m.x.y*m.z.z)-(m.w.x*m.z.y*m.x.z)-(m.z.x*m.x.y*m.w.z)-(m.x.x*m.w.y*m.z.z);
    adj.z.w = (m.x.x*m.y.y*m.w.z)+(m.y.x*m.w.y*m.x.z)+(m.w.x*m.x.y*m.y.z)-(m.w.x*m.y.y*m.x.z)-(m.y.x*m.x.y*m.w.z)-(m.x.x*m.w.y*m.y.z);
    adj.w.w = (m.x.x*m.y.y*m.z.z)+(m.y.x*m.z.y*m.x.z)+(m.z.x*m.x.y*m.y.z)-(m.z.x*m.y.y*m.x.z)-(m.y.x*m.x.y*m.z.z)-(m.x.x*m.z.y*m.y.z);
    return matScale((1/det), adj);
}

float matDet(mat4 m) {
    float r = m.x.x*((m.y.y*m.z.z*m.w.w)+(m.z.y*m.w.z*m.y.w)+(m.w.y*m.y.z*m.z.w)-(m.w.y*m.z.z*m.y.w)-(m.z.y*m.y.z*m.w.w)-(m.y.y*m.w.z*m.z.w))
    -m.x.y*((m.y.x*m.z.z*m.w.w)+(m.z.x*m.w.z*m.y.w)+(m.w.x*m.y.z*m.z.w)-(m.w.x*m.z.z*m.y.w)-(m.z.x*m.y.z*m.w.w)-(m.y.x*m.w.z*m.z.w))
    +m.x.z*((m.y.x*m.z.y*m.w.w)+(m.z.x*m.w.y*m.y.w)+(m.w.x*m.y.y*m.z.w)-(m.w.x*m.z.y*m.y.w)-(m.z.x*m.y.y*m.w.w)-(m.y.x*m.w.y*m.z.w))
    -m.x.w*((m.y.x*m.z.y*m.w.z)+(m.z.x*m.w.y*m.y.z)+(m.w.x*m.y.y*m.z.z)-(m.w.x*m.z.y*m.y.z)-(m.z.x*m.y.y*m.w.z)-(m.y.x*m.w.y*m.z.z));
    return r;
}

mat4 matTrans(mat4 m) {
    mat4 r;
    r.x.x = m.x.x;
    r.x.y = m.y.x;
    r.x.z = m.z.x;
    r.x.w = m.w.x;
    r.y.x = m.x.y;
    r.y.y = m.y.y;
    r.y.z = m.z.y;
    r.y.w = m.w.y;
    r.z.x = m.x.z;
    r.z.y = m.y.z;
    r.z.z = m.z.z;
    r.z.w = m.w.z;
    r.w.x = m.x.w;
    r.w.y = m.y.w;
    r.w.z = m.z.w;
    r.w.w = m.w.w;
    return r;
}

vec4 matVecMult(vec4 v, mat4 m) {
    vec4 r;
    r.x = (m.x.x * v.x) + (m.y.x * v.y) + (m.z.x * v.z) + (m.w.x * v.w);
    r.y = (m.x.y * v.x) + (m.y.y * v.y) + (m.z.y * v.z) + (m.w.y * v.w);
    r.z = (m.x.z * v.x) + (m.y.z * v.y) + (m.z.z * v.z) + (m.w.z * v.w);
    r.w = (m.x.w * v.x) + (m.y.w * v.y) + (m.z.w * v.z) + (m.w.w * v.w);
    //printf("%7.3f = (%7.3f * %7.3f) + (%7.3f * %7.3f) + (%7.3f * %7.3f) + (%7.3f * %7.3f);\n", r.x,m.x.x,v.x,m.y.x,v.y,m.z.x,v.z,m.w.x,v.w);
    //printf("%7.3f = (%7.3f * %7.3f) + (%7.3f * %7.3f) + (%7.3f * %7.3f) + (%7.3f * %7.3f);\n", r.y,m.x.y,v.x,m.y.y,v.y,m.z.y,v.z,m.w.y,v.w);
    //printf("%7.3f = (%7.3f * %7.3f) + (%7.3f * %7.3f) + (%7.3f * %7.3f) + (%7.3f * %7.3f);\n", r.z,m.x.z,v.x,m.y.z,v.y,m.z.z,v.z,m.w.z,v.w);
    //printf("%7.3f = (%7.3f * %7.3f) + (%7.3f * %7.3f) + (%7.3f * %7.3f) + (%7.3f * %7.3f);\n", r.w,m.x.w,v.x,m.y.w,v.y,m.z.w,v.z,m.w.w,v.w);
    //printf("\n");
    return r;
}

mat4 identity() {
    mat4 r;
    r.x.x = 1;
    r.x.y = 0;
    r.x.z = 0;
    r.x.w = 0;
    r.y.x = 0;
    r.y.y = 1;
    r.y.z = 0;
    r.y.w = 0;
    r.z.x = 0;
    r.z.y = 0;
    r.z.z = 1;
    r.z.w = 0;
    r.w.x = 0;
    r.w.y = 0;
    r.w.z = 0;
    r.w.w = 1;
    return r;
}

mat4 translate(float x, float y, float z){
    mat4 r = identity();
    r.w.x = x;
    r.w.y = y;
    r.w.z = z;
    r.w.w = 1;
    return r;
}

mat4 scale(float x, float y, float z){
    mat4 r = identity();
    r.x.x = x;
    r.y.y = y;
    r.z.z = z;
    return r;
}


//1 = x
//2 = y
//3 = z
mat4 rotate(float deg, int axis){
    mat4 r = identity();
    float rad = deg * M_PI / 180.00;
    //printf("%7.3f", rad);
    if (axis == 1) {
        r.y.y = cos(rad);
        r.z.y = -sin(rad);
        r.y.z = sin(rad);
        r.z.z = cos(rad);
    } else if (axis == 2) {
        r.x.x = cos(rad);
        r.z.x = sin(rad);
        r.x.z = -sin(rad);
        r.z.z = cos(rad);
    } else if (axis == 3) {
        r.x.x = cos(rad);
        r.y.x = -sin(rad);
        r.x.y = sin(rad);
        r.y.y = cos(rad);
    }
    //printf("%d\n",axis);
    //matPrint(r);
    //printf("\n");
    return r;
}