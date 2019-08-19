#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition;

in float phase;
out float g_phase;
void main()
{
    gl_Position.xyz = vertexPosition;
    gl_Position.w = 1.0;

    g_phase = phase;
}
