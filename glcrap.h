#ifndef GLCRAP_H
#define GLCRAP_H


void  init_texture(GLuint text, unsigned int w, unsigned int h);

void  setup_render_texture(GLuint text, unsigned int w, unsigned int h);

void  resize_render_texture(GLuint render_texture, int w, int h);

char* read_file(const char *fn);

void  add_shader(GLuint program, size_t srcc, const char **srcv, GLenum shader_type);

void  remove_shaders(GLuint program);

GLuint uniform_loc(GLuint program, const char* name, int assert_uniform);

GLuint block_offset(GLuint program, GLuint interface_type, const char* name);

static GLuint vbo;
static GLuint vao;
static GLuint ubo;
static GLuint ssbo;

// offsets within a block of respective uniforms that are shared by all shaders
// the layout of said block is defined to be 'shared', so offsets are guaranteed 
// to be the same in all shaders declaring it. 
static GLuint w_;
static GLuint h_;
static GLuint elapsed_t_;
static GLuint labsX_;
static GLuint E_;
static GLuint Ecoarse_;

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

void init_shdr(Shdr *, const char *vsrc_name, const char *fsrc_name, int assert_uniform);
void init_compute_shdr( Cshdr *s, const char *src_name, int assert_uniform);

void recompile_shaders(Shdr *, int assert_uniform);
void recompile_compute_shader(Cshdr *, int assert_uniform);

void init_shared_uniforms(GLuint program);
void set_block_uniforms();

void compute(Cshdr *);

void setup_draw(Shdr *);
void draw0(Shdr *);
void draw1(Shdr *, GLuint texture);
void draw2(Shdr *, GLuint tex1, GLuint tex2);

#endif
