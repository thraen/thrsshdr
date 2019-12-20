#include "things.h"

void Rect::recompile_shaders( bool assert_uniform ) {

    remove_shaders(shader_program);

    char shared_defs[1000];
    snprintf( shared_defs, sizeof(shared_defs), 
              "#version 450 core \n "
              "#define _nfreq %d \n "
              "#define _nband %d \n "
              , _nfreq, _nband );

    size_t srcc = 3;
    const char *vert_src[srcc] = { 
        shared_defs,
        read_file("header.vert"),
        read_file(vert_src_name) 
    };
    add_shader(shader_program, srcc, vert_src, GL_VERTEX_SHADER);

    const char *frag_src[srcc] = { 
        shared_defs,
        read_file("header.frag"),
        read_file(frag_src_name)
    };
    add_shader(shader_program, srcc, frag_src , GL_FRAGMENT_SHADER);

    GLint  good  = 0;
    GLchar err[1024];
    
    glLinkProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &good);
    if (!good) {
        glGetProgramInfoLog(shader_program, sizeof(err), NULL, err);
        errexit("Error linking shader program:\n'%s'\n", err);
    }

    glValidateProgram(shader_program);
    glGetProgramiv(shader_program, GL_VALIDATE_STATUS, &good);
    if (!good) {
        glGetProgramInfoLog(shader_program, sizeof(err), NULL, err);
        (stderr, "Invalid shader program:\n'%s'\n", err);
        exit(1);
    }

    _low_       = uniform_loc(shader_program, "low", assert_uniform);
    _mid_       = uniform_loc(shader_program, "mid", assert_uniform);
    _hig_       = uniform_loc(shader_program, "hig", assert_uniform);

    _E_         = uniform_loc(shader_program, "E",      assert_uniform);
    _nband_     = uniform_loc(shader_program, "_nband", assert_uniform);

    _normX_     = uniform_loc(shader_program, "normX",  assert_uniform);
    _nfreq_     = uniform_loc(shader_program, "_nfreq", assert_uniform);

    _w_         = uniform_loc(shader_program, "_w", assert_uniform);
    _h_         = uniform_loc(shader_program, "_h", assert_uniform);
    _elapsed_t_ = uniform_loc(shader_program, "_elapsed_t", assert_uniform);

    for(;--srcc;) { // naughty
        dbg("freeing shader source array %d\n", srcc);
        free((void*)vert_src[srcc]);
        free((void*)frag_src[srcc]);
    }

    dbg("glUseProgram %d\n", shader_program);
    glUseProgram(shader_program);
}

void Rect::set_global_uniforms(){
    glUniform1f(_low_, low);
    glUniform1f(_mid_, mid);
    glUniform1f(_hig_, hig);

    glUniform1i(_w_, _w);
    glUniform1i(_h_, _h);

    glUniform1i(_elapsed_t_, _elapsed_t);

    glUniform1i(_nband_, _nband);
    glUniform1i(_nfreq_, _nfreq);


    glUniform1fv(_E_,     _nband, E);
    glUniform1fv(_normX_, _nfreq, normX);
}

void Rect::init( const char *v_src_name, const char *f_src_name, bool assert_uniform ){
    vert_src_name = v_src_name;
    frag_src_name = f_src_name;

    Vector3f Vertices[6];
    Vertices[0] = Vector3f(-1.0f, -1.0f,  0.0f);
    Vertices[1] = Vector3f( 1.0f, -1.0f,  0.0f);
    Vertices[2] = Vector3f(-1.0f,  1.0f,  0.0f);
    Vertices[3] = Vector3f(-1.0f,  1.0f,  0.0f);
    Vertices[4] = Vector3f( 1.0f, -1.0f,  0.0f);
    Vertices[5] = Vector3f( 1.0f,  1.0f,  0.0f);

    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);
    dbg("Rect init. %o %lu\n", vbo, sizeof(Vertices));

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    shader_program = glCreateProgram();
    if (shader_program == 0) errexit("Error creating shader program\n");

    recompile_shaders(assert_uniform);

    u_now_ = uniform_loc(shader_program, "u_now", assert_uniform);
    u_prv_ = uniform_loc(shader_program, "u_prv", assert_uniform);
}

void inline Rect::setup_draw( GLuint shader_program, GLuint vao ) {
    glUseProgram(shader_program);
    glBindVertexArray(vao);
    set_global_uniforms();

//     glEnableVertexAttribArray(0);
//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void inline Rect::cleanup_draw() {
//     glDisableVertexAttribArray(0);
}

void Rect::draw() {
    setup_draw(shader_program, vao);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    cleanup_draw();
}

void Rect::draw( GLuint texture ) {
    setup_draw(shader_program, vao);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(u_now_, 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    cleanup_draw();
}

void Rect::draw( GLuint tex1, GLuint tex2 ) {
    setup_draw(shader_program, vao);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex1);
    glUniform1i(u_now_, 0);

    glActiveTexture(GL_TEXTURE0+1);
    glBindTexture(GL_TEXTURE_2D, tex2);
    glUniform1i(u_prv_, 1);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    cleanup_draw();
}

