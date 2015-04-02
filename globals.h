#include "initalsa.h"

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <fftw3.h>

//#define _buflen 8192
// #define _buflen 4096
// #define _buflen 2048
#define _buflen 1024
// #define _buflen 512

#define _nfreq (_buflen/2 +1)  // immer mit Klammer!
// #define _nfreq 513 // besser

#define _nbands 400
// #define _nbands 40

extern unsigned int		_frame_t;//gleitender durchschnitt
// float 		frame_usec2=1000;//gleitender durchschnitt
extern unsigned int		_elapsed_t; 
extern unsigned int		_n_frames; 

extern unsigned int		_w;
extern unsigned int		_h;

//alsa
extern snd_pcm_t		*handle;
extern double			*x[2]; 		// two channels of real data
// extern double			**x; 		// two channels of real data
//fft
extern fftw_plan		plan;


extern fftw_complex			X[_nfreq];
// extern fftw_complex			*X;

//predefined energy bands
extern double				E[_nbands];
// extern double				*E;

extern float 				E_gesamt;
extern float				low;
extern float				mid;
extern float				hig;

extern GLuint 				framebuffer;
extern GLuint 				render_texture;
extern GLuint 				render_texture2;
extern GLuint 				render_texture3;

float sum( double *arr, int from, int till );

void  init_texture(GLuint text, unsigned int w, unsigned int h);
void  setup_render_texture(GLuint text, unsigned int w, unsigned int h);
void  resize_render_texture(GLuint render_texture, int w, int h);
char* readFile(const char *fn);
void  add_shader(GLuint shader_program, const char* pShaderText, GLenum ShaderType);
void  remove_shaders(GLuint shader_program);
