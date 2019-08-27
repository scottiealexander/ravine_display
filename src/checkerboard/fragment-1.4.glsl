#version 120

// number of bits in a GLSL int
const int INT_NBIT = 32;

// 16*16 bits indicating the state (black | white) of each element
uniform int pattern[8];

// screen position (in pixels) of the lower-left corner of our frame
uniform ivec2 px_origin;

// side length in pixels of the frame
uniform int px_height;

// side length of each element (square) in pixels
uniform int px_side_length;

void main()
{
    // scale factor for transforming corrdinate sum to row-major linear index
    float scale = round(float(px_height) / float(px_side_length));

    // convert screen coordinates to frame coordinates
    float x = floor(gl_FragCoord.x - px_origin.x);
    float y = floor(gl_FragCoord.y - px_origin.y);

    if (x < 0 || y < 0)
    {
        discard;
    }

    // linear index of the "square" / "element" to which this fragment belongs
    // which also  cooresponds to the bit within <pattern> that holds the color
    int kbit = int((scale * floor(x / px_side_length)) + floor(y / px_side_length));

    // gl_FragColor = vec4(1.0f);
    // gl_FragColor.xyz = vec3(float(kbit) / 255.0f);

    // the element within <pattern> where we find the k'th bit <kbit>
    int kel = int(floor(kbit / INT_NBIT));

    // gl_FragColor = vec4(1.0f);
    // gl_FragColor.xyz = vec3(float(kel) / 7.0f);

    // the bit within the <kel>th element of <pattern>
    int shift = kbit - (kel * INT_NBIT);

    // high-bit  = white, low-bit = black
    float ishigh = mod(pattern[kel] / int(pow(2, shift)), 2);

    if (ishigh == 0)
    {
        gl_FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
    else if (ishigh == 1)
    {
        gl_FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    else if (ishigh >= 0.5)
    {
        gl_FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    }
    else if (ishigh > 0)
    {
        gl_FragColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);
    }
    else
    {
        gl_FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
    }

    // gl_FragColor = vec4(1.0f);
    // gl_FragColor.xyz = vec3(float(ishigh));
}
