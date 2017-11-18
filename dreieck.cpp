
void Dreieck::init(const char *v_src_name, const char *f_src_name){
    vert_src_name = v_src_name;
    frag_src_name = f_src_name;

    Vector3f Vertices[3];
    Vertices[0] = Vector3f(-1.0f, -1.0f,  0.0f);
    Vertices[1] = Vector3f( 1.0f, -1.0f,  0.0f);
    Vertices[2] = Vector3f( 0.0f,  1.0f,  0.0f);

    glGenBuffers(1, &vbo);
    fprintf(stderr, "dreieck init. %o\n", vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    shader_program = glCreateProgram();
        if (shader_program == 0) { fprintf(stderr, "Error creating shader program\n"); exit(1); }
    recompile_shaders();
}

void Dreieck::draw(){
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glUseProgram(shader_program);

    set_global_uniforms();

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(0);
}

