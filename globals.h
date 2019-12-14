#ifndef GLOBALS_H
#define GLOBALS_H

#include <alsa/asoundlib.h>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <fftw3.h>
#include <math.h>

#define _max(a,b) ((a) > (b) ? a : b)
#define _min(a,b) ((a) < (b) ? a : b)


//#define _N 8192
// #define _N 4096
// #define _N 2048
#define _N 1024
// #define _N 512

#define _buflen 256 // _N must be divisible by _buflen or we segfault

static constexpr int _nfreq = _N/2 +1;

static constexpr int _nband = (int) (log(_nfreq) / log(2));

static constexpr int _lowbound = 2;
static constexpr int _midbound = 5;
static constexpr int _higbound = _nband;

extern unsigned int _frame_t;
extern unsigned int _elapsed_t; 
extern unsigned int _t0; 
extern unsigned int _n_frames; 

extern unsigned int _w;
extern unsigned int _h;

//alsa
extern snd_pcm_t *handle;

extern float *x[2];  // two channels of real data
//fft
extern fftwf_plan plan;

extern fftwf_complex X[_nfreq];

extern float normX[_nfreq];
extern float nXmax[_nfreq];

//predefined energy bands
extern float E[_nband];
extern float E_max[_nband];

extern float low;
extern float mid;
extern float hig;

extern GLuint framebuffer;
extern GLuint render_texture;
extern GLuint render_texture2;
extern GLuint render_texture3;

float sum(float *arr, int from, int till);

// opengl
void  init_texture(GLuint text, unsigned int w, unsigned int h);

void  setup_render_texture(GLuint text, unsigned int w, unsigned int h);

void  resize_render_texture(GLuint render_texture, int w, int h);

char* read_file(const char *fn);

void  add_shader(GLuint shader_program, const char **srcv, GLenum shader_type);

void  remove_shaders(GLuint shader_program);

GLuint uniform_loc(GLuint shader_program, const char* s, bool assert_uniform);

// terminal analyzer
void print_bars(const float *E, const float *E_max, size_t n, size_t maxlen);

#endif //GLOBALS_H
