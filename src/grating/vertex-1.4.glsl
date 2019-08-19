#version 120

attribute vec3 vertexPosition;
attribute float phase;

varying float g_phase;

void main()
{
    gl_Position.xyz = vertexPosition;
    gl_Position.w = 1.0;

    g_phase = phase;
}
