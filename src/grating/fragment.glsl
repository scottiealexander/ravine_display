#version 330 core

uniform vec2 center;
uniform float radius;
uniform float freq;
uniform float contrast;
uniform float ori;

// should this be an attribute?, thus passed to vertex.glsl and then on to us here
// see PTB DriftDemo2 and Screen('DrawTexture')
in float g_phase;

in vec4 gl_FragCoord;

// Ouput data
out vec3 color;

void main()
{

    /* Query current output texel position: */
    float pos = gl_FragCoord.x * cos(ori) + gl_FragCoord.y * sin(ori);

    // circular aperature
    if (distance(gl_FragCoord.xy, center) > radius)
    {
        discard;
    }

    // cos() at current pixel given freq and phase
    float sv = cos(pos * freq + g_phase);

    color = (0.5 * contrast * vec3(1.0f, 1.0f, 1.0f) * sv) + 0.5f;
}
