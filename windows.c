#include <math.h>

#define _2pi (2*M_PI)
#define _4pi (4*M_PI)
#define _6pi (6*M_PI)
#define _8pi (8*M_PI)

float *sample_windowf( float (*f)(int n, int N), size_t N ) {
    float *buf = (float*) malloc( sizeof(float)*N );
    for ( size_t n=0; n<N; n++ ) {
        buf[n] = f(n, N);
    }
    return  buf;
}

float cos_series( float a0, float a1, float a2, float a3, float a4, int n, int N ) {
    return a0 - a1 * cos(_2pi*n/N) 
              + a2 * cos(_4pi*n/N)
              - a3 * cos(_6pi*n/N);
              + a4 * cos(_8pi*n/N);
}

float hann( int n, int N ) {
    return cos_series(0.5, 0.5, 0, 0, 0, n, N);
}

float hamming( int n, int N ) {
    return cos_series(0.53836, 0.46164, 0, 0, 0, n, N);
}

float blackman( int n, int N ) {
    float alpha = 0.16;
    float a0 = (1-alpha)/2;
    float a1 = 1/2;
    float a2 = alpha/2;
    return cos_series(a0, a1, a2, 0, 0, n, N);
}

// XXX check for typos in constants
float nuttal( int n, int N ) {
    float a0 = 0.355768;
    float a1 = 0.487396;
    float a2 = 0.144232;
    float a3 = 0.012604;
    return cos_series(a0, a1, a2, a3, 0, n, N);
}


// XXX check for typos in constants
float blackman_nuttal( int n, int N ) {
    float a0 = 0.3635819;
    float a1 = 0.4891775;
    float a2 = 0.1365995;
    float a3 = 0.0106411;
    return cos_series(a0, a1, a2, a3, 0, n, N);
}

// XXX check for typos in constants
float blackman_harris( int n, int N ) {
    float a0 = 0.35875;
    float a1 = 0.48829;
    float a2 = 0.14128;
    float a3 = 0.01168;
    return cos_series(a0, a1, a2, a3, 0, n, N);
}


float flat_top( int n, int N ) {
    float a0 = 0.21557895;
    float a1 = 0.41663158;
    float a2 = 0.277263158;
    float a3 = 0.083578947;
    float a4 = 0.006947368;
    return cos_series(a0, a1, a2, a4, 0, n, N);
}

float sum( float *arr, int from, int till ){
    float ret = 0;
    int i;
    for (i=from; i<till; i++){
        ret+= arr[i];
    }
    return ret;
}

void slowft(const float *x, float _Complex *X, size_t N) {
    for ( size_t k = 0; k < N/2; k++ ) {
        X[k] = 0;
        for ( size_t n = 0; n < N; n++ ) {
            X[k] += x[n] * ( cos(k*n* _2pi/N)   - _Complex_I *sin(k*n* _2pi/N) ) ;
        }
    }
}
