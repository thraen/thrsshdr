#include "globals.h"
#include "things.h"

#include <pulse/simple.h>
#include <pulse/error.h>

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <time.h>
#include <complex.h>

#include <fftw3.h>

#include <GL/glew.h>

#include "windows.cpp"


Rect  init_rect;
Rect  postproc_rect;
Rect  render_rect;

static void keyCallback( unsigned char key, int x, int y );
static void gamepad( unsigned int buttonMask, int x, int y, int z );

static void gather() {
    for ( int k=0; k<_nband; k++ ){ 
        E[k] = 0;
        // xxx use sum and indices
        for ( int j = pow(2,k)-1; j < pow(2,k+1)-1; j++ ) {
            absX[j] = cabsf(X[j]) / _nfreq;
            max_absX[j] = _max( absX[j], max_absX[j] );

            labsX[j]     = log(1+absX[j]);
            max_labsX[j] = _max( log(1+absX[j]), max_labsX[j] );

            E[k] += labsX[k];
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

void apply_window( float *wsamp, float *x, float *out, size_t s, size_t N ) {
    for ( int i=0; i<N; i++ ) {
        out[i] = x[ (i-s)%N ] * wsamp[i];
    }
}

static void* do_fft( void *ptr ) {
//     __init_timer();

    size_t n, s;
    int err;
    
    size_t nbytes = _buflen * pa_frame_size(&_pa_sspec); // xxx const

    float *wsamp = sample_windowf( &hamming, _N );

    float *xi; // we read into a circular buffer. this points to the start of the buffer
    
    float *tmp = (float *) malloc( sizeof(float)*_N );

//     plan = fftwf_plan_dft_r2c_1d(_N, tmp, X, FFTW_MEASURE);
    plan = fftwf_plan_dft_r2c_1d(_N, tmp, reinterpret_cast<fftwf_complex*>(X), FFTW_MEASURE); // xxx todo: get rid of c++

    for ( s=0;; s= (s+_buflen) %_N ) {

        //// overlapping moving windows: shift pointer on x s forward mod N
        xi = & (x[s]);

        if (pa_simple_read( pa_source, (void*) xi, nbytes, &err) < 0)
            dbg(__FILE__": pa_simple_read() failed: %s\n", pa_strerror(err));

//         __start_timer();

        //// copy data and apply window function
        apply_window(wsamp, x, tmp, s, _N);

        fftwf_execute(plan);

        gather();

//         __stop_timer();

//         print_equalizer(absX, max_absX, _nfreq, 25);
        print_equalizer(E, E_max, 3, 25);
//         print_equalizer(Ecoarse, max_Ecoarse, 3, 25);
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
    init_rect.draw();

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, render_texture, 0);
    init_rect.draw();

}

static void render() {
    _n_frames++;
    _frame_t   = glutGet(GLUT_ELAPSED_TIME)-_elapsed_t-_t0;
    _elapsed_t = glutGet(GLUT_ELAPSED_TIME)-_t0;

    //     fprintf(stderr, "_frame_t %d, fpms %f, _n_frames %d\n", _frame_t, _n_frames*1.0/_elapsed_t, _n_frames);

    // Render to Screen
//     glBindFramebuffer(GL_FRAMEBUFFER, 0);
//     init_rect.draw();
//     /*

    // Render to texture 
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // we render to render_texture3
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, render_texture3, 0);

    // two input textures that were rendered into from last and the previous to last pass of this loop
    render_rect.draw(render_texture, render_texture2);
    
    // finally render render_texture3 to screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

    if (!(pa_source = pa_simple_new(NULL, argv[0], PA_STREAM_RECORD, snd_src_name, "record", &_pa_sspec, NULL, &_pa_bufattr, &err)))
        errexit(__FILE__": pa_simple_new() for source %s failed: %s\n", snd_src_name, pa_strerror(err));

    dbg("pulseaudio frame size: %lu, sample size %lu \n", pa_frame_size(&_pa_sspec), pa_sample_size(&_pa_sspec));

    printf("\nusing read buffer size %d\n", _N);
    printf("this results in frequency resolution of %d\n", _nfreq);
    printf("we map them to %d energy bands\n\n", _nband);

    //GL
    glutInit(&argc, argv);
    glutInitContextVersion(3, 3);
    glutInitContextFlags( GLUT_FORWARD_COMPATIBLE | GLUT_CORE_PROFILE );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA );

    glutInitWindowSize(1024, 768); // get window size?
    glutCreateWindow(argv[1]);

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
    dbg("GL Extensions:\n %s\n", glGetString(GL_EXTENSIONS)); // b0rk3d

    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &err);
    nfo("GL_MAX_FRAGMENT_UNIFORM_COMPONENTS: %d\n", err);

    // setup framebuffer for render to texture
    nfo("set up framebuffer\n");
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

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

    //  // Set the list of draw buffers. only needed if we want more than one fragment shdr output
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

    _t0 = glutGet(GLUT_ELAPSED_TIME);

    memset(absX, 0, _nfreq);
    memset(max_absX, 0, _nfreq);

    memset(E,     0, _nband);
    memset(E_max, 0, _nband);

    glutMainLoop();

    // xxx thread, opengl
    if (pa_source) pa_simple_free(pa_source);
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

