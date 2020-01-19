#ifndef GLOBALS_H
#define GLOBALS_H

#include <pulse/simple.h>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <math.h>

#include <complex.h>
#include <fftw3.h>

#define _max(a,b) ((a) > (b) ? a : b)
#define _min(a,b) ((a) < (b) ? a : b)

#define DEBUG 0
#define NFO   1

#define dbg(...) do { if (DEBUG) fprintf(stderr, __VA_ARGS__); } while (0)
#define nfo(...) do { if (NFO)   fprintf(stderr, __VA_ARGS__); } while (0)

#define errexit(...) {fprintf(stderr, __VA_ARGS__); exit(1);}

#define __init_timer()  timespec ___t0, ___T; long ___tdiff; long ___ravg = 0;
#define __start_timer() clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &___t0);
#define __stop_timer()  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &___T); \
                        ___tdiff =  (___T.tv_sec  - ___t0.tv_sec) * (long)1e9 + (___T.tv_nsec - ___t0.tv_nsec) ; \
                        ___ravg  = 0.01 * (___ravg * (100-1) + ___tdiff) ; \
                        fprintf( stderr, "it took %12lu nanoseconds, %12lu on average.\n", \
								 ___tdiff, ___ravg )
                     
// const int _N = 16384
const int _N = 8192;
// const int _N = 4096
// const int _N = 2048
// const int _N = 1024
// const int _N = 512

const int _buflen = 64; // _N must be divisible by _buflen or we segfault
// const int _buflen = 128 // _N must be divisible by _buflen or we segfault
// const int _buflen = 256 // _N must be divisible by _buflen or we segfault

const int _nfreq = _N/2 +1;

const int _nband = (int) (log(_nfreq) / log(2));

extern unsigned int _frame_t;
extern unsigned int _elapsed_t; 
extern unsigned int _t0; 
extern unsigned int _n_frames; 

extern unsigned int _w;
extern unsigned int _h;

//pulse audio
extern pa_simple *pa_source;

const pa_sample_spec _pa_sspec = { .format = PA_SAMPLE_FLOAT32LE, .rate = 48000, .channels = 1 };

const pa_buffer_attr _pa_bufattr = {
    .maxlength = (uint32_t) -1,
    .tlength   = (uint32_t) -1,
    .prebuf    = (uint32_t) -1,
    .minreq    = (uint32_t) -1,
    .fragsize  = 160 // xxx
};

extern float *x;  // two channels of real data
//fft
extern fftwf_plan plan;

extern float _Complex X[_nfreq];

extern float absX[_nfreq];
extern float max_absX[_nfreq];
extern float labsX[_nfreq];
extern float max_labsX[_nfreq];

// predefined energy bands
extern float E[_nband];
extern float E_max[_nband];

const float _Escale = 200;

// more condensed energy bands: low mid hid
extern float Ecoarse[3];
extern float max_Ecoarse[3];
const int    _lowbound = 3;
const int    _midbound = 5;
const int    _higbound = _nband;

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

void  add_shader(GLuint shader_program, size_t srcc, const char **srcv, GLenum shader_type);

void  remove_shaders(GLuint shader_program);

GLuint uniform_loc(GLuint shader_program, const char* s, bool assert_uniform);

void print_equalizer(const float *E, const float *E_max, size_t n, size_t maxlen);

#endif //GLOBALS_H
