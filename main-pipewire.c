/*
   * use pipewire stream api to capture audio
   * fourier transform the data
   * make the fft available to a fragment shader
   * .. and draw it
   Pipewire part is taken from https://docs.pipewire.org/audio-capture_8c-example.html
   Copyright © 2022 Wim Taymans
 */

#include <stdio.h>
#include <math.h>
#include <signal.h>

#include <complex.h> /// must be included before fftw so that fftw uses _Complex
#include <fftw3.h>

#include <spa/param/audio/format-utils.h>
#include <pipewire/pipewire.h>

#include "windows.c"

#include "globals.h"

struct mydata {
    struct pw_main_loop *loop;
    struct pw_stream *stream;

    struct spa_audio_info format;
};

struct data { /// xxx remove
    struct pw_main_loop *loop;
    struct pw_stream *stream;

    struct spa_audio_info format;
    unsigned move:1;
};

void gather() {
    // this is wrong!! we loose frequencies. see definition of _nband
    for ( int k=0; k<_nband; k++ ){ 
        E[k] = 0;
        // xxx use sum and indices
        for ( int j = pow(2,k)-1; j < pow(2,k+1)-1; j++ ) {
            absX[j] = cabsf(X[j]) / _nfreq;
            max_absX[j] = _max( absX[j], max_absX[j] );

            labsX[j]     = log(1+absX[j]);
            max_labsX[j] = _max( log(1+absX[j]), max_labsX[j] );

            E[k] += labsX[j];
        }
        E[k] *= (_Escale/( pow(2,k+1) - pow(2,k) ));

        E_max[k] = _max(E[k], E_max[k]);
    }

    Ecoarse[0] = sum(E, 0          , _lowbound) / _lowbound;
    Ecoarse[1] = sum(E, _lowbound+1, _midbound) / (_midbound-_lowbound);
    Ecoarse[2] = sum(E, _midbound+1, _higbound) / (_higbound-_midbound);
    
    max_Ecoarse[0] = _max(Ecoarse[0], max_Ecoarse[0]);
    max_Ecoarse[1] = _max(Ecoarse[1], max_Ecoarse[1]);
    max_Ecoarse[2] = _max(Ecoarse[2], max_Ecoarse[2]);
}

void print_equalizer( const float *E, const float *E_max, size_t n, size_t maxlen ) {
    char s[maxlen+1];
    s[maxlen] = '\0';

    for (int i=0; i<n; i++) {
        int len = _max( _min( E[i], (maxlen-1) ), 0 );

        memset(s, '*', len);
        memset(s+len, ' ', maxlen-len);

        int m = _max( _min( E_max[i], (maxlen-1) ), 0 );
        s[m]  = '|';

        fprintf(stderr, "%3d %7.3f %7.3f %s\n", i, E_max[i], E[i], s);
    }
    fprintf(stderr, "\x1b[%luA", n); // move cursor up
}

void apply_window( float *wsamp, float *x, float *out, size_t s, size_t N ) {
    for ( int i=0; i<N; i++ ) {
        out[i] = x[ (i-s)%N ] * wsamp[i];
    }
}

int s = 0;

static
void _process(void *userdata)
{
    struct mydata *data = userdata;

    struct pw_buffer *b;
    struct spa_buffer *buf;
    if ((b = pw_stream_dequeue_buffer(data->stream)) == NULL) {
        pw_log_warn("out of buffers: %m");
        return;
    }

    float *samples, max;
    buf = b->buffer;
    if ((samples = buf->datas[0].data) == NULL)
        return;

    int n_channels = data->format.info.raw.channels;
    int n_samples = buf->datas[0].chunk->size / sizeof(float);

    fprintf(stdout, "captured %d samples\n", n_samples / n_channels);

//     fprintf(stdout, "channel %d: |%*s%*s| peak:%f\n",
//             c, peak+1, "*", 40 - peak, "", max);
    pw_stream_queue_buffer(data->stream, b);

//     int _buflen = n_samples; /// xxx do we need the global?

    float sampled_window[_N];
    sample_windowf( &flat_top, sampled_window, _N );

    float *xi; // we read into a circular buffer. this points to the start of the buffer
    float tmp[_N];

    fftwf_plan plan;
    plan = fftwf_plan_dft_r2c_1d(_N, tmp, X, FFTW_MEASURE);

    xi = x + s;

    for (int i = 0; i < n_samples; ++i, ++xi) {
        *xi = samples[i]; 

        max = fmaxf(max, fabsf(samples[i]));
        apply_window(sampled_window, x, tmp, s, _N);

        fftwf_execute(plan);
    }

    s = (s+n_samples) %_N;

    gather();
//     print_equalizer(absX, max_absX, _nfreq, 25);
//     for ( size_t s=0;; s= (s+_buflen) %_N ) {
//     for ( size_t s=0;; s= (s+n_samples) %_N ) {
//         xi = x + s;
//     }

    pw_stream_queue_buffer(data->stream, b);
}

/* our data processing function is in general:
 *
 *  struct pw_buffer *b;
 *  b = pw_stream_dequeue_buffer(stream);
 *
 *  .. consume stuff in the buffer ...
 *
 *  pw_stream_queue_buffer(stream, b);
 */
static 
void __process(void *userdata)
{
    struct data *data = userdata;
    struct pw_buffer *b;
    struct spa_buffer *buf;
    float *samples, max;
    uint32_t c, n, n_channels, n_samples, peak;

    if ((b = pw_stream_dequeue_buffer(data->stream)) == NULL) {
        pw_log_warn("out of buffers: %m");
        return;
    }

    buf = b->buffer;
    if ((samples = buf->datas[0].data) == NULL)
        return;

    n_channels = data->format.info.raw.channels;
    n_samples = buf->datas[0].chunk->size / sizeof(float);

    /* move cursor up */
    if (data->move)
        fprintf(stdout, "%c[%dA", 0x1b, n_channels + 1);

    fprintf(stdout, "captured %d samples\n", n_samples / n_channels);
    for (c = 0; c < n_channels; c++) {
        max = 0.0f;

        for (n = c; n < n_samples; n += n_channels)
            max = fmaxf(max, fabsf(samples[n]));

        peak = (uint32_t)SPA_CLAMPF(max * 30, 0.f, 39.f);

        fprintf(stdout, "channel %d: |%*s%*s| peak:%f\n",
                c, peak+1, "*", 40 - peak, "", max);
    }
    data->move = true;
    fflush(stdout);

    pw_stream_queue_buffer(data->stream, b);
}

/* Be notified when the stream param changes. We're only looking at the
 * format changes.
 */
static void
on_stream_param_changed(void *_data, uint32_t id, const struct spa_pod *param)
{
    struct data *data = _data;

    /* NULL means to clear the format */
    if (param == NULL || id != SPA_PARAM_Format)
        return;

    if (spa_format_parse(param, &data->format.media_type, &data->format.media_subtype) < 0)
        return;

    /* only accept raw audio */
    if (data->format.media_type != SPA_MEDIA_TYPE_audio ||
            data->format.media_subtype != SPA_MEDIA_SUBTYPE_raw)
        return;

    /* call a helper function to parse the format for us. */
    spa_format_audio_raw_parse(param, &data->format.info.raw);

    fprintf(stdout, "capturing rate:%d channels:%d\n",
            data->format.info.raw.rate, data->format.info.raw.channels);
}

static const 
struct pw_stream_events stream_events = {
    PW_VERSION_STREAM_EVENTS,
    .param_changed = on_stream_param_changed,
    .process = _process,
};

static 
void quit(void *userdata, int signal_number)
{
    struct data *data = userdata;
    pw_main_loop_quit(data->loop);
}

int main(int argc, char *argv[])
{
    pw_init(&argc, &argv);

    struct data data = { 0, };
    data.loop = pw_main_loop_new(NULL);

    pw_loop_add_signal(pw_main_loop_get_loop(data.loop), SIGINT, quit, &data);
    pw_loop_add_signal(pw_main_loop_get_loop(data.loop), SIGTERM, quit, &data);

    /* Create a simple stream, the simple stream manages the core and remote
     * objects for you if you don't need to deal with them.
     *
     * If you plan to autoconnect your stream, you need to provide at least
     * media, category and role properties.
     *
     * Pass your events and a user_data pointer as the last arguments. This
     * will inform you about the stream state. The most important event
     * you need to listen to is the process event where you need to produce
     * the data.
     */
    struct pw_properties *props =
        pw_properties_new(PW_KEY_MEDIA_TYPE, "Audio",
                          PW_KEY_MEDIA_CATEGORY, "Capture",
                          PW_KEY_MEDIA_ROLE, "Music",
                          NULL);

    if (argc > 1)
        /* Set stream target if given on command line */
        pw_properties_set(props, PW_KEY_TARGET_OBJECT, argv[1]);

    /* uncomment if you want to capture from the sink monitor ports */
    /* pw_properties_set(props, PW_KEY_STREAM_CAPTURE_SINK, "true"); */

    data.stream = 
        pw_stream_new_simple( pw_main_loop_get_loop(data.loop),
                              "audio-capture",
                              props,
                              &stream_events,
                              &data );

    /* Make one parameter with the supported formats. The SPA_PARAM_EnumFormat
     * id means that this is a format enumeration (of 1 value).
     * We leave the channels and rate empty to accept the native graph
     * rate and channels. */
    uint8_t buffer[1024];
    struct spa_pod_builder b = SPA_POD_BUILDER_INIT(buffer, sizeof(buffer));
    const struct spa_pod *params[1];

    struct spa_audio_info_raw   audio_info = {
        .format = SPA_AUDIO_FORMAT_F32,
        .channels = 1
//         .rate = 48000,
    };

    params[0] = spa_format_audio_raw_build(&b, SPA_PARAM_EnumFormat,
            &audio_info);
//                                            &SPA_AUDIO_INFO_RAW_INIT(
//                                                 .format = SPA_AUDIO_FORMAT_F32,
//                                                 .channels = SPA_AUDIO_CHANNEL_MONO
//                                                 ));

    enum pw_stream_flags flags = PW_STREAM_FLAG_AUTOCONNECT |
                                 PW_STREAM_FLAG_MAP_BUFFERS |
                                 PW_STREAM_FLAG_RT_PROCESS;   // realtime thread

    pw_stream_connect(data.stream, PW_DIRECTION_INPUT, PW_ID_ANY,
                      flags, params, 1);

    pw_main_loop_run(data.loop);

cleanup:
    pw_stream_destroy(data.stream);
    pw_main_loop_destroy(data.loop);
    pw_deinit();

    return 0;
}
