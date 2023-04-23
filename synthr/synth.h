#include <stdio.h>
#include <math.h>

#include "portaudio.h"

#include "ui.h"

/// fuck is it?
#define SAMPLE_RATE   (44100)
/// fuck ?
#define FRAMES_PER_BUFFER  (64)

#define TABLE_SIZE   (200)

float sine_[TABLE_SIZE];

static
void init_samples() {
    for( int i=0; i<TABLE_SIZE; i++ ) {
        sine_[i] = (float) sin( ((double)i/(double)TABLE_SIZE) * M_PI * 2. );
    }
}

static
int patestCallback( const void *buf_in, void *buf_out,
                    unsigned long n_frames, // frames per buffer
                    const PaStreamCallbackTimeInfo *time_info,
                    PaStreamCallbackFlags status_flags,
                    void *user_data )
{
    // Prevent unused variable warnings.
    (void) time_info; (void) status_flags; (void) buf_in;

    Ui_State *state = (Ui_State*) user_data;

    static int left_phase = 0;
    static int right_phase = 0;

    float *out = (float*)buf_out;

    for( int i = 0; i < n_frames; i++ )
    {
        *out++ = state->sample[left_phase];
        *out++ = state->sample[right_phase];
        left_phase += 1;
        if( left_phase >= TABLE_SIZE ) left_phase -= TABLE_SIZE;
        // higher pitch so we can distinguish left and right.
        right_phase += 3;
        if( right_phase >= TABLE_SIZE ) right_phase -= TABLE_SIZE;
    }

    return paContinue;
}

static
void stream_finished( void *user_data )
{
//     paTestData *data = (paTestData *) user_data;
    printf( "stream finished callback \n");
}
