#version 430

layout(location = 0) in vec2 pos;

uniform mat3 VM;

layout(shared, binding = 0) uniform VM_ {
    uniform int asdf;
};

out vec2 uv;

void main() {

    vec3 p = VM * vec3(pos, 1);
    gl_Position = vec4(p.x, p.y, 0, 1);

    uv = pos.xy;
}

