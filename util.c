#include <string.h>
#include <stdio.h>
#include <math.h>

#include "globals.h"

static
float sum( float *arr, int from, int till ){
    float ret = 0;
    int i;
    for (i=from; i<till; i++){
        ret+= arr[i];
    }
    return ret;
}


/// from nfreq complex frequencies X
/// calculate nfreq energies, i.e. absolute values of X
/// as well as logarithm of the absolute values and max of each
// static
// void process_freqs( const float _Complex *X, int nfreq,
//                     float **absX, float **labsX, 
//                     float **max_absX, float **max_labsX )
void process_freqs( const float _Complex *X, int nfreq,
                    float *absX, float *labsX, 
                    float *max_absX, float *max_labsX )
{
    for (int j=0; j<nfreq; j++) {
        absX[j] = cabsf(X[j]) / _nfreq;
//         absX[j]      = cabsf(X[j]);
        max_absX[j]  = fmax( absX[j], max_absX[j] );

        labsX[j]     = log(1+absX[j]);
        max_labsX[j] = fmax( labsX[j], max_labsX[j] );

//         *absX[j]      = cabsf(X[j]);
//         *max_absX[j]  = fmax( *absX[j], *max_absX[j] );
//         *labsX[j]     = log(1+*absX[j]);
//         *max_labsX[j] = fmax( *labsX[j], *max_labsX[j] );
    }
}

/// Group frequencies into logarithmic bands
/// the last frequency is not part of any band, we don't care
static 
void gather_bands( int nfreq, float *Xs,
                   int nband, float *E, float *E_max, 
                   float Ecoarse[3], float max_Ecoarse[3] ) 
{
    for (int k=0; k < nband; k++ ){ 
        E[k] = 0;
        for ( int j = pow(2,k)-1; j < pow(2,k+1)-1; j++ ) {
            E[k] += Xs[j];
        }
        E[k] *= (_Escale/( pow(2,k+1) - pow(2,k) ));

        E_max[k] = fmax(E[k], E_max[k]);
    }

    Ecoarse[0] = sum(E, 0          , _lowbound) / (float) _lowbound;
    Ecoarse[1] = sum(E, _lowbound+1, _midbound) / (float) (_midbound-_lowbound);
    Ecoarse[2] = sum(E, _midbound+1, _higbound) / (float)(_higbound-_midbound);
    
    max_Ecoarse[0] = _max(Ecoarse[0], max_Ecoarse[0]);
    max_Ecoarse[1] = _max(Ecoarse[1], max_Ecoarse[1]);
    max_Ecoarse[2] = _max(Ecoarse[2], max_Ecoarse[2]);
}

static
void print_equalizer( const float *E, const float *E_max, size_t n, size_t maxlen ) {
    char s[maxlen+1];
    s[maxlen] = '\0';

    for (int i=0; i<n; i++) {
        int len = fmax( fmin( E[i], (maxlen-1) ), 0 );

        memset(s, '*', len);
        memset(s+len, ' ', maxlen-len);

        int m = fmax( fmin( E_max[i], (maxlen-1) ), 0 );
        s[m]  = '|';

        fprintf(stderr, "%3d %7.3f %7.3f %s\n", i, E_max[i], E[i], s);
    }
    fprintf(stderr, "\x1b[%luA", n); // move cursor up
}

