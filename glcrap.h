#ifndef GLCRAP_H
#define GLCRAP_H

#include <GL/glew.h>
#include <GL/freeglut.h>

void  init_texture(GLuint text, unsigned int w, unsigned int h);

void  setup_render_texture(GLuint text, unsigned int w, unsigned int h);

void  resize_render_texture(GLuint render_texture, int w, int h);

char* read_file(const char *fn);

void  add_shader(GLuint shader_program, size_t srcc, const char **srcv, GLenum shader_type);

void  remove_shaders(GLuint shader_program);

GLuint uniform_loc(GLuint shader_program, const char* s, int assert_uniform);

GLuint vbo;
GLuint vao;

typedef struct {
    GLuint shader_program;

    const char *vert_src_name;
    const char *frag_src_name;

    GLuint labsX_;
    GLuint nfreq_;

    GLuint E_;
    GLuint nband_;

    GLuint Ecoarse_;

    GLuint elapsed_t_;
    GLuint w_;
    GLuint h_;

    // textures
    GLuint u_now_;
    GLuint u_prv_;  
} Shdr;

void init_rect();

void init_shdr(Shdr *, const char *vsrc_name, const char *fsrc_name, int assert_uniform);

void recompile_shaders( Shdr *, int assert_uniform );
void set_global_uniforms( Shdr * );

void setup_draw(Shdr *);

void draw0(Shdr *);
void draw1(Shdr *, GLuint texture);
void draw2(Shdr *, GLuint tex1, GLuint tex2);

#endif
