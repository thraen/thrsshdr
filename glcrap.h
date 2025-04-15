#pragma once

#include <GL/gl.h>

void  init_texture(GLuint text, unsigned int w, unsigned int h);

typedef struct {
    GLuint program;
    const char *src_name;
} Cshdr;

typedef struct {
    GLuint program;

    const char *vert_src_name;
    const char *frag_src_name;

    // these are uniforms referencing textures
    GLuint u_now_;
    GLuint u_prv_;  
} Shdr;

void init_rect();

void init_texture( GLuint text,
                   unsigned int w,
                   unsigned int h );

void init_shdr( Shdr *,
                const char *vsrc_name,
                const char *fsrc_name,
                int assert_uniform );

void init_compute_shdr( Cshdr *s,
                        const char *src_name,
                        int assert_uniform );

void recompile_shaders( Shdr *,
                        int assert_uniform );

void recompile_compute_shader( Cshdr *,
                               int assert_uniform );

void init_shared_uniforms( GLuint program );
void set_block_uniforms();

void compute( Cshdr * );

void draw0( Shdr * );
void draw1( Shdr *, GLuint texture );
void draw2( Shdr *, GLuint tex1, GLuint tex2 );
