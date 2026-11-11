#version 120

attribute vec4 vPosition;
attribute vec4 vColor;
attribute vec4 vNormal;

varying vec4 color;

uniform mat4 ctm;
uniform mat4 modelView;
uniform mat4 projection;

vec4 light_position = vec4(-1, 2, 1, 1);


void main()
{
    vec4 L = normalize(light_position);
    vec4 N = normalize(ctm * vNormal);
    color = max(dot(L,N), 0.0) * vec4(0.8, 0.8, 0.8, 1.0);
    //gl_Position = modelView * vPosition;
    gl_Position = projection * modelView * vPosition;
}
