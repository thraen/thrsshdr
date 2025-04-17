#include <math.h>
#include <complex.h>
#include <unistd.h>

#define _2pi (2*M_PI)
#define _4pi (4*M_PI)
#define _6pi (6*M_PI)
#define _8pi (8*M_PI)

static
void apply_window_on_ringbuffer( float *wsamp, float *x, float *out, int s, int N )
{
    for ( int i=0; i<N; i++ ) {
        out[i] = x[ (s+i)%N ] * wsamp[i];
    }
}

static
void sample_windowf( float (*f)(int n, int N), float *buf, size_t N )
{
    for ( size_t n=0; n<N; n++ ) {
        buf[n] = f(n, N);
    }
}

static
float cos_series( float a0, float a1, float a2, float a3, float a4, int n, int N ) {
    return a0 - a1 * cos(_2pi*n/N) 
              + a2 * cos(_4pi*n/N)
              - a3 * cos(_6pi*n/N)
              + a4 * cos(_8pi*n/N);
}

static
float hann( int n, int N ) {
    return cos_series(0.5, 0.5, 0, 0, 0, n, N);
}

static
float hamming( int n, int N ) {
    return cos_series(0.53836, 0.46164, 0, 0, 0, n, N);
}

static
float blackman( int n, int N ) {
    float alpha = 0.16;
    float a0 = 0.5*(1-alpha);
    float a1 = 0.5;
    float a2 = 0.5*alpha;
    return cos_series(a0, a1, a2, 0, 0, n, N);
}

// XXX check for typos in constants
static
float nuttal( int n, int N ) {
    float a0 = 0.355768;
    float a1 = 0.487396;
    float a2 = 0.144232;
    float a3 = 0.012604;
    return cos_series(a0, a1, a2, a3, 0, n, N);
}


// XXX check for typos in constants
static
float blackman_nuttal( int n, int N ) {
    float a0 = 0.3635819;
    float a1 = 0.4891775;
    float a2 = 0.1365995;
    float a3 = 0.0106411;
    return cos_series(a0, a1, a2, a3, 0, n, N);
}

// XXX check for typos in constants
static
float blackman_harris( int n, int N ) {
    float a0 = 0.35875;
    float a1 = 0.48829;
    float a2 = 0.14128;
    float a3 = 0.01168;
    return cos_series(a0, a1, a2, a3, 0, n, N);
}


static
float flat_top( int n, int N ) {
    float a0 = 0.21557895;
    float a1 = 0.41663158;
    float a2 = 0.277263158;
    float a3 = 0.083578947;
    float a4 = 0.006947368;
    return cos_series(a0, a1, a2, a3, a4, n, N);
}

static
void slowft(const float *x, float _Complex *X, int N) {
    for ( int k = 0; k < N/2; k++ ) {
        X[k] = 0;
        for ( int n = 0; n < N; n++ ) {
            X[k] += x[n] * ( cos(k*n* _2pi/N)   - _Complex_I *sin(k*n* _2pi/N) ) ;
        }
    }
}
