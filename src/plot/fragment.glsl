#version 330 core

uniform vec4 color;

void main(void)
{
	gl_FragColor = color;

    // vec2 pos = mod(gl_FragCoord.xy, vec2(50.0)) - vec2(25.0);
    // float dist_squared = dot(pos, pos);
    //
    // gl_FragColor = (dist_squared < 400.0)
    //     ? color
    //     : vec4(1.0, 1.0, 1.0, 1.0);
}