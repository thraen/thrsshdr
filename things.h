#include <GL/glew.h>
#include <GL/freeglut.h>

#include "math_3d.h"
#include "globals.h"

class Things{
    protected:
        GLuint vbo;
        GLuint vao;
        GLuint shader_program;

        const char *vert_src_name;
        const char *frag_src_name;

        GLuint _low_;
        GLuint _mid_;
        GLuint _hig_;
        GLuint _E_;
        GLuint _elapsed_t_;
        GLuint _w_;
        GLuint _h_;

    public:
        void recompile_shaders();
        void set_global_uniforms();

        virtual void init(const char *vert_src_name, const char *frag_src_name) = 0;
        virtual void draw()                                                     = 0;
};

class Dreieck : public Things{
    public:
        void init(const char *vert_src_name, const char *frag_src_name);
        void draw();
};

class Quad : public Things{
    protected:
        // textures
        GLuint u_now_;
        GLuint u_prv_;  
    public:
        void init(const char *vert_src_name, const char *frag_src_name);
        void draw();
        void draw(GLuint texture);
        void draw(GLuint tex1, GLuint tex2);
};

