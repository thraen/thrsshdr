#include "globals.h"

#include <string.h>

unsigned int _w = 1024;
unsigned int _h = 768;
 
unsigned int _elapsed_t = 0;
unsigned int _t0        = 0;
unsigned int _frame_t   = 0;
unsigned int _n_frames  = 0;

//pulse audio
pa_simple *pa_source = NULL;

float *x = (float *) malloc( sizeof(float)*_N );

float _Complex X[_nfreq];

float normX[_nfreq];
float nXmax[_nfreq];

float E[_nband];
float E_max[_nband];

float low      = 0;
float mid      = 0;
float hig      = 0;

fftwf_plan plan;

GLuint framebuffer     = 0;
GLuint render_texture  = 0;
GLuint render_texture2 = 0;
GLuint render_texture3 = 0;

void init_texture(GLuint text, unsigned int w, unsigned int h) {
    glBindTexture(GL_TEXTURE_2D, text);
////glTexImage2D(target,        level, internalformat, w, height, border, format,  type,     void * data);
    glTexImage2D(GL_TEXTURE_2D, 0,     GL_RGBA32F,     w, h,      0,      GL_RGBA, GL_INT, NULL);
//     glTexImage2D(GL_TEXTURE_2D, 0,     GL_RGBA32F,     w, h,      0,      GL_RGBA, GL_FLOAT, NULL);
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

GLuint uniform_loc( GLuint shader_program, const char* s, bool strict ) {
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

float sum( float *arr, int from, int till ){
    float ret = 0;
    int i;
    for (i=from; i<till; i++){
        ret+= arr[i];
    }
    return ret;
}

void print_bars(const float *E, const float *E_max, size_t n, size_t maxlen) {
    char s[maxlen+1];
    s[maxlen] = '\0';

    fprintf(stderr, "%d %d %d                \n", _lowbound, _midbound, _higbound);
    float sc = 5;

    for (int i=0; i<n; i++) {
        float logE = sc*log(E[i] + 1);
        int len = _max( _min( logE, maxlen ), 0 );

        memset(s, '*', len);
        memset(s+len, ' ', maxlen-len);

        int m = sc*log(E_max[i]+1);
        m = _max( _min( m, maxlen ), 0 );
        s[m]  = '|';

        fprintf(stderr, "%d %6.3f %s\n", i, logE, s);
    }
    fprintf(stderr, "\x1b[%luA", n+1);
}


void make_bands(float *E, size_t nbands, size_t *idxs, size_t nidxs) {

}

// XXX use or remove
// void log_indices(size_t* idxs, size_t max_idx, size_t k) {
void log_indices(size_t max_idx, size_t k) {
    size_t idxs[_nband];

    int i=0, j=0;
    while (j<max_idx) {
        idxs[i] = j;
        j*=2; i++;
        printf("%d", j);
    }
//     return idxs;
}

