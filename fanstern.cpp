class Fanstern: Things{
    public:
    void reinit_shaders();
    void init();
    void draw();
};

void Fanstern::reinit_shaders(){
    char* vert_src = readFile("fanstern_vert.gl");
    char* frag_src = readFile("fanstern_frag.gl");

    shader_program = glCreateProgram();
    if (shader_program == 0) { fprintf(stderr, "Error creating shader program\n"); exit(1); }
    recompile_shaders(shader_program, vert_src, frag_src);
}

void Fanstern::init() {
//     Vector3f Vertices[_nbands+1];
    Vector3f Vertices[52];
    int i;
    Vertices[0] = Vector3f(0.0f, 0.0f,  0.0f);
    for (i=1; i<=52; i++){
//      float x = cosf((i-1)*2*M_PI/_nbands);
//      float y = sinf((i-1)*2*M_PI/_nbands);
        float x = cosf((i-1)*2*M_PI/50);
        float y = sinf((i-1)*2*M_PI/50);
        Vertices[i] = Vector3f( x, y, 0.0f );
//      fprintf( stderr, "%d %f %f\n", i, x, y);
    }
    glGenBuffers(1, &vbo);
    fprintf(stderr, "fanstern init. %o\n", vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
    //     .. und so wird 's dann gemalt, siehe render
    //     glDrawArrays(GL_TRIANGLE_FAN, 0, 52);
    reinit_shaders();
}

void Fanstern::draw(){
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glUseProgram(shader_program);

    set_global_uniforms();

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

//     glDrawArrays(GL_TRIANGLE_FAN, 0, _nbands+1);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 52);

    glDisableVertexAttribArray(0);
}

