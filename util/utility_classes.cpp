#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "util/utility_classes.hpp"
#include "util/program.hpp"
#include "util/bmp.hpp"

/* ========================================================================== */
void GLProgram::close()
{
    if (_state)
    {
        std::cout << "Deleting program..." << std::endl;
        glDeleteProgram(get_id());
        _state = false;
    }
}

GLint GLProgram::get_attribute(const char* name)
{
    GLint attribute = glGetAttribLocation(get_id(), name);
    if (attribute == -1)
    {
        fprintf(stderr, "Could not bind attribute %s\n", name);
    }
    return attribute;
}

GLint GLProgram::get_uniform(const char* name)
{
    GLint uniform = glGetUniformLocation(get_id(), name);
    if (uniform == -1)
    {
        fprintf(stderr, "Could not bind uniform %s\n", name);
    }
    return uniform;
}
/* ========================================================================== */
GLVBO::GLVBO()
{
    glGenVertexArrays(1, &get_id());
    glBindVertexArray(get_id());
}

void GLVBO::close()
{
    if (_state)
    {
        std::cout << "Deleting VBO..." << std::endl;
        glDeleteVertexArrays(1, &get_id());
        _state = false;
    }
}
/* ========================================================================== */
GLTexture::GLTexture(const BMPImage& im)
{
    glGenTextures(1, &get_id());

    // "Bind" the newly created texture : all future texture functions will modify this texture
    bind();

    // Give the image to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, im.width(), im.height(), 0,
        GL_BGR, GL_UNSIGNED_BYTE, im.data());

    // ... nice trilinear filtering ...
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    // ... which requires mipmaps. Generate them automatically.
    glGenerateMipmap(GL_TEXTURE_2D);
}

GLTexture::GLTexture(const DDSTexture& im)
{
    glGenTextures(1, &get_id());

    bind();

    // set pixel storage mode: single byte
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    GLenum format = get_format(im);

    unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
    unsigned int offset = 0;

    unsigned int wd = im.width();
    unsigned int ht = im.height();

    std::cout <<
        "wd = " << wd <<
        " ht = " << ht <<
        " nmap = " << im.nmap() <<
        std::endl;

    unsigned char* ptr = im.data();

    /* load the mipmaps */
    for (unsigned int level = 0; level < im.nmap() && (wd || ht); ++level)
    {
        unsigned int size = ((wd+3)/4) * ((ht+3)/4) * blockSize;

        glCompressedTexImage2D(GL_TEXTURE_2D, level, format, wd, ht,
            0, size, ptr + offset);

        offset += size;
        wd /= 2;
        ht /= 2;

        // deal with Non-Power-Of-Two textures
        if(wd < 1) { wd = 1; }
        if(ht < 1) { ht = 1; }
    }

    // When MAGnifying the image (no bigger mipmap available), use LINEAR filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // When MINifying the image, use a LINEAR blend of two mipmaps, each filtered LINEARLY too
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}

void GLTexture::close()
{
    if (_state)
    {
        std::cout << "Deleteing texture..." << std::endl;
        glDeleteTextures(1, &get_id());
        _state = false;
    }
}
/* ========================================================================== */
