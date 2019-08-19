#ifndef UTILITY_CLASSES_HPP_
#define UTILITY_CLASSES_HPP_

#include <iostream>
#include "util/program.hpp"
#include "util/bmp.hpp"

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

struct Point
{
    GLfloat x;
    GLfloat y;
};

/* ========================================================================== */
class GLFloatBuffer
{
public:
    GLFloatBuffer(size_t ncol, size_t nrow): ncol(ncol), nrow(nrow)
    {
        // generate the buffer and get it's id
        glGenBuffers(1, &id);
    }

    ~GLFloatBuffer() { std::cout << "Deleting buffer.." << std::endl; glDeleteBuffers(1, &id); }

    inline void bind() { glBindBuffer(GL_ARRAY_BUFFER, id); }

    inline GLuint getID() { return id; }

    void setData(const GLfloat* data)
    {
        // set this buffer as the current one that we're operating on
        bind();

        // give verticies to OpenGL
        glBufferData(GL_ARRAY_BUFFER, ncol * nrow * sizeof(float), data, GL_STATIC_DRAW);
    }

private:
    GLuint id;
    size_t ncol;
    size_t nrow;
};
/* ========================================================================== */
class GLPointBuffer
{
public:
    GLPointBuffer() : npt(0)
    {
        // generate the buffer and get it's id
        glGenBuffers(1, &id);
    }

    ~GLPointBuffer()
    {
        std::cout << "Deleting buffer.." << std::endl;
        glDeleteBuffers(1, &id);
    }

    inline void bind() const { glBindBuffer(GL_ARRAY_BUFFER, id); }

    inline GLuint getID() const { return id; }

    inline size_t getLength() const { return npt; }

    void setData(const Point* data, size_t length, GLenum drawType)
    {
        // set this buffer as the current one that we're operating on
        bind();

        // give verticies to OpenGL
        glBufferData(GL_ARRAY_BUFFER, length * sizeof(Point), data, drawType);

        npt = length;
    }

private:
    GLuint id;
    size_t npt;
};
/* ========================================================================== */
class GLProgram
{
public:
    GLProgram(const char* vshader, const char* fshader) :
    id(create_program(vshader, fshader)), state(true)
    {}
    GLProgram(GLuint id) : id(id), state(true) {}
    ~GLProgram()
    {
        if (state)
        {
            std::cout << "Deleting program..." << std::endl;
            glDeleteProgram(id);
            state = false;
        }
    }
    void close()
    {
        glDeleteProgram(id);
        state = false;
    }
    GLint getAttribute(const char* name)
    {
        GLint attribute = glGetAttribLocation(getID(), name);
        if (attribute == -1)
        {
            fprintf(stderr, "Could not bind attribute %s\n", name);
        }
        return attribute;
    }
    GLint getUniform(const char* name)
    {
        GLint uniform = glGetUniformLocation(getID(), name);
        if (uniform == -1)
        {
            fprintf(stderr, "Could not bind uniform %s\n", name);
        }
        return uniform;
    }
    inline GLuint getID() { return id; }
    inline void use() { glUseProgram(id); }
private:
    GLuint id;
    bool state;
};
/* ========================================================================== */
class GLVBO
{
public:
    GLVBO() : state(true)
    {
        glGenVertexArrays(1, &id);
        glBindVertexArray(id);
    }
    ~GLVBO()
    {
        if (state)
        {
            std::cout << "Deleting VBO..." << std::endl;
            glDeleteVertexArrays(1, &id);
            state = false;
        }
    }

    void close()
    {
        glDeleteVertexArrays(1, &id);
        state = false;
    }
    inline GLuint getID() { return id; }
private:
    GLuint id;
    bool state;
};
/* ========================================================================== */
class GLTexture
{
public:
    GLTexture(const BMPImage& im)
    {
        glGenTextures(1, &id);

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

    GLTexture(const DDSTexture& im)
    {
        glGenTextures(1, &id);

        bind();

        // set pixel storage mode: single byte
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        GLenum format = getFormat(im);

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

    ~GLTexture() { std::cout << "Deleting texture..." << std::endl; glDeleteTextures(1, &id); }

    inline void bind() { glBindTexture(GL_TEXTURE_2D, id); }
    inline GLuint getID() { return id; }

private:
    GLenum getFormat(const DDSTexture& im)
    {
        switch(im.format())
        {
        case FOURCC_DXT1:
            return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        case FOURCC_DXT3:
            return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        case FOURCC_DXT5:
            return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        default:
            return 0;
        }
    }

private:
    GLuint id;
};
/* ========================================================================== */
#endif
