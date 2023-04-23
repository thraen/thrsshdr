#pragma once


#include "globals.h"
// #include "matrix.h"

#include <GLFW/glfw3.h>

typedef struct Ui_State {
    int w;
    int h;
    int not_quit;
    int mouse_mode; /// fuck enum

    float *sample;

    int tmp;
} Ui_State;

void init_ui_state( Ui_State *ui,
                    float *sample );

void on_mouse_move( GLFWwindow* win,
                    double x,
                    double y );

void on_mouse_scroll( GLFWwindow* win,
                      double xoffset,
                      double yoffset );

void on_mouse_button( GLFWwindow* win,
                      int button,
                      int action,
                      int mods );

void on_key( GLFWwindow* win,
             int key,
             int scancode,
             int action,
             int mods );

