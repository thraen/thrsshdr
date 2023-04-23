#include "globals.h"

#include <GLFW/glfw3.h>

#include "synth.h"

#include "ui.h"

static
void on_glfw_error(int error, const char* description) {
    nfo("GLFW error: %s\n", description);
}

static
void on_framebuffer_resize(GLFWwindow* window, int width, int height) {
    Ui_State *up = (Ui_State *) glfwGetWindowUserPointer(window);
    up->w = width; up->h = height;
}

static 
GLFWwindow *init_glfw_window(Ui_State *ui) {
    dbg("init_glfw_window()\n");
    if (!glfwInit())
        err_exit("Cannot initialize GLFW.");
//     if (!glfwVulkanSupported())
//         err_exit("GLFW failed to find the Vulkan loader.");

    glfwSetErrorCallback(on_glfw_error);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); /// forbid resizing, see below
    GLFWwindow *win = glfwCreateWindow(600, 600, "blah", NULL, NULL);

    glfwSetWindowUserPointer(win, ui);
    glfwSetFramebufferSizeCallback(win, on_framebuffer_resize);
    glfwSetKeyCallback(win, on_key);
    glfwSetMouseButtonCallback(win, on_mouse_button);
    glfwSetCursorPosCallback(win, on_mouse_move);
    glfwSetScrollCallback(win, on_mouse_scroll);
    return win;
}

void draw() {};

GLFWwindow *window_;
void init_rendering(GLFWwindow *window) {};

PaError pa_err_ = 0;

void fuckoff() {
//     gl_cleanup();
    Pa_Terminate();
    if (pa_err_) {
        fprintf( stderr, "a port audio error occured:\n %s\n",
                 Pa_GetErrorText(pa_err_) );
    }

    glfwDestroyWindow(window_);
    glfwTerminate();
}

int main(int argc, char **argv) {
    int enable_validation = 1;

    Ui_State ui;

    init_ui_state(&ui);

    window_ = init_glfw_window(&ui); /// fuck does this have to come first?

    init_rendering(window_);

    /// pa crap
    PaStreamParameters pa_params;
    PaStream *stream;
    paTestData data;
    for( int i=0; i<TABLE_SIZE; i++ ) {
        data.sine[i] = (float) sin( ((double)i/(double)TABLE_SIZE) * M_PI * 2. );
    }
    data.left_phase = data.right_phase = 0;

    fuckoff_if((
        pa_err_ = Pa_Initialize()
    ));

    pa_params.device = Pa_GetDefaultOutputDevice(); 
    fuckoff_if(pa_params.device == paNoDevice);

    pa_params.channelCount = 2;
    pa_params.sampleFormat = paFloat32;
    pa_params.suggestedLatency = Pa_GetDeviceInfo( pa_params.device )->defaultLowOutputLatency;
    pa_params.hostApiSpecificStreamInfo = NULL;

    fuckoff_if(( 
        pa_err_ = Pa_OpenStream( &stream,
                                NULL,
                                &pa_params,
                                SAMPLE_RATE,
                                FRAMES_PER_BUFFER,
                                paClipOff,
                                patestCallback,
                                &data )
    ));
    
    fuckoff_if((
        pa_err_ = Pa_SetStreamFinishedCallback(stream, &stream_finished)
    ));

    fuckoff_if((
        pa_err_ = Pa_StartStream( stream )
    ));

    nfo("done initializing %f\n", glfwGetTime());

    while ( ui.not_quit && (!glfwWindowShouldClose(window_)) ) {
        glfwPollEvents(); 
        draw();
    }

    fuckoff_if((
        pa_err_ = Pa_StopStream( stream )
    ));

    fuckoff_if((
        pa_err_ = Pa_CloseStream( stream )
    ));

    fuckoff();
}
