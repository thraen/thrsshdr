#include "globals.h"

unsigned int _w = 1024;
unsigned int _h = 768;
 
unsigned int _elapsed_t   = 0;
unsigned int _t0          = 0;
unsigned int _render_t    = 0;
unsigned int _soundproc_t = 0;

//pulse audio
pa_simple *pa_source = NULL;

const pa_sample_spec _pa_sspec = {
    .format = PA_SAMPLE_FLOAT32LE,
    .rate = 48000,
    .channels = 1
};

const pa_buffer_attr _pa_bufattr = {
    .maxlength = (uint32_t) -1,
    .tlength   = (uint32_t) -1,
    .prebuf    = (uint32_t) -1,
    .minreq    = (uint32_t) -1,
    .fragsize  = 200 // xxx very probably too low, other hand: put to default, we sometimes wait long
};

float x[_N];

float _Complex X[_nfreq];

float absX[_nfreq];
float max_absX[_nfreq];
float labsX[_nfreq];
float max_labsX[_nfreq];

float E[_nband];
float E_max[_nband];

float Ecoarse[3];
float max_Ecoarse[3];

fftwf_plan plan;

GLuint framebuffer     = 0;
GLuint render_texture  = 0;
GLuint render_texture2 = 0;
GLuint render_texture3 = 0;

void tdiff(struct timespec *t, struct timespec *s, struct timespec *dt) {
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
