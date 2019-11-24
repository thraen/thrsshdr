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

#include "globals.h"
#include "things.h"
#include "initalsa.cpp"

// Fanstern fanstern;   
// Dreieck  dreieck;    
Quad  init_quad; 
Quad  post_processing_quad;  
Quad  dgl_tmp_quad;  

static void keyCallback( unsigned char key, int x, int y );
static void gamepad( unsigned int buttonMask, int x, int y, int z );

static void* do_fft( void *ptr ){
    int err;

    while(1){
        int err, j, k;
        static float E_gesamt_now  = 0.0f;
        static float E_abweichung  = 0.0f;
        static float E_schwerpunkt = 0.0f;

        err = snd_pcm_readn (handle, (void**) x, _buflen);
        if (err < 0){
            int e = snd_pcm_recover(handle, err, 1);
            fprintf( stderr, "%d\n", e);
        }

        fftw_execute(plan);

        for ( k=0; k<_nbands; k++ ){ 
            E[k] = 0;
            for ( j = k * _nfreq/_nbands; j < (k+1) * _nfreq/_nbands; ++j ) { // _nfreq/_nbands = freqwidth of a band
                //fprintf(stderr, "%d %d, %f %f \n", k, j, X[j][0], X[j][1]);
                E[k] += X[j][0] * X[j][0] + X[j][1] * X[j][1];
            }
            E_gesamt_now  += E[k];
            E_schwerpunkt += k * E[k];
            E_abweichung  += (E_gesamt - E[k]) * (E_gesamt - E[k]);
        }
        E_gesamt = E_gesamt_now;

        low = sum(E, 0,  2) / _nbands;
        mid = sum(E, 3,  5) / _nbands;
        hig = sum(E, 6, 50) / _nbands;

        low = 0.5*log(1+low);
        mid = 0.5*log(1+mid);
        hig = 0.5*log(1+hig);

        fprintf(stderr, "samples read %d, low %f mid %f hig %f\n", err, low, mid, hig);
    };
}


static void reshape(int w, int h){
    glViewport(0, 0, w, h);
    _w          = w;
    _h          = h;
    _n_frames   = 0;
    _t0         = glutGet(GLUT_ELAPSED_TIME);
    // reinit the texture to new w/h
    // thr! destroy texture!
    init_texture(render_texture3, w, h);
    init_texture(render_texture2, w, h);
    // render once to first texture 
    glClear(GL_COLOR_BUFFER_BIT);
    init_texture(render_texture , w, h);

    // render once to first texture 
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, render_texture2, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    //  dreieck.draw();
    init_quad.draw();

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, render_texture, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    //  dreieck.draw();
    init_quad.draw();
}

static void render() {
    _n_frames++;
    _frame_t   = glutGet(GLUT_ELAPSED_TIME)-_elapsed_t-_t0;
    _elapsed_t = glutGet(GLUT_ELAPSED_TIME)-_t0;

    //fprintf(stderr, "_elapsed_t %d, _frame_t %d, _n_frames %d, fpms %f\n", _elapsed_t, _frame_t, _n_frames, _n_frames*1.0/_elapsed_t);
    fprintf(stderr, "_frame_t %d, fpms %f, _n_frames %d\n", _frame_t, _n_frames*1.0/_elapsed_t, _n_frames);
    // fprintf(stderr, "%u frame_t %u, low %f mid %f hig %f\n", _elapsed_t, _frame_t, low, mid, hig);

    //  // Render to Screen
    //  glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //  glClear(GL_COLOR_BUFFER_BIT);
    //  dreieck.draw();

    //  // Render to our framebuffer
    //  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    //  glClear(GL_COLOR_BUFFER_BIT);
    //  dreieck.draw();

    // Render 1st texture to 2nd texture
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, render_texture3, 0);
    dgl_tmp_quad.draw(render_texture, render_texture2);
    
    // Render texture to Screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    post_processing_quad.draw(render_texture3);

    glutSwapBuffers();

    // Cycle textures
    GLuint tmp      = render_texture3;
    render_texture3 = render_texture2;
    render_texture2 = render_texture;
    render_texture  = tmp;

    //sleep(1);
}


int main(int argc, char** argv) {
    //alsa
    fprintf(stderr, "\n==================== dings ====================\n");
    int err;
    if ( (err = snd_pcm_open(&handle, argv[1], SND_PCM_STREAM_CAPTURE, 0 )) < 0 ) { 
        fprintf(stderr, "cannot open audio device %s (%s)\n", argv[1], snd_strerror (err)); exit (1); }

    alsa_setpar( handle,  argv[1] );

    fprintf(stderr, "\nusing read buffer size %d\n", _buflen);
    fprintf(stderr, "this results in frequency resolution of %d\n", _nfreq);
    fprintf(stderr, "we map them to %d energy bands\n\n", _nbands);

    //fft
    plan            = fftw_plan_dft_r2c_1d(_buflen, x[0], X, FFTW_MEASURE);
    //GL
    glutInit(&argc, argv);
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

    fprintf(stderr, "GL Version: %s\n", glGetString(GL_VERSION));
    //  fprintf(stderr, "GL Extensions:\n %s\n", glGetString(GL_EXTENSIONS));

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

//  dreieck.init             ("dreieck_vert.gl"     ,"dreieck_frag.gl");
    init_quad.init           ("quad_pass_through.gl", "slotted_disc.gl");
    post_processing_quad.init("quad_pass_through.gl", "postprocess.gl");
    dgl_tmp_quad.init        ("quad_pass_through.gl", "active_fragment_shader.gl");

    //  fanstern.init();

    // start reading from capture device and do fft in own thread
    pthread_t audio_thread;
    int audio_ret   = pthread_create( &audio_thread, NULL, do_fft, NULL );

    // turn vsync off. seems to not work with nvidia. needs to be turned off in driver
    //  glXSwapIntervalEXT(-1); 
    //  glXSwapIntervalEXT(0);
    //  glXSwapIntervalSGI(0);
    _t0 = glutGet(GLUT_ELAPSED_TIME);
    glutMainLoop();

    //exit
    fftw_destroy_plan(plan);
    
    snd_pcm_close(handle);

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
            //          fanstern.reinit_shaders();
            //          dreieck.recompile_shaders();
            init_quad.recompile_shaders();
            post_processing_quad.recompile_shaders();
            dgl_tmp_quad.recompile_shaders();
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

