#version 330 core

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

in vec4 gl_FragCoord;
out vec3 color;

void main()
{
    // scale factor for transforming corrdinate sum to row-major linear index
    float scale = round(float(px_height) / float(px_side_length));

    // convert screen coordinates to frame coordinates
    float x = floor(gl_FragCoord.x - px_origin.x);
    float y = floor(gl_FragCoord.y - px_origin.y);

    if (x < 0.0f || y < 0.0f) { discard; }

    // linear index of the "square" / "element" to which this fragment belongs
    // which also  cooresponds to the bit within <pattern> that holds the color
    int kbit = int((scale * floor(x / px_side_length)) + floor(y / px_side_length));

    // the element within <pattern> where we find the k'th bit <kbit>
    int kel = int(floor(kbit / INT_NBIT));

    // the bit within the <kel>th element of <pattern>
    int shift = kbit - (kel * INT_NBIT);

    // high-bit  = white, low-bit = black
    color = vec3(float((pattern[kel] >> shift) & 0x01));
}
