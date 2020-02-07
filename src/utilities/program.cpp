#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>

#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>

#include "program.hpp"

int compile_shader(const char* filepath, GLenum shader_type, GLuint& shader_id)
{
    shader_id = glCreateShader(shader_type);

    // Read the Vertex Shader code from the file
    std::string src;
    std::ifstream src_stream(filepath, std::ios::in);
    if(src_stream.is_open())
    {
        std::stringstream sstr;
        sstr << src_stream.rdbuf();
        src = sstr.str();
        src_stream.close();
    }
    else
    {
        printf("Failed to open %s!\n", filepath);
        printf("Are you in the right directory? Don't forget to read the FAQ!\n");
        getchar();
        return 0;
    }

    GLint result = GL_FALSE;
    int log_length;

    // Compile Shader
    printf("Compiling shader : %s\n", filepath);
    const char* src_ptr = src.c_str();
    glShaderSource(shader_id, 1, &src_ptr , NULL);
    glCompileShader(shader_id);

    // Check Shader
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);

    if (log_length > 0)
    {
        std::vector<char> err_msg(log_length+1);
        glGetShaderInfoLog(shader_id, log_length, NULL, &err_msg[0]);
        printf("%s\n", &err_msg[0]);
        return 0;
    }

    return 1;
}

GLuint link_program(GLuint vshader_id, GLuint fshader_id)
{
    // Link the program
    printf("Linking program\n");
    GLuint program_id = glCreateProgram();
    glAttachShader(program_id, vshader_id);
    glAttachShader(program_id, fshader_id);
    glLinkProgram(program_id);

    GLint result = GL_FALSE;
    int log_length;

    // Check the program
    glGetProgramiv(program_id, GL_LINK_STATUS, &result);
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);

    if (log_length > 0)
    {
        std::vector<char> err_msg(log_length+1);
        glGetProgramInfoLog(program_id, log_length, NULL, &err_msg[0]);
        printf("%s\n", &err_msg[0]);
    }

    glDetachShader(program_id, vshader_id);
    glDetachShader(program_id, fshader_id);

    glDeleteShader(vshader_id);
    glDeleteShader(fshader_id);

    return program_id;
}

GLuint create_program(const char* vertex_filepath, const char* fragment_filepath)
{
    GLuint vshader_id, fshader_id;
    if (!compile_shader(vertex_filepath, GL_VERTEX_SHADER, vshader_id))
    {
        fprintf(stderr, "Failed to compile: %s\n", vertex_filepath);
        return 0x00;
    }

    if (!compile_shader(fragment_filepath, GL_FRAGMENT_SHADER, fshader_id))
    {
        fprintf(stderr, "Failed to compile: %s\n",  fragment_filepath);
        return 0x00;
    }

    return link_program(vshader_id, fshader_id);
}
