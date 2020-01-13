#include <GL/glew.h>
#include <GL/freeglut.h>

#include "globals.h"

struct Vector3f
{
    float x;
    float y;
    float z;

    Vector3f() {}

    Vector3f(float _x, float _y, float _z) {
        x = _x;
        y = _y;
        z = _z;
    }
};

class Rect {
    private:
        GLuint vbo;
        GLuint vao;
        GLuint shader_program;

        const char *vert_src_name;
        const char *frag_src_name;

        GLuint _labsX_;
        GLuint _nfreq_;

        GLuint _E_;
        GLuint _nband_;

        GLuint _Ecoarse_;

        GLuint _elapsed_t_;
        GLuint _w_;
        GLuint _h_;

        // textures
        GLuint u_now_;
        GLuint u_prv_;  

        void setup_draw( GLuint shader_program, GLuint vao );
        void cleanup_draw( );

    public:
        void init( const char *vert_src_name, const char *frag_src_name, bool assert_uniform );
        void recompile_shaders( bool assert_uniform );
        void set_global_uniforms();

        void draw();
        void draw( GLuint texture );
        void draw( GLuint tex1, GLuint tex2 );

};


