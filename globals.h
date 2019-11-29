#ifndef GLOBALS_H
#define GLOBALS_H

#define _max(a,b) ((a) > (b) ? a : b)
#define _min(a,b) ((a) < (b) ? a : b)

#include <alsa/asoundlib.h>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <fftw3.h>
#include <math.h>

//#define _buflen 8192
// #define _buflen 4096
// #define _buflen 2048
#define _buflen 1024
// #define _buflen 512

static constexpr int _nfreq = _buflen/2 +1;

static constexpr int _nband = (int) log(_nfreq);

// static constexpr int 34;
// static constexpr int _lowbound = _nband/34;
// static constexpr int _midbound = _lowbound + 2*_nband/4;
// static constexpr int _higbound = _nband;

static constexpr int _lowbound = 1;
static constexpr int _midbound = 4;
static constexpr int _higbound = _nband;

extern unsigned int _frame_t;
extern unsigned int _elapsed_t; 
extern unsigned int _t0; 
extern unsigned int _n_frames; 

extern unsigned int _w;
extern unsigned int _h;

//alsa
extern snd_pcm_t *handle;

// extern double *x[2];  // two channels of real data
extern float *x[2];  // two channels of real data

//fft
extern fftwf_plan plan;

extern fftwf_complex X[_nfreq];

extern float normX[_nfreq];

//predefined energy bands
extern float E[_nband];
extern float E_max[_nband];

extern float E_gesamt;

extern float low;
extern float mid;
extern float hig;

extern GLuint framebuffer;
extern GLuint render_texture;
extern GLuint render_texture2;
extern GLuint render_texture3;

float sum(float *arr, int from, int till);

void  init_texture(GLuint text, unsigned int w, unsigned int h);

void  setup_render_texture(GLuint text, unsigned int w, unsigned int h);

void  resize_render_texture(GLuint render_texture, int w, int h);

char* readFile(const char *fn);

void  add_shader(GLuint shader_program, const char* pShaderText, GLenum ShaderType);

void  remove_shaders(GLuint shader_program);

GLuint uniform_loc(GLuint shader_program, const char* s, bool assert_uniform);

#endif //GLOBALS_H
