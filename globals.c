#include "globals.h"

unsigned int _w = 1024;
unsigned int _h = 768;
 
struct timespec _render_t;
struct timespec _tr;
struct timespec _soundproc_t;
struct timespec _ts;

struct timespec _t;

unsigned int _elapsed_t;

float x[_N];

float _Complex X[_nfreq];

float absX[_nfreq];
float max_absX[_nfreq];
float labsX[_nfreq];
float max_labsX[_nfreq];

float schwerpunkt = 0.0f;

float E[_nband];
float E_max[_nband];

float Ecoarse[3];
float max_Ecoarse[3];

int charstream[50] = {0};

fftwf_plan plan; /// xxx should be static in main

GLuint framebuffer     = 0;
GLuint render_texture  = 0;
GLuint render_texture2 = 0;
GLuint render_texture3 = 0;

void tdiff(const struct timespec *s, const struct timespec *t, struct timespec *dt) {
    long dns = s->tv_nsec - t->tv_nsec;
    if (dns < 0) {
        dt->tv_sec  = s->tv_sec  - t->tv_sec  - 1;
        dt->tv_nsec = dns + 1E9;
    } 
    else {
        dt->tv_sec  = s->tv_sec  - t->tv_sec;
        dt->tv_nsec = dns;
    }
}

// extern inline long millis(const struct timespec t);
// extern inline long micros(const struct timespec t);
