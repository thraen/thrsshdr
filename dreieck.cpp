
void Dreieck::init(const char *v_src_name, const char *f_src_name, bool assert_uniform){
    vert_src_name = v_src_name;
    frag_src_name = f_src_name;

    Vector3f Vertices[3];
    Vertices[0] = Vector3f(-1.0f, -1.0f,  0.0f);
    Vertices[1] = Vector3f( 1.0f, -1.0f,  0.0f);
    Vertices[2] = Vector3f( 0.0f,  1.0f,  0.0f);

    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);
    fprintf(stderr, "dreieck init. %o\n", vbo);

    shader_program = glCreateProgram();
    if (shader_program == 0) { fprintf(stderr, "Error creating shader program\n"); exit(1); }
    recompile_shaders(assert_uniform);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

//     glDisableVertexAttribArray(0);
}

void Dreieck::draw(){
    glUseProgram(shader_program);
    glBindVertexArray(vao);

    set_global_uniforms();

//     glEnableVertexAttribArray(0);
//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(GL_TRIANGLES, 0, 3);

//     glDisableVertexAttribArray(0);
}

