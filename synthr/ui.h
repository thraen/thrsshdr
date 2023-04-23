#pragma once

#include "globals.h"
// #include "matrix.h"

#include <GLFW/glfw3.h>

typedef struct Ui_State {
    int w;
    int h;
    int not_quit;
    int mouse_mode; /// fuck enum

    float pitch;
    int tmp;
} Ui_State;

static
void init_ui_state(Ui_State *ui) {
    *ui = (Ui_State) {
        .not_quit    = 1,
        .mouse_mode  = 0,
        .pitch       = 0.0f,
        .tmp         = 0,
    };
}

static double last_x_ = -1;
static double last_y_ = -1;

static
void on_mouse_move( GLFWwindow* window,
                    double x, double y )
{
    Ui_State *ui = (Ui_State *) glfwGetWindowUserPointer(window);

//     dbg("last %4.1f, %4.1f\n", last_x_, last_y_);
//     dbg(" now %4.1f, %4.1f\n", x, y);

    if (ui->mouse_mode == 1) {
        double dx = (last_x_ - x); // / (float) ui->w;
        double dy = (last_y_ - y); // / (float) ui->h;
        ui->pitch += dy;
    }
    last_x_ = x;
    last_y_ = y;
}

static
void on_mouse_scroll( GLFWwindow* window,
                      double xoffset, double yoffset )
{
    Ui_State *ui = (Ui_State *) glfwGetWindowUserPointer(window);
}

static
void on_mouse_button( GLFWwindow* window,
                      int button, int action, int mods )
{
    Ui_State *ui = (Ui_State *) glfwGetWindowUserPointer(window);
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        /// fuck make an enum
        ui->mouse_mode = 1;

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        //         if (glfwRawMouseMotionSupported())
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);//fuck: this isn't disabled. so we can also set it up once at init?
    }
    else {
        ui->mouse_mode = 0;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        // Next line is because after glfwSetInputMode(..NORMAL), the next 
        // x, y mouse event will be at the point of the visual cursor. 
        // This will be the place where the cursor was last seen - generally 
        // different from last_x set with GLFW_CURSOR_DISABLED.
        glfwGetCursorPos(window, &last_x_, &last_y_);
    }
}

static
void on_key( GLFWwindow* window,
             int key, int scancode, int action, int mods )
{
    Ui_State *ui = (Ui_State *) glfwGetWindowUserPointer(window);
    if ((key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE) && action == GLFW_PRESS)
        ui->not_quit = 0;
        
    if (key == GLFW_KEY_E  &&  action != GLFW_RELEASE)
      nfo("e");
    if (key == GLFW_KEY_D  &&  action != GLFW_RELEASE) // NOLINT
      nfo("d");
    if (key == GLFW_KEY_T  &&  action != GLFW_RELEASE) // NOLINT
      nfo("t");
    if (key == GLFW_KEY_SPACE) //NOLINT
      nfo("space");

    if (key == GLFW_KEY_O) {
        ui->tmp++;
        dbg(" ui->tmp %d ", ui->tmp );
    }

    if (key == GLFW_KEY_I) {
        ui->tmp--;
        dbg(" ui->tmp %d ", ui->tmp );
    }

}

