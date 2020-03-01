#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdlib.h>

#include <pulse/simple.h>

#include <GL/glew.h>

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


#include <time.h>
#define __init_timer()  struct timespec ___t0, ___T; long ___tdiff; long ___ravg = 0;
#define __start_timer() clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &___t0);
#define __stop_timer()  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &___T); \
                        ___tdiff =  (___T.tv_sec  - ___t0.tv_sec) * (long)1e9 + (___T.tv_nsec - ___t0.tv_nsec) ; \
                        ___ravg  = 0.01 * (___ravg * (100-1) + ___tdiff) ; \
                        fprintf( stderr, "it took %12lu nanoseconds, %12lu on average.\n", \
								 ___tdiff, ___ravg )


#define SYNCHRONOUS 1
                     
#define bits_N   13                   // -> _N 8192
#define _N      (1<<bits_N)
#define _buflen (1<<9) // 512
#define _nfreq  ((1<<(bits_N-1))+1)
#define _nband  (bits_N-1)  // actually a few high frequencies are missing, but we don't care.

#define _Escale 200.0f

extern struct timespec _render_t;
extern struct timespec _tr; 
extern struct timespec _soundproc_t;
extern struct timespec _ts; 

extern struct timespec _t; 
unsigned int _elapsed_t;

extern unsigned int _w;
extern unsigned int _h;

//pulse audio
extern pa_simple *pa_source;

extern const pa_sample_spec _pa_sspec;
extern const pa_buffer_attr _pa_bufattr;

float x[_N];
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


// more condensed energy bands: low mid hid
extern float Ecoarse[3];
extern float max_Ecoarse[3];

#define _lowbound 3
#define _midbound 5
#define _higbound _nband

extern GLuint framebuffer;
extern GLuint render_texture;
extern GLuint render_texture2;
extern GLuint render_texture3;

void tdiff(const struct timespec *s, const struct timespec *t, struct timespec *dt);

inline unsigned int millis(const struct timespec t) {
    return t.tv_sec * 1E3 + t.tv_nsec / 1E6;
}

inline unsigned int micros(const struct timespec t) {
    return t.tv_sec * 1E6 + t.tv_nsec / 1E3;
}

#endif //GLOBALS_H
