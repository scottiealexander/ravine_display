#ifndef PROGRAM_HPP_
#define PROGRAM_HPP_

GLuint create_program(const char* vertex_filepath, const char* fragment_filepath);

GLuint link_program(GLuint fshader_id, GLuint vshader_id);

int compile_shader(const char* filepath, GLenum shader_type, GLuint& chader_id);

#endif
