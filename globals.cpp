#include "globals.h"

unsigned int _w = 1024;
unsigned int _h = 768;
 
unsigned int _elapsed_t = 0;
unsigned int _t0        = 0;
unsigned int _frame_t   = 0;

unsigned int _n_frames  = 0;

//alsa
snd_pcm_t *handle = {}; //thr ?


float* x[2] = { (float *) malloc( sizeof(float)*_buflen ),
                (float *) malloc( sizeof(float)*_buflen ) };

fftwf_complex X[_nfreq]; 

float E[_nbands];

float E_gesamt = 0;
float low      = 0;
float mid      = 0;
float hig      = 0;


fftwf_plan plan        = {}; //thr ?

GLuint framebuffer     = 0;
GLuint render_texture  = 0;
GLuint render_texture2 = 0;
GLuint render_texture3 = 0;

void init_texture(GLuint text, unsigned int w, unsigned int h){
    glBindTexture(GL_TEXTURE_2D, text);
    //// Give an empty image to OpenGL ( the last "0" )
    ////  we chose format GL_RGBA32F for we need good accuracy pde 
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
}

void setup_render_texture(GLuint text, unsigned int w, unsigned int h){
    init_texture(text, w, h);
    // Poor filtering. Needed !
    fprintf(stderr, "set up texture filtering\n");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}


char* readFile(const char *fn){
    fprintf(stderr, "reading file %s\n", fn);
    FILE *f     = fopen(fn, "rb");
    fseek(f, 0, SEEK_END);
    size_t flen = ftell(f);
    rewind(f);

    char *ret   = (char*) malloc( (flen+1)* sizeof(char) );
    fread(ret, sizeof(char), flen, f);
    ret[flen] = '\0';
    fclose(f);
    return ret;
}

void add_shader(GLuint shader_program, const char* pShaderText, GLenum ShaderType) {
    fprintf(stderr, "adding %d shader to program: %d \n", ShaderType, shader_program);

    const GLchar* p[1];
    GLint Lengths[1];

    GLuint ShaderObj = glCreateShader(ShaderType);
    if (ShaderObj == 0) {
        fprintf(stderr, "Error creating shader type %d\n", ShaderType); exit(0); }

    p[0]       = pShaderText;
    Lengths[0] = strlen(pShaderText);

    glShaderSource(ShaderObj, 1, p, Lengths);
    glCompileShader(ShaderObj);
    GLint success;
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d:\n%s\n", ShaderType, InfoLog);
        exit(1);
    }

    glAttachShader(shader_program, ShaderObj);
}

void remove_shaders(GLuint shader_program){
    fprintf(stderr, "remove_shaders for program %d\n",shader_program);
    GLsizei max_count = 3; // thr!!
    GLuint  shaders[max_count];
    GLsizei count;
    int     i;

    glGetAttachedShaders(shader_program,  max_count,  &count,  shaders);
    fprintf(stderr, "remove_shaders, found: %d in program: %d \n", count, shader_program);
    for (i=0; i<count; ++i){
        fprintf(stderr, "deleting shader %d, %d\n", i, shaders[i]);
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

