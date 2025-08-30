/*
   * use pipewire stream api to capture audio
   * fourier transform the data
   * make the fft available to a fragment shader
   * .. and draw it
   Pipewire part is taken from https://docs.pipewire.org/audio-capture_8c-example.html
   Copyright © 2022 Wim Taymans
 */

/// Readme pipewire pods
/// readme 
// https://docs.pipewire.org/page_spa_pod.html

#include <stdio.h>
#include <math.h>
#include <signal.h>

#include <complex.h> /// must be included before fftw so that fftw uses _Complex
#include <fftw3.h>

#include <spa/param/audio/format-utils.h>
#include <pipewire/pipewire.h>
// #include <spa/debug/pod.h>

#include "globals.h"

#include "windows.c"
#include "util.c"


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


/// fftw_malloc!
float tmp[_N];
int s = 0;

static
void _process(void *userdata)
{
        fprintf(stderr, "processing\n");
    struct mydata *data = userdata;

    struct pw_buffer *b;
    struct spa_buffer *buf;
    if ((b = pw_stream_dequeue_buffer(data->stream)) == NULL) {
        pw_log_warn("out of buffers: %m");
        fprintf(stderr, "out of buffers: %m\n");
        return;
    }

    float *samples;
    buf = b->buffer;
    if ((samples = buf->datas[0].data) == NULL) {
        fprintf(stderr, "no data in buffer\n");
        return;
    }

    int n_channels = data->format.info.raw.channels;
    int n_samples = buf->datas[0].chunk->size / sizeof(float);

    fprintf(stderr, "captured %d samples\n", n_samples / n_channels);

//     pw_stream_queue_buffer(data->stream, b);

//     int _buflen = n_samples; /// xxx do we need the global?

    float sampled_window[_N];
    sample_windowf( &blackman_nuttal, sampled_window, _N );

    float *xi; // we read into a circular buffer. this points to the start of the buffer
    xi = x + s;

    exit_if( _buflen != n_samples);

    float max = 0.0f;
    uint32_t peak;
    for (int i = 0; i < n_samples; ++i, ++xi) {
        *xi = samples[i]; 

        /// fuck. s+n_namples must go in here
        apply_window_on_ringbuffer(sampled_window, x, tmp, s, _N);

        fftwf_execute(plan);
    }
    s = (s+n_samples) % _N;

    float scale = 1000.0f / (float) _nfreq;
//     float scale = 1.0;
    process_freqs( X, _nfreq, absX, labsX, max_absX, max_labsX,
                   &schwerpunkt, scale );
    gather_bands(_nfreq, labsX, _nband, E, E_max, Ecoarse, max_Ecoarse);

    print_equalizer(labsX, max_labsX, 26, 30);
//     print_equalizer(absX, max_absX, 26, 25);
//     print_equalizer(absX, max_absX, _nfreq, 25);
//     print_equalizer(E, E_max, _nband, 25);

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
    c = 0;
//     for (c = 0; c < n_channels; c++) {
        max = 0.0f;

        for (n = c; n < n_samples; n += n_channels)
            max = fmaxf(max, fabsf(samples[n]));

        peak = (uint32_t)SPA_CLAMPF(max * 30, 0.f, 39.f);

        fprintf(stdout, "channel %d: |%*s%*s| peak:%f\n",
                c, peak+1, "*", 40 - peak, "", max);
//     }
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
    fprintf(stderr, "on_stream_param_changed id:%d %p\n", id, param);

    struct data *data = _data;

    /* NULL means to clear the format */
    if (param == NULL || id != SPA_PARAM_Format)
        return;

//     spa_debug_pod(2, NULL, param);

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
    plan = fftwf_plan_dft_r2c_1d(_N, tmp, X, FFTW_MEASURE);

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
    pw_properties_set(props, PW_KEY_STREAM_CAPTURE_SINK, "true");

    pw_properties_set(props, PW_KEY_NODE_LATENCY, "256/48000");

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
        .format = SPA_AUDIO_FORMAT_F32_LE,
        .channels = 1,
//         .rate = 48000,
    };

    params[0] = spa_format_audio_raw_build(&b, SPA_PARAM_EnumFormat,
            &audio_info);

    enum pw_stream_flags flags = PW_STREAM_FLAG_AUTOCONNECT |
                                 PW_STREAM_FLAG_MAP_BUFFERS |
                                 PW_STREAM_FLAG_RT_PROCESS;   // realtime thread

    pw_stream_connect(data.stream, PW_DIRECTION_INPUT, PW_ID_ANY,
                      flags, params, 1);

    uint32_t node_id = pw_stream_get_node_id(data.stream);

    pw_main_loop_run(data.loop);

cleanup:
    pw_stream_destroy(data.stream);
    pw_main_loop_destroy(data.loop);
    pw_deinit();

    return 0;
}
