#include <pulse/simple.h>
#include <pulse/error.h>

#include <alsa/asoundlib.h>
#include "initalsa.cpp"

#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

#include <GL/glew.h>

#include <fftw3.h>


#include "globals.h"
#include "things.h"

#include "windows.cpp"

Rect  init_rect;
Rect  postproc_rect;
Rect  render_rect;

static void keyCallback( unsigned char key, int x, int y );
static void gamepad( unsigned int buttonMask, int x, int y, int z );

#define ALSA 1

static int read_alsa(snd_pcm_t *handle, void **p, size_t n) {
    int err = snd_pcm_readn(handle, (void **) p, n);

    if (err < 0) {
        int e = snd_pcm_recover(handle, err, 1);
        nfo("recovering alsa pcm %d, %d\n ", err, e);
    }
    return err;
};

static int read_pa(pa_simple *s, void *p, size_t n) {
    int err;
    if (pa_simple_read(s, p, n, &err) < 0)
        dbg(__FILE__": pa_simple_read() failed: %s\n", pa_strerror(err));
}

static void gather() {
    for ( int k=0; k<_nband; k++ ){ 
        E[k] = 0;
        // xxx use sum and indices
        for ( int j = pow(2,k)-1; j < pow(2,k+1)-1; j++ ) {
            normX[j] = (X[j][0] * X[j][0] + X[j][1] * X[j][1]) / _nfreq;
            nXmax[j] = _max( normX[j], nXmax[j] );
//             fprintf(stderr, "%d %d, %f %f %f \n", k, j, X[j][0], X[j][1], normX[j]);

            E[k] += normX[k];
        }
        E_max[k] = _max(E[k], E_max[k]);
    }
}

void apply_window( float *wsamp, float *x[2], float *out, size_t s, size_t N ) {
    for ( int i=0; i<N; i++ ) {
        out[i] = x[0][ (i-s)%N ] * wsamp[i];
    }
}

static long tdiff(timespec T, timespec t0) {
    return (T.tv_sec  - t0.tv_sec) * (long)1e9
         + (T.tv_nsec - t0.tv_nsec);
}

static void* do_fft( void *ptr ) {

    timespec t0, T;

    size_t n, s;
    float *xi[2]; // we use x as a circular buffer. this points to the start of the buffer

    float *wsamp = sample_windowf( &hamming, _N );
    float *tmp   = (float *) malloc( sizeof(float)*_N );
    
    plan = fftwf_plan_dft_r2c_1d(_N, tmp, X, FFTW_MEASURE);

    for ( s=0;; s= (s+_buflen) %_N ) {

        //// overlapping moving windows: shift pointer on x s forward mod N
        xi[0] = & (x[0][s]);
        xi[1] = & (x[1][s]);

        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t0);
#ifdef ALSA
        n = read_alsa( handle, (void**) xi, _buflen );
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &T);
//         fprintf(stderr, "%lu \n", tdiff(T, t0));
#else
        const pa_sample_spec ss = { .format = PA_SAMPLE_FLOAT32LE, .rate = 48000, .channels = 1 };// xxx global
//         size_t nbytes = _buflen * pa_sample_size(&ss) * pa_frame_size(&ss); // xxx const
        size_t nbytes = _buflen * pa_frame_size(&ss); // xxx const
        n = read_pa( pa_source, (void*) xi[0], nbytes );
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &T);
//         fprintf(stderr, "%lu %lu %lu \n", tdiff(T, t0), pa_frame_size(&ss), pa_sample_size(&ss));
#endif
//         fprintf(stderr, "%lu \n", T-t0);

        //// copy data and apply window function
        apply_window(wsamp, x, tmp, s, _N);

        fftwf_execute(plan);

        gather();

        low = sum(E, 0          , _lowbound) / _nband;
        mid = sum(E, _lowbound+1, _midbound) / _nband;
        hig = sum(E, _midbound+1, _higbound) / _nband;

//         fprintf(stderr, "low  : %f   \n",low);
//         fprintf(stderr, "mid  : %f   \n",mid);
//         fprintf(stderr, "hig  : %f   \n",hig);

//         print_bars(E, E_max, _nband, 30);
//         print_bars(normX, nXmax, _nfreq, 30);

        low = 0.05*log(1+low);
        mid = 0.01*log(1+mid);
        hig = 0.05*log(1+hig);
    };
}


static void reshape(int w, int h){
    glViewport(0, 0, w, h);
    _w        = w;
    _h        = h;
    _n_frames = 0;
    _t0       = glutGet(GLUT_ELAPSED_TIME);
    // reinit the texture to new w/h
    // xxx! destroy texture!
    init_texture(render_texture3, w, h);
    init_texture(render_texture2, w, h);
    init_texture(render_texture , w, h);

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, render_texture2, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    init_rect.draw();

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, render_texture, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    init_rect.draw();
}

static void render() {
    _n_frames++;
    _frame_t   = glutGet(GLUT_ELAPSED_TIME)-_elapsed_t-_t0;
    _elapsed_t = glutGet(GLUT_ELAPSED_TIME)-_t0;

    //     fprintf(stderr, "_frame_t %d, fpms %f, _n_frames %d\n", _frame_t, _n_frames*1.0/_elapsed_t, _n_frames);
    //     fprintf(stderr, "%u _frame_t %u, low %f mid %f hig %f\n", _elapsed_t, _frame_t, low, mid, hig);

    // Render to Screen
//     glBindFramebuffer(GL_FRAMEBUFFER, 0);
//     glClear(GL_COLOR_BUFFER_BIT);
//     init_rect.draw();
//     /*

    // Render to texture 
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // we render to render_texture3
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, render_texture3, 0);

    // two input textures that were rendered into from last and the previous to last 
    // pass of this loop
    render_rect.draw(render_texture, render_texture2);
    
    // finally render render_texture3 to screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    postproc_rect.draw(render_texture3);


//     */

    glutSwapBuffers();

    // Cycle textures
    GLuint tmp      = render_texture3;
    render_texture3 = render_texture2;
    render_texture2 = render_texture;
    render_texture  = tmp;

//     sleep(1);
}


int main(int argc, char** argv) {

    char *snd_src_name = argv[1];

    if (argc != 2) 
        errexit("give capture device/source as argument\n");

    int err;
    printf("\n=== capture device: %s ===\n", snd_src_name);

#ifdef ALSA
    if ( (err = snd_pcm_open(&handle, snd_src_name, SND_PCM_STREAM_CAPTURE, 0 )) < 0 ) 
        errexit("cannot open audio device %s (%s)\n", snd_src_name, snd_strerror (err))

    alsa_setpar( handle, snd_src_name );
#else
    const pa_sample_spec ss = { .format = PA_SAMPLE_FLOAT32LE, .rate = 48000, .channels = 1 };

    static const pa_buffer_attr attr = {
        .maxlength = (uint32_t) -1,
        .tlength = (uint32_t) -1,
        .prebuf = (uint32_t) -1,
        .minreq = (uint32_t) -1,
        .fragsize = 160 // xxx
    };

    if (!(pa_source = pa_simple_new(NULL, argv[0], PA_STREAM_RECORD, snd_src_name, "record", &ss, NULL, &attr, &err)))
        errexit(__FILE__": pa_simple_new() for source %s failed: %s\n", snd_src_name, pa_strerror(err));
#endif

    printf("\nusing read buffer size %d\n", _N);
    printf("this results in frequency resolution of %d\n", _nfreq);
    printf("we map them to %d energy bands\n\n", _nband);

    //GL
    glutInit(&argc, argv);
    glutInitContextVersion(3, 3);
    glutInitContextFlags( GLUT_FORWARD_COMPATIBLE | GLUT_CORE_PROFILE );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA );

    glutInitWindowSize(1024, 768); // get window size?
    glutCreateWindow("__fuck__");

    glutKeyboardFunc(keyCallback);
    
    glutJoystickFunc(gamepad, 300);
    glutForceJoystickFunc();

    glutDisplayFunc(render);
    glutIdleFunc(render);
    glutReshapeFunc(reshape);

    // glew init after glut is init!
    GLenum res = glewInit();
    if (res != GLEW_OK)
        errexit("Error: '%s'\n", glewGetErrorString(res));

    const unsigned char *gl_version = glGetString(GL_VERSION);
    nfo("GL Version: %s\n", gl_version);
    if ( memcmp(gl_version, "4.", 2) != 0 )
        errexit("\nERROR: GL Version not supported\n")


    dbg("GLEW Version %s\n", glewGetString(GLEW_VERSION));
    dbg("GL Extensions:\n %s\n", glGetString(GL_EXTENSIONS));

    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &err);
    dbg("GL_MAX_FRAGMENT_UNIFORM_COMPONENTS: %d\n", err);

    // setup framebuffer for render to texture
    nfo("set up framebuffer\n");
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glGenFramebuffers(1, &framebuffer);
    nfo("create textures\n");
    glGenTextures(1, &render_texture);
    glGenTextures(1, &render_texture2);
    glGenTextures(1, &render_texture3);

    setup_render_texture( render_texture , 1024, 768 );
    setup_render_texture( render_texture2, 1024, 768 );
    setup_render_texture( render_texture3, 1024, 768 );

    // Set render_texture as our colour attachement #0 for render to texture
    nfo("set up render to texture\n");
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, render_texture, 0);

    //  //
    //  // Set the list of draw buffers. braucht man das?
    //  GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    //  glDrawBuffers(1, DrawBuffers); //only one drawbuffer

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return false;

    nfo("load shaders\n");
    init_rect.init    ("v.vert", "triangle.frag",    false);
    render_rect.init  ("v.vert", "link.frag",        false);
    postproc_rect.init("v.vert", "postprocess.frag", false);

    // start reading from capture device and do fft in own thread
    pthread_t audio_thread;
    int audio_ret   = pthread_create( &audio_thread, NULL, do_fft, NULL );

    // turn vsync off. seems to not work with nvidia. needs to be turned off in driver
    // glXSwapIntervalEXT(-1);
    // glXSwapIntervalEXT(0);
    // glXSwapIntervalSGI(0);
    _t0 = glutGet(GLUT_ELAPSED_TIME);

    memset(normX, 0, _nfreq);
    memset(nXmax, 0, _nfreq);

    memset(E,     0, _nband);
    memset(E_max, 0, _nband);

    glutMainLoop();

    // xxx thread, opengl
#ifdef ALSA
    if (handle)    snd_pcm_close(handle);
#else
    if (pa_source) pa_simple_free(pa_source);
#endif
    if (plan)      fftwf_destroy_plan(plan);

    return 0;
}

static void keyCallback(unsigned char key, int x, int y){
    switch(key){
        case 27:
            exit(0);
            break;
        case 'q':
            exit(0);
            break;
        case 'r':
            nfo("reloading shaders\n");
            init_rect.recompile_shaders(false);
            render_rect.recompile_shaders(false);
            postproc_rect.recompile_shaders(false);
            break;
    }
}

static void gamepad(unsigned int buttonMask, int x, int y, int z){
    if(buttonMask & GLUT_JOYSTICK_BUTTON_A) {
        dbg("button A is pressed ");
    }
    if(buttonMask & GLUT_JOYSTICK_BUTTON_B) {
        dbg("button B is pressed ");
    }
    if(buttonMask & GLUT_JOYSTICK_BUTTON_C) {
        dbg("button C is pressed ");
    }
    if(buttonMask & GLUT_JOYSTICK_BUTTON_D) {
        dbg("button D is pressed ");
    }
}

