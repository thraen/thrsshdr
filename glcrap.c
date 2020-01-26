#include <string.h>

#include "globals.h"
#include "glcrap.h"

void init_texture(GLuint text, unsigned int w, unsigned int h) {
    glBindTexture(GL_TEXTURE_2D, text);
    ////         target,        level, internalformat, w, height, border, format,  type,   void * data
    glTexImage2D(GL_TEXTURE_2D, 0,     GL_RGBA32F,     w, h,      0,      GL_RGBA, GL_INT, NULL); 
    //// XXX setting type == GL_INT should imply not clamping color values. It still does clamp :(
}

void setup_render_texture(GLuint text, unsigned int w, unsigned int h){
    init_texture(text, w, h);
    // Poor filtering. Needed!
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

char* read_file(const char *fn) {
    dbg("read_file %s\n", fn);
    FILE *f     = fopen(fn, "rb");
    fseek(f, 0, SEEK_END);
    size_t flen = ftell(f);
    rewind(f);

    char *ret = (char*) malloc( (flen+1)* sizeof(char) );
    if ( flen != fread(ret, sizeof(char), flen, f) )
        errexit("error reading file %s\n", fn);

    ret[flen] = '\0';
    fclose(f);
    return ret;
}

void add_shader( GLuint shader_program, size_t srcc, const char **srcv, GLenum shader_type ) {
    dbg("attaching %lu shader sources of type %d to program: %d \n", srcc, shader_type, shader_program);

    GLuint shader = glCreateShader(shader_type);

    if (shader == 0)
        errexit("Error creating shader type %d\n", shader_type);

    GLint len[srcc];
    for (int i = srcc; i--;)
        len[i] = strlen(srcv[i]);

    glShaderSource(shader, srcc, srcv, len);
    glCompileShader(shader);
    GLint good;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &good);
    if (!good) {
        GLchar err[1024];
        glGetShaderInfoLog(shader, 1024, NULL, err);
        errexit("Error compiling shader type %d:\n%s\n", shader_type, err);
    }

    glAttachShader(shader_program, shader);
}

GLuint uniform_loc( GLuint shader_program, const char* s, int strict ) {
    GLuint ret = glGetUniformLocation(shader_program, s);
    if (strict && ret == 0xFFFFFFFF && s)
        errexit("Uniform  %s  not active!", s);

    return ret;
}

void remove_shaders( GLuint shader_program ) {
    dbg("remove_shaders for program %d\n",shader_program);
    GLsizei max_count = 3; // thr!!
    GLuint  shaders[max_count];
    GLsizei count;
    int     i;

    glGetAttachedShaders(shader_program,  max_count,  &count,  shaders);
    dbg("remove_shaders, found: %d in program: %d \n", count, shader_program);
    for (i=0; i<count; ++i) {
        dbg("deleting shader %d, %d\n", i, shaders[i]);
        glDetachShader(shader_program, shaders[i]);
        glDeleteShader(shaders[i]);
    }
}

void recompile_shaders(Shdr *r, int assert_uniform ) {
    remove_shaders(r->shader_program);

    char shared_defs[1000];
    snprintf( shared_defs, sizeof(shared_defs), 
              "#version 430 core \n "
              "#define _nfreq %d \n "
              "#define _nband %d \n "
              , _nfreq, _nband );

    size_t srcc = 3;
    const char *vert_src[srcc];
    vert_src[0] = shared_defs;
    vert_src[1] = read_file("header.vert");
    vert_src[2] = read_file(r->vert_src_name);

    add_shader(r->shader_program, srcc, vert_src, GL_VERTEX_SHADER);

    const char *frag_src[srcc];
    frag_src[0] = shared_defs;
    frag_src[1] = read_file("header.frag");
    frag_src[2] = read_file(r->frag_src_name);

    add_shader(r->shader_program, srcc, frag_src , GL_FRAGMENT_SHADER);

    GLint  good  = 0;
    GLchar err[1024];
    
    glLinkProgram(r->shader_program);
    glGetProgramiv(r->shader_program, GL_LINK_STATUS, &good);
    if (!good) {
        glGetProgramInfoLog(r->shader_program, sizeof(err), NULL, err);
        errexit("Error linking shader program:\n'%s'\n", err);
    }

    glValidateProgram(r->shader_program);
    glGetProgramiv(r->shader_program, GL_VALIDATE_STATUS, &good);
    if (!good) {
        glGetProgramInfoLog(r->shader_program, sizeof(err), NULL, err);
        (stderr, "Invalid shader program:\n'%s'\n", err);
        exit(1);
    }

    r->Ecoarse_   = uniform_loc(r->shader_program, "Ecoarse",    assert_uniform);

    r->E_         = uniform_loc(r->shader_program, "E",          assert_uniform);
    r->nband_     = uniform_loc(r->shader_program, "_nband",     assert_uniform);

    r->labsX_     = uniform_loc(r->shader_program, "labsX",      assert_uniform);
    r->nfreq_     = uniform_loc(r->shader_program, "_nfreq",     assert_uniform);

    r->w_         = uniform_loc(r->shader_program, "_w",         assert_uniform);
    r->h_         = uniform_loc(r->shader_program, "_h",         assert_uniform);
    r->elapsed_t_ = uniform_loc(r->shader_program, "_elapsed_t", assert_uniform);

    for(;--srcc;) { // naughty
        dbg("freeing shader source array %lu\n", srcc);
        free((void*)vert_src[srcc]);
        free((void*)frag_src[srcc]);
    }

    dbg("glUseProgram %d\n", r->shader_program);
    glUseProgram(r->shader_program);
}

void set_global_uniforms(Shdr *r){
    glUniform1i(r->w_, _w);
    glUniform1i(r->h_, _h);

    glUniform1i(r->elapsed_t_, _elapsed_t);

    glUniform1i(r->nband_, _nband);
    glUniform1i(r->nfreq_, _nfreq);

    glUniform1fv(r->E_,       _nband, E);
    glUniform1fv(r->Ecoarse_, 3,      Ecoarse);
    glUniform1fv(r->labsX_,   _nfreq, labsX);
}

void init_rect() {
    float vertices[6][3] = {
        {-1.0f, -1.0f,  0.0f},
        { 1.0f, -1.0f,  0.0f},
        {-1.0f,  1.0f,  0.0f},
        {-1.0f,  1.0f,  0.0f},
        { 1.0f, -1.0f,  0.0f},
        { 1.0f,  1.0f,  0.0f}
    };

    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);
    dbg("init_rect. init vbo, vao %o %lu\n", vbo, sizeof(vertices));

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
}

void init_shdr( Shdr *r, const char *vsrc_name, const char *fsrc_name, int assert_uniform ){
    r->vert_src_name = vsrc_name;
    r->frag_src_name = fsrc_name;

    r->shader_program = glCreateProgram();
    if (r->shader_program == 0) errexit("Error creating shader program\n");

    recompile_shaders(r, assert_uniform);

    r->u_now_ = uniform_loc(r->shader_program, "u_now", assert_uniform);
    r->u_prv_ = uniform_loc(r->shader_program, "u_prv", assert_uniform);
}

void inline setup_draw(Shdr *r) {
    glUseProgram(r->shader_program);
    set_global_uniforms(r);
}

void draw0(Shdr *r) {
    setup_draw(r);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void draw1( Shdr *r, GLuint texture ) {
    setup_draw(r);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(r->u_now_, 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void draw2( Shdr *r, GLuint tex1, GLuint tex2 ) {
    setup_draw(r);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex1);
    glUniform1i(r->u_now_, 0);

    glActiveTexture(GL_TEXTURE0+1);
    glBindTexture(GL_TEXTURE_2D, tex2);
    glUniform1i(r->u_prv_, 1);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

