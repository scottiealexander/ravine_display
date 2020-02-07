#ifndef UTILITY_CLASSES_HPP_
#define UTILITY_CLASSES_HPP_

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "program.hpp"
#include "bmp.hpp"

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

struct Point
{
    GLfloat x;
    GLfloat y;
};

/* ========================================================================== */
class GLObject
{
public:
    GLObject() : _state(true) {}
    virtual void close() = 0;
    inline void set_id(GLuint id) { _id = id; }
    inline GLuint& get_id() { return _id; }
protected:
    GLuint _id;
    bool _state;
};
/* ========================================================================== */
// cover GLFloatBuffer, GLPointBuffer, etc.
class GLArrayBuffer : public GLObject
{
    GLArrayBuffer() : _length(0) { glGenBuffers(1, &get_id()); }
    ~GLArrayBuffer() { close(); }
    void close() override
    {
        if (_state)
        {
            std::cout << "Deleting buffer!" << std::endl;
            glDeleteBuffers(1, &get_id());
            _state = false;
        }
    }

    inline void bind() { glBindBuffer(GL_ARRAY_BUFFER, get_id()); }
    inline size_t length() const { return _length; }

    template<typename T>
    void set_data(const T* data, size_t length, GLenum draw_type = GL_STATIC_DRAW)
    {
        // set this buffer as the current one that we're operating on
        bind();

        // give verticies to OpenGL
        glBufferData(GL_ARRAY_BUFFER, length * sizeof(T), data, draw_type);

        _length = length;
    }

private:
    size_t _length;
};
/* ========================================================================== */
class GLProgram : public GLObject
{
public:
    GLProgram(const char* vshader, const char* fshader)
    {
        set_id(create_program(vshader, fshader));
    }
    GLProgram(GLuint id) { set_id(id); }
    ~GLProgram () { close(); }
    void close() override;
    GLint get_attribute(const char* name);
    GLint get_uniform(const char* name);
    inline void use() { glUseProgram(get_id()); }
};
/* ========================================================================== */
class GLVBO : public GLObject
{
public:
    GLVBO();
    ~GLVBO() { close(); }
    void close() override;
};
/* ========================================================================== */
class GLTexture : public GLObject
{
public:
    GLTexture(const BMPImage& im);
    GLTexture(const DDSTexture& im);
    ~GLTexture() { close(); }
    void close() override;
    inline void bind() { glBindTexture(GL_TEXTURE_2D, get_id()); }

private:
    GLenum get_format(const DDSTexture& im)
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
};
/* ========================================================================== */
#endif
