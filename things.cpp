#include "things.h"

void Things::recompile_shaders()  {
    remove_shaders(shader_program);

    char* vert_src = readFile(vert_src_name);
    add_shader(shader_program, vert_src, GL_VERTEX_SHADER);

    char* frag_src = readFile(frag_src_name);
    add_shader(shader_program, frag_src, GL_FRAGMENT_SHADER);

    GLint  Success        = 0;
    GLchar ErrorLog[1024] = { 0 };

    glLinkProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &Success);
    if (Success == 0) {
        glGetProgramInfoLog(shader_program, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Error linking shader program:\n'%s'\n", ErrorLog);
        exit(1);
    }

    glValidateProgram(shader_program);
    glGetProgramiv(shader_program, GL_VALIDATE_STATUS, &Success);
    if (!Success) {
        glGetProgramInfoLog(shader_program, sizeof(ErrorLog), NULL, ErrorLog);
        (stderr, "Invalid shader program:\n'%s'\n", ErrorLog);
        (1);
    }

    _mid_       = glGetUniformLocation(shader_program, "mid"); // it has to be used in the shader, im sinne von die variable muss auch benutzt werden!
    _hig_       = glGetUniformLocation(shader_program, "hig");
    _low_       = glGetUniformLocation(shader_program, "low");
    _E_         = glGetUniformLocation(shader_program, "E");
    _w_         = glGetUniformLocation(shader_program, "_w");
    _h_         = glGetUniformLocation(shader_program, "_h");
    _elapsed_t_ = glGetUniformLocation(shader_program, "_elapsed_t");
//  assert(_E_          != 0xFFFFFFFF);
//  assert(_mid_        != 0xFFFFFFFF);
//  assert(_hig_        != 0xFFFFFFFF);
//  assert(_low_        != 0xFFFFFFFF);

//  assert(_h_          != 0xFFFFFFFF);
//  assert(_w_          != 0xFFFFFFFF);
//  assert(_elapsed_t_  != 0xFFFFFFFF);

    free(vert_src);
    free(frag_src);

    fprintf(stderr, "glUseProgram %d\n", shader_program);
    glUseProgram(shader_program);
}

void Things::set_global_uniforms(){
//  glUniform1f(_low_, sinf( low ));
    glUniform1f(_low_, low);
    glUniform1f(_mid_, mid);
    glUniform1f(_hig_, hig);

    glUniform1i(_w_, _w);
    glUniform1i(_h_, _h);

    glUniform1i(_elapsed_t_, _elapsed_t);

    glUniform1fv(_E_, _nbands, E);
}

#include "dreieck.cpp"
#include "quad.cpp"

