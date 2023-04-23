#include <stdio.h>
#include <math.h>
#include "portaudio.h"

/// fuck is it?
#define SAMPLE_RATE   (44100)
/// fuck ?
#define FRAMES_PER_BUFFER  (64)

#define TABLE_SIZE   (200)

typedef struct
{
    float sine[TABLE_SIZE];
    int left_phase;
    int right_phase;
} paTestData;

static
int patestCallback( const void *buf_in, void *buf_out,
                    unsigned long n_frames, // frames per buffer
                    const PaStreamCallbackTimeInfo *time_info,
                    PaStreamCallbackFlags status_flags,
                    void *user_data )
{
    // Prevent unused variable warnings.
    (void) time_info; (void) status_flags; (void) buf_in;

    paTestData *data = (paTestData*) user_data;
    float *out = (float*)buf_out;
    unsigned long i;

    for( i=0; i < n_frames; i++ )
    {
        *out++ = data->sine[data->left_phase];
        *out++ = data->sine[data->right_phase];
        data->left_phase += 1;
        if( data->left_phase >= TABLE_SIZE ) data->left_phase -= TABLE_SIZE;
        // higher pitch so we can distinguish left and right.
        data->right_phase += 3;
        if( data->right_phase >= TABLE_SIZE ) data->right_phase -= TABLE_SIZE;
    }

    return paContinue;
}

static
void stream_finished( void *user_data )
{
//     paTestData *data = (paTestData *) user_data;
    printf( "stream finished callback \n");
}
