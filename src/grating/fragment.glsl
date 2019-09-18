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

out vec3 color;

void main()
{
    // circular aperature
    if (abs(distance(gl_FragCoord.xy, center)) > radius)
    {
        discard;
    }

    // use position relative to center, so center pixel is full white 1.0
    vec2 rel_pos = gl_FragCoord.xy - center;

    // current texel position given our orientation
    float pos = rel_pos.x * cos(ori) + rel_pos.y * sin(ori);

    // grayscale value cos(0) -> 1.0, so bright phase (white) is always centered
    float sv = cos(pos * freq + g_phase);

    color = (0.5 * contrast * vec3(1.0f, 1.0f, 1.0f) * sv) + 0.5f;
}
