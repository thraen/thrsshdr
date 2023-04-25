#include <string.h>

#include "globals.h"
#include "glcrap.h"

static GLuint vbo;
static GLuint vao;
static GLuint ubo;
static GLuint ssbo;

// offsets within a block of respective uniforms that are shared by all shaders
// the layout of said block is defined to be 'shared', so offsets are guaranteed 
// to be the same in all shaders declaring it. 
static GLuint w_;
static GLuint h_;
static GLuint elapsed_t_;
static GLuint labsX_;
static GLuint E_;
static GLuint Ecoarse_;


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
    if (ext) exit(1);
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
        "   };                                         \n"
        "   layout(shared, binding=0) buffer FUUK {    \n"
        "       float lastf[_nfreq];                   \n"
        "       float difff[_nfreq];                   \n"
        "       float sumf[_nfreq];                    \n"
        "       float smoothf[_nfreq];                 \n"
        "       float maxf[_nfreq];                    \n"
        "   };                                         \n";

    char *shared_defs = (char *) malloc(10000);
    snprintf( shared_defs, 10000, tmp, _nfreq, _nband );
    dbg("%s", tmp);
    return shared_defs;
}

void init_texture(GLuint text, unsigned int w, unsigned int h) {
    glBindTexture(GL_TEXTURE_2D, text);
    ////         target,        level, internalformat, w, height, border, format,  type,   void * data
    glTexImage2D(GL_TEXTURE_2D, 0,     GL_RGBA32F,     w, h,      0,      GL_RGBA, GL_INT, NULL); 
    //// XXX setting type == GL_INT should imply not clamping color values. It still does clamp :(
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

static
char* read_file(const char *fn) {
    dbg("read_file(%s)\n", fn);
    FILE *f= fopen(fn, "rb");
        if(!f) perr_exit(fn);
    fseek(f, 0, SEEK_END);
    size_t flen = ftell(f);
    rewind(f);

    char *ret = (char*) malloc( (flen+1)* sizeof(char) );
    if ( flen != fread(ret, sizeof(char), flen, f) )
        err_exit("error reading file %s\n", fn);

    ret[flen] = '\0';
    fclose(f);
    return ret;
}

static
void add_shader( GLuint program, size_t srcc, const char **srcv, GLenum shader_type ) {
    dbg("attaching %lu shader sources of type %d to program: %d \n", srcc, shader_type, program);

    GLuint shader = glCreateShader(shader_type);

    if (shader == 0)
        err_exit("Error creating shader type %d\n", shader_type);

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
        err_exit("Error compiling shader type %d:\n%s\n", shader_type, err);
    }

    glAttachShader(program, shader);
}

static
GLuint block_offset( GLuint program, GLuint interface_type, const char* uniform_name ) {
    GLuint ind = glGetProgramResourceIndex(program, interface_type, uniform_name);
    const GLenum query_properties[1] = { GL_OFFSET };
    GLint props[1];
    glGetProgramResourceiv(program, interface_type, ind, 1, query_properties, 1, NULL, props);
    dbg("block_offset(%u, %s) determined offset %d\n", program, uniform_name, props[0]);
    return props[0];
}

static
GLuint uniform_loc( GLuint program, const char* uniform_name, int strict ) {
    GLuint ret = glGetUniformLocation(program, uniform_name);
    if (strict && ret == 0xFFFFFFFF && uniform_name)
        err_exit("Uniform  %s  not active!", uniform_name);

    return ret;
}

static
void remove_shaders( GLuint program ) {
    dbg("remove_shaders for program %d\n",program);
    GLsizei max_count = 3; // thr!!
    GLuint  shaders[max_count];
    GLsizei count;

    glGetAttachedShaders(program,  max_count,  &count,  shaders);
    dbg("remove_shaders, found: %d in program: %d \n", count, program);
    for (int i=0; i<count; ++i) {
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
        err_exit("Error linking shader program:\n'%s'\n", err);
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
    remove_shaders(s->program);
    const char *src[2];

    src[0] = make_shared_def();
    src[1] = read_file(s->src_name);

    add_shader(s->program, 2, src, GL_COMPUTE_SHADER);
    glLinkProgram(s->program);
    shader_good(s->program);

    free((void*)src[0]);
    free((void*)src[1]);
}

// since these uniforms are within a "layout = shared"-block it doesn't matter 
// for which of the shaders we call that function
void init_shared_uniforms(GLuint program) {
    w_         = block_offset(program, GL_UNIFORM, "_w");
    h_         = block_offset(program, GL_UNIFORM, "_h");
    elapsed_t_ = block_offset(program, GL_UNIFORM, "_elapsed_t");
    labsX_     = block_offset(program, GL_UNIFORM, "labsX");
    E_         = block_offset(program, GL_UNIFORM, "E");
    Ecoarse_   = block_offset(program, GL_UNIFORM, "Ecoarse");
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

    r->u_now_ = uniform_loc(r->program, "u_now", assert_uniform);
    r->u_prv_ = uniform_loc(r->program, "u_prv", assert_uniform);

    for (;srcc--;) {
        dbg("freeing shader source array %lu\n", srcc);
        free((void*)vert_src[srcc]);
        free((void*)frag_src[srcc]);
    }
}

void set_block_uniforms() {
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    GLvoid *p = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    memcpy(p+w_, &_w, sizeof(_w));
    memcpy(p+h_, &_h, sizeof(_h));

    memcpy(p+elapsed_t_, &_elapsed_t, sizeof(_elapsed_t));

    memcpy(p+labsX_  , &labsX,   sizeof(labsX));
    memcpy(p+E_      , &E,       sizeof(E));
    memcpy(p+Ecoarse_, &Ecoarse, sizeof(Ecoarse));

//     memcpy(p+xxx_   &_xxx, sizeof(_xxx));

    glUnmapBuffer(GL_UNIFORM_BUFFER);
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
    nfo("init_rect() init ubo %o, sze %lu \n", ubo, sze);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    // glBufferData both allocates and fills. the next line is for allocation. data is filled later
    glBufferData(GL_UNIFORM_BUFFER, sze, NULL, GL_STREAM_DRAW); // xxx GL_STATIC_DRAW appropriate?
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo);

    sze = 5*_nfreq*sizeof(float); // lastf + difff + sumf + smoothf + maxf
    char *zerodat[sze]; memset(zerodat, 0, sze);
    glGenBuffers(1, &ssbo);
    nfo("init_rect() init ssbo %o, sze %lu \n", ssbo, sze);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);

//     glBufferData(GL_SHADER_STORAGE_BUFFER, sze, NULL, GL_STATIC_READ); // xxx GL_STATIC_DRAW appropriate? // xxx need?
//     glBufferData(GL_SHADER_STORAGE_BUFFER, sze, NULL, GL_STREAM_READ); // xxx GL_STATIC_DRAW appropriate? // xxx need?
//     glBufferData(GL_SHADER_STORAGE_BUFFER, sze, NULL, GL_DYNAMIC_READ); // xxx GL_STATIC_DRAW appropriate? // xxx need?
    glBufferData(GL_SHADER_STORAGE_BUFFER, sze, zerodat, GL_STREAM_COPY); // xxx GL_STATIC_DRAW appropriate? // xxx need?
//     glBufferData(GL_SHADER_STORAGE_BUFFER, sze, NULL, GL_DYNAMIC_DRAW); // xxx GL_STATIC_DRAW appropriate? // xxx need?
//     glBufferData(GL_SHADER_STORAGE_BUFFER, sze, NULL, GL_STREAM_DRAW); // xxx GL_STATIC_DRAW appropriate? // xxx need?

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo); // xxx need only once or before every shader invocation
}

void init_shdr( Shdr *r, const char *vsrc_name, const char *fsrc_name, int assert_uniform ){
    r->vert_src_name = vsrc_name;
    r->frag_src_name = fsrc_name;

    r->program = glCreateProgram();
    if (r->program == 0) err_exit("Error creating shader program\n");

    recompile_shaders(r, assert_uniform);
}

void init_compute_shdr( Cshdr *s, const char *src_name, int assert_uniform ) {
    nfo("initializing compute shdr %s\n", src_name);
    s->src_name = src_name;

    s->program = glCreateProgram();
    if (s->program == 0) err_exit("Error creating shader program\n");

    recompile_compute_shader(s, assert_uniform);
}

void compute(Cshdr *c) {
    glUseProgram(c->program);
    glDispatchCompute(_nfreq, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

//     float difff[_nfreq];
//     GLuint off = block_offset(c->program, GL_BUFFER_VARIABLE, "difff");
//     glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, off, sizeof(difff), difff);
//     nfo("difff %d %f \n", 1, difff[1]);
//     for (int i=_nfreq; i--;)
//         nfo("difff %d %f \n", i, difff[i]);

}

static inline
void setup_draw(Shdr *r) {
    glUseProgram(r->program);
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

