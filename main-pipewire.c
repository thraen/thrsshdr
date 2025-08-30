/*
   * use pipewire stream api to capture audio
   * fourier transform the data
   * make the fft available to a fragment shader
   Pipewire Copyright © 2022 Wim Taymans
 */

#include <stdio.h>
#include <math.h>
#include <signal.h>

#include <complex.h> /// must be included before fftw so that fftw uses _Complex
#include <fftw3.h>

#include <spa/param/audio/format-utils.h>
#include <pipewire/pipewire.h>
#include <spa/debug/pod.h>

#include "globals.h"
#include "glcrap.h"

#include "windows.c"
#include "util.c"


static struct pw_main_loop *loop;
static struct pw_stream *stream;
static struct spa_audio_info format;
static float *tmp;

static
void process(void *userdata)
{
    struct pw_buffer *b;
    struct spa_buffer *buf;
    if ((b = pw_stream_dequeue_buffer(stream)) == NULL) {
        err_exit("out of buffers: %m");
        return;
    }

    float *samples;
    buf = b->buffer;
    if ((samples = buf->datas[0].data) == NULL) {
        wrn("no data in buffer\n");
        return;
    }

    int n_channels = format.info.raw.channels;
    int n_samples = buf->datas[0].chunk->size / sizeof(float);

//     fprintf(stderr, "captured %d samples\n", n_samples / n_channels);

    float sampled_window[_N];
    sample_windowf( &hann, sampled_window, _N );

    static int s = 0;
    float *xi; // we read into a circular buffer. this points to the start of the buffer
    xi = x + s;

    exit_if( _buflen != n_samples); // fuck make those same

    float max = 0.0f;
    uint32_t peak;
    for (int i = 0; i < n_samples; ++i, ++xi) {
        *xi = samples[i]; 

        /// the oldest element of the buffer is at s+_buflen! that is important
        apply_window_on_ringbuffer(sampled_window, x, tmp, s + n_samples, _N);

        fftwf_execute(plan);
    }
    s = (s+n_samples) % _N;

    float scale = 1000.0f / (float) _nfreq;
//     float scale = 1.0;
    process_freqs( X, _nfreq, absX, labsX, max_absX, max_labsX,
                   &schwerpunkt, scale );
    gather_bands(_nfreq, labsX, _nband, E, E_max, Ecoarse, max_Ecoarse);

//     print_equalizer(labsX, max_labsX, 26, 30);
//     print_equalizer(absX, max_absX, 26, 25);
//     print_equalizer(absX, max_absX, _nfreq, 25);
    print_equalizer(E, E_max, _nband, 50);

    pw_stream_queue_buffer(stream, b);
}

static void
on_stream_param_changed(void *_data, uint32_t id, const struct spa_pod *param)
{
    fprintf(stderr, "on_stream_param_changed id:%d %p\n", id, param);

    /* NULL means to clear the format */
    if (param == NULL || id != SPA_PARAM_Format)
        return;

    spa_debug_pod(2, NULL, param);

    if (spa_format_parse(param, &format.media_type, &format.media_subtype) < 0)
        return;

    /* only accept raw audio */
    if (format.media_type != SPA_MEDIA_TYPE_audio ||
        format.media_subtype != SPA_MEDIA_SUBTYPE_raw)
        return;

    /* call a helper function to parse the format for us. */
    spa_format_audio_raw_parse(param, &format.info.raw);

    fprintf(stdout, "capturing rate:%d channels:%d\n",
            format.info.raw.rate, format.info.raw.channels);
}

static const 
struct pw_stream_events stream_events = {
    PW_VERSION_STREAM_EVENTS,
    .param_changed = on_stream_param_changed,
    .process = process,
};

static 
void quit(void *userdata, int signal_number)
{
    pw_main_loop_quit(loop);
}

int main(int argc, char *argv[])
{
    tmp = (float*) fftwf_malloc(sizeof(float) * _N);
    plan = fftwf_plan_dft_r2c_1d(_N, tmp, X, FFTW_MEASURE);

    pw_init(&argc, &argv);

    loop = pw_main_loop_new(NULL);

    pw_loop_add_signal(pw_main_loop_get_loop(loop), SIGINT, quit, NULL);
    pw_loop_add_signal(pw_main_loop_get_loop(loop), SIGTERM, quit, NULL);

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

    char latency_str[50];
    snprintf(latency_str, 50, "%d/48000", _buflen);
    pw_properties_set(props, PW_KEY_NODE_LATENCY, latency_str);

    stream = 
        pw_stream_new_simple( pw_main_loop_get_loop(loop),
                              "audio-capture",
                              props,
                              &stream_events,
                              NULL ); // no user data

    char buf[2000];
    struct spa_pod_builder b = SPA_POD_BUILDER_INIT(buf, sizeof(buf));
    const struct spa_pod *params[1];

    struct spa_audio_info_raw   audio_info = {
        .format = SPA_AUDIO_FORMAT_F32_LE,
        .channels = 1,
        .rate = 48000,
    };

    params[0] = spa_format_audio_raw_build(&b, SPA_PARAM_EnumFormat,
            &audio_info);

    enum pw_stream_flags flags = PW_STREAM_FLAG_AUTOCONNECT |
                                 PW_STREAM_FLAG_MAP_BUFFERS |
                                 PW_STREAM_FLAG_RT_PROCESS;   // realtime thread

    pw_stream_connect(stream, PW_DIRECTION_INPUT, PW_ID_ANY,
                      flags, params, 1);

    uint32_t node_id = pw_stream_get_node_id(stream);

    pw_main_loop_run(loop);

cleanup:
    pw_stream_destroy(stream);
    pw_main_loop_destroy(loop);
    pw_deinit();

    return 0;
}
