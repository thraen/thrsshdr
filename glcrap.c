#include <string.h>

#include "globals.h"
#include "glcrap.h"

void glerr() {
    int err =0, ext = 0;
    while(err = glGetError()) {
        nfo("error ");
        switch(err) {
            case GL_INVALID_OPERATION:             nfo("GL_INVALID_OPERATION\n");             break;
            case GL_INVALID_ENUM:                  nfo("GL_INVALID_ENUM\n");                  break;
            case GL_INVALID_VALUE:                 nfo("GL_INVALID_VALUE\n");                 break;
            case GL_OUT_OF_MEMORY:                 nfo("GL_OUT_OF_MEMORY\n");                 break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: nfo("GL_INVALID_FRAMEBUFFER_OPERATION\n"); break;
        }
        ext = 1;
    }
    if (ext) exit(0);
    else nfo("  :)\n");
}

char* make_shared_def() {
    const char *tmp = 
        "   #version 430 core                          \n"
        "   #define _nfreq %d                          \n"
        "   #define _nband %d                          \n"
        "   layout(shared, binding = 0) uniform Fuk    \n"
        "   {                                          \n"
        "       uniform int _w;                        \n"
        "       uniform int _h;                        \n"
        "       uniform int _elapsed_t;                \n"
        "       uniform float labsX[_nfreq];           \n"
        "       uniform float E[_nband];               \n"
        "       uniform float Ecoarse[3];              \n"
        "   };                                         \n";
    char *shared_defs = (char *) malloc(10000);
    snprintf( shared_defs, 10000, tmp, _nfreq, _nband );
    return shared_defs;
}

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

void add_shader( GLuint program, size_t srcc, const char **srcv, GLenum shader_type ) {
    dbg("attaching %lu shader sources of type %d to program: %d \n", srcc, shader_type, program);

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

    glAttachShader(program, shader);
}

GLuint block_offset( GLuint program, GLuint interface_type, const char* uniform_name ) {
    GLuint ind = glGetProgramResourceIndex(program, interface_type, uniform_name);
    const GLenum query_properties[1] = { GL_OFFSET };
    GLint props[1];
    glGetProgramResourceiv(program, interface_type, ind, 1, query_properties, 1, NULL, props);
    dbg("block_offset(%u, %s) determined offset %d\n", program, uniform_name, props[0]);
    return props[0];
}

GLuint uniform_loc( GLuint program, const char* uniform_name, int strict ) {
    GLuint ret = glGetUniformLocation(program, uniform_name);
    if (strict && ret == 0xFFFFFFFF && uniform_name)
        errexit("Uniform  %s  not active!", uniform_name);

    return ret;
}

void remove_shaders( GLuint program ) {
    dbg("remove_shaders for program %d\n",program);
    GLsizei max_count = 3; // thr!!
    GLuint  shaders[max_count];
    GLsizei count;
    int     i;

    glGetAttachedShaders(program,  max_count,  &count,  shaders);
    dbg("remove_shaders, found: %d in program: %d \n", count, program);
    for (i=0; i<count; ++i) {
        dbg("deleting shader %d, %d\n", i, shaders[i]);
        glDetachShader(program, shaders[i]);
        glDeleteShader(shaders[i]);
    }
}

void shader_good(GLuint program) {
    GLint  good = 0;
    GLchar err[1024];
    
    glGetProgramiv(program, GL_LINK_STATUS, &good);
    if (!good) {
        glGetProgramInfoLog(program, sizeof(err), NULL, err);
        errexit("Error linking shader program:\n'%s'\n", err);
    }

    glValidateProgram(program);
    glGetProgramiv(program, GL_VALIDATE_STATUS, &good);
    if (!good) {
        glGetProgramInfoLog(program, sizeof(err), NULL, err);
        (stderr, "Invalid shader program:\n'%s'\n", err);
        exit(1);
    }
}

void recompile_compute_shader( Cshdr *s, int assert_uniform ) {
    nfo("recompile_compute_shader\n");
    const char *src[2];

    src[0] = make_shared_def();
    src[1] = read_file(s->src_name);

    add_shader(s->program, 2, src, GL_COMPUTE_SHADER);
    glLinkProgram(s->program);
    shader_good(s->program);

    free((void*)src[0]);
    free((void*)src[1]);
}


void recompile_shaders( Shdr *r, int assert_uniform ) {
    nfo("recompile_shaders %s %s\n", r->vert_src_name, r->frag_src_name);
    remove_shaders(r->program);

    size_t srcc = 3;
    const char *vert_src[srcc];
    vert_src[0] = make_shared_def();
    vert_src[1] = read_file("header.vert");
    vert_src[2] = read_file(r->vert_src_name);

    add_shader(r->program, srcc, vert_src, GL_VERTEX_SHADER);

    const char *frag_src[srcc];
    frag_src[0] = make_shared_def();
    frag_src[1] = read_file("header.frag");
    frag_src[2] = read_file(r->frag_src_name);

    add_shader(r->program, srcc, frag_src , GL_FRAGMENT_SHADER);
    glLinkProgram(r->program);

    shader_good(r->program);

    r->w_         = block_offset(r->program, GL_UNIFORM, "_w");
    r->h_         = block_offset(r->program, GL_UNIFORM, "_h");
    r->elapsed_t_ = block_offset(r->program, GL_UNIFORM, "_elapsed_t");

    r->labsX_     = block_offset(r->program, GL_UNIFORM, "labsX");
    r->E_         = block_offset(r->program, GL_UNIFORM, "E");
    r->Ecoarse_   = block_offset(r->program, GL_UNIFORM, "Ecoarse");

    r->u_now_ = uniform_loc(r->program, "u_now", assert_uniform);
    r->u_prv_ = uniform_loc(r->program, "u_prv", assert_uniform);

    for (;srcc--;) {
        dbg("freeing shader source array %lu\n", srcc);
        free((void*)vert_src[srcc]);
        free((void*)frag_src[srcc]);
    }
}

void set_block_uniforms(Shdr *r) {
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    GLvoid *p = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    memcpy(p+r->w_, &_w, sizeof(_w));
    memcpy(p+r->h_, &_h, sizeof(_h));

    memcpy(p+r->elapsed_t_, &_elapsed_t, sizeof(_elapsed_t));

    memcpy(p+r->labsX_  , &labsX,   sizeof(labsX));
    memcpy(p+r->E_      , &E,       sizeof(E));
    memcpy(p+r->Ecoarse_, &Ecoarse, sizeof(Ecoarse));

//     memcpy(p+r->xxx_   &_xxx, sizeof(_xxx));

    glUnmapBuffer(GL_UNIFORM_BUFFER);
}

void set_global_uniforms(Shdr *r) {
    glUniform1i(r->w_, _w);
    glUniform1i(r->h_, _h);

    glUniform1i(r->elapsed_t_, _elapsed_t);

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

    size_t sze = 
        + sizeof(_w)
        + sizeof(_h)
        + sizeof(_elapsed_t)
        + sizeof(labsX)
        + sizeof(E)
        + sizeof(Ecoarse)
        ;

    glGenBuffers(1, &ubo);
    nfo("init_rect. init ubo %o, sze %lu \n", ubo, sze);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    // glBufferData both allocates and fills. the next line is for allocation. data is filled later
    glBufferData(GL_UNIFORM_BUFFER, sze, NULL, GL_STREAM_DRAW); // xxx GL_STATIC_DRAW appropriate?
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo);

    float tmp[10] = {5,5,5,5,5,5,5,5,5,5};
    sze = 10*sizeof(float);
    glGenBuffers(1, &ssbo);
    nfo("init_rect. init ssbo %o, sze %lu \n", ssbo, sze);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);

//     glBufferData(GL_SHADER_STORAGE_BUFFER, sze, tmp, GL_STATIC_READ); // xxx GL_STATIC_DRAW appropriate? // xxx need?
//     glBufferData(GL_SHADER_STORAGE_BUFFER, sze, tmp, GL_STREAM_READ); // xxx GL_STATIC_DRAW appropriate? // xxx need?
    glBufferData(GL_SHADER_STORAGE_BUFFER, sze, tmp, GL_DYNAMIC_READ); // xxx GL_STATIC_DRAW appropriate? // xxx need?
//     glBufferData(GL_SHADER_STORAGE_BUFFER, sze, NULL, GL_STREAM_COPY); // xxx GL_STATIC_DRAW appropriate? // xxx need?
//     glBufferData(GL_SHADER_STORAGE_BUFFER, sze, NULL, GL_DYNAMIC_DRAW); // xxx GL_STATIC_DRAW appropriate? // xxx need?
//     glBufferData(GL_SHADER_STORAGE_BUFFER, sze, NULL, GL_STREAM_DRAW); // xxx GL_STATIC_DRAW appropriate? // xxx need?

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo); // xxx need only once or before every shader invocation
}

void init_shdr( Shdr *r, const char *vsrc_name, const char *fsrc_name, int assert_uniform ){
    r->vert_src_name = vsrc_name;
    r->frag_src_name = fsrc_name;

    r->program = glCreateProgram();
    if (r->program == 0) errexit("Error creating shader program\n");

    recompile_shaders(r, assert_uniform);
}

void init_compute_shdr( Cshdr *s, const char *src_name, int assert_uniform ) {
    nfo("initializing compute shdr %s\n", src_name);
    s->src_name = src_name;

    s->program = glCreateProgram();
    if (s->program == 0) errexit("Error creating shader program\n");

    recompile_compute_shader(s, assert_uniform);
}

void compute(Cshdr *c) {
    glUseProgram(c->program);
    glDispatchCompute(1, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

//     float lala[10] = {1,2,3,4,5,6,7,8,9,0};
//     GLuint off = block_offset(c->program, GL_BUFFER_VARIABLE, "lala");
//     glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, off, sizeof(lala), lala);
// 
//     for (int i=10; i--;)
//         nfo("lala %d %f \n", i, lala[i]);

}

void inline setup_draw(Shdr *r) {
    glUseProgram(r->program);
//     set_global_uniforms(r);
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

