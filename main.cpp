// start with ./.aout plughw:0,1
//
// where  the argument is an alsa capture device
// we can setup alsa to use loopback device for audio
// playback so all the sound that is played is accessible 
// through this device: 

#include <stdio.h>
#include <pthread.h>
#include <alsa/asoundlib.h>
#include <math.h>
#include <GL/glew.h>
#include <fftw3.h>

#include "globals.h"
#include "things.h"
#include "initalsa.cpp"

Quad  init_quad; 
Quad  postproc_quad;  
Quad  dgl_tmp_quad;  

static void keyCallback( unsigned char key, int x, int y );
static void gamepad( unsigned int buttonMask, int x, int y, int z );

#include <pulse/simple.h>
#include <pulse/error.h>

static const pa_sample_spec ss = {
    .format = PA_SAMPLE_FLOAT32LE,
    .rate = 44100,
    .channels = 2
};

static int read_pcm(snd_pcm_t *handle, void** x, size_t n) {

    int err = snd_pcm_readn(handle, (void**) x, n);

    if (err < 0) {
        int e = snd_pcm_recover(handle, err, 1);
        fprintf( stderr, "%d, %d\n ", err, e);
    }
    return err;
};

static void print_bars(const float *E, const float *E_max, size_t n, size_t maxlen) {
    char s[maxlen+1];
    s[maxlen] = '\0';

    fprintf(stderr, "%d %d %d                \n", _lowbound, _midbound, _higbound);

    for (int i=0; i<n; i++) {
        float logE = log(E[i] + M_E-0.1);

        int len = _max( _min( logE, maxlen ), 0);

        memset(s, '*', len);
        memset(s+len, ' ', maxlen-len);

        int m = log(E_max[i] + M_E-0.1);
        s[m]  = '|';

        fprintf(stderr, "%d %6.3f %s\n", i, logE, s);

//         fprintf(stderr, "%04d : %d              \n",i, len);
    }
    fprintf(stderr, "\x1b[%luA", n+1);
}

static void gather() {
    for ( int k=0; k<_nband; k++ ){ 
        E[k] = 0;
        for ( int j = pow(2,k)-1; j < pow(2,k+1)-1; j++ ) {
            normX[j] = (X[j][0] * X[j][0] + X[j][1] * X[j][1]) / _nfreq;
            nXmax[j] = _max(normX[j], nXmax[j]);
//             fprintf(stderr, "%d %d, %f %f %f \n", k, j, X[j][0], X[j][1], normX[j]);

            E[k] += normX[k];
        }
        E_max[k] = _max(E[k], E_max[k]);
        E_gesamt  += E[k];
    }
}

static void* do_fft( void *ptr ) {
    int n;

    while (1) {
        n = read_pcm( handle, (void**) x, _buflen );

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
    // thr! destroy texture!
    init_texture(render_texture3, w, h);
    init_texture(render_texture2, w, h);
    init_texture(render_texture , w, h);

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, render_texture2, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    init_quad.draw();

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, render_texture, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    init_quad.draw();
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
//     init_quad.draw();
//     /*

    // Render to texture 
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // we render to render_texture3
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, render_texture3, 0);

    // two input textures that were rendered into from last and the previous to last 
    // pass of this loop
    dgl_tmp_quad.draw(render_texture, render_texture2);
    
    // finally render render_texture3 to screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    postproc_quad.draw(render_texture3);


//     */

    glutSwapBuffers();

    // Cycle textures
    GLuint tmp      = render_texture3;
    render_texture3 = render_texture2;
    render_texture2 = render_texture;
    render_texture  = tmp;

//     sleep(0.6);
}


int main(int argc, char** argv) {
    //alsa
    char *snd_device = argv[1];

    fprintf(stderr, "\n================= capture device: %s =================\n", snd_device);
    int err;

    if ( (err = snd_pcm_open(&handle, snd_device, SND_PCM_STREAM_CAPTURE, 0 )) < 0 ) { 
        fprintf(stderr, "cannot open audio device %s (%s)\n", snd_device, snd_strerror (err)); exit (1); }

    alsa_setpar( handle, snd_device );

//     if ( !(handle = pa_simple_new(NULL, snd_device, PA_STREAM_RECORD, NULL, "record", &ss, NULL, NULL, &err)) ) {
//         fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(err));
//         exit(1);
//     }

    fprintf(stderr, "\nusing read buffer size %d\n", _buflen);
    fprintf(stderr, "this results in frequency resolution of %d\n", _nfreq);
    fprintf(stderr, "we map them to %d energy bands\n\n", _nband);

    //fft
    plan = fftwf_plan_dft_r2c_1d(_buflen, x[0], X, FFTW_MEASURE);

    //GL
    glutInit(&argc, argv);
    glutInitContextVersion(3, 3);
    glutInitContextFlags( GLUT_FORWARD_COMPATIBLE | GLUT_CORE_PROFILE );
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);

    glutInitWindowSize(1024, 768); // get window size?
    glutCreateWindow("dings");

    glutKeyboardFunc(keyCallback);
    
    glutJoystickFunc(gamepad, 300);
    glutForceJoystickFunc();

    glutDisplayFunc(render);
    glutIdleFunc(render);
    glutReshapeFunc(reshape);

    // Must be done after glut is initialized!
    GLenum res = glewInit();
    if (res != GLEW_OK) { printf("Error: '%s'\n", glewGetErrorString(res)); return 1; }

    const unsigned char *gl_version = glGetString(GL_VERSION);
    fprintf(stderr, "GL Version: %s\n", gl_version);
    if ( memcmp(gl_version, "4.", 2) != 0 ) {
        fprintf(stderr, "\nERROR: GL Version not supported\n"); exit(1); }


    fprintf(stderr, "GLEW Version %s\n", glewGetString(GLEW_VERSION));
//     fprintf(stderr, "GL Extensions:\n %s\n", glGetString(GL_EXTENSIONS));

    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &err);
    fprintf(stderr, "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS: %d\n", err);

    // setup framebuffer for render to texture
    fprintf(stderr, "set up framebuffer\n");
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glGenFramebuffers(1, &framebuffer);
    fprintf(stderr, "create textures\n");
    glGenTextures(1, &render_texture);
    glGenTextures(1, &render_texture2);
    glGenTextures(1, &render_texture3);

    setup_render_texture( render_texture , 1024, 768 );
    setup_render_texture( render_texture2, 1024, 768 );
    setup_render_texture( render_texture3, 1024, 768 );

    // Set render_texture as our colour attachement #0 for render to texture
    fprintf(stderr, "set up render to texture\n");
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, render_texture, 0);

    //  //
    //  // Set the list of draw buffers. braucht man das?
    //  GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    //  glDrawBuffers(1, DrawBuffers); //only one drawbuffer

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return false;

    fprintf(stderr, "load shaders\n");
    init_quad.init    ("v.vert", "triangle.frag", false);
    dgl_tmp_quad.init ("v.vert", "link.frag", true);
    postproc_quad.init("v.vert", "postprocess.frag", false);

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

finish:
    if (plan)      fftwf_destroy_plan(plan);
    if (handle)    snd_pcm_close(handle);
//     if (handle) pa_simple_free(handle);

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
            fprintf(stderr, "reloading shaders\n");
            init_quad.recompile_shaders(false);
            dgl_tmp_quad.recompile_shaders(false);
            postproc_quad.recompile_shaders(false);
            break;
    }
}

static void gamepad(unsigned int buttonMask, int x, int y, int z){
    if(buttonMask & GLUT_JOYSTICK_BUTTON_A) {
        printf("button A is pressed ");
    }
    if(buttonMask & GLUT_JOYSTICK_BUTTON_B) {
        printf("button B is pressed ");
    }
    if(buttonMask & GLUT_JOYSTICK_BUTTON_C) {
        printf("button C is pressed ");
    }
    if(buttonMask & GLUT_JOYSTICK_BUTTON_D) {
        printf("button D is pressed ");
    }
}

