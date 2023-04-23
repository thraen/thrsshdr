#version 410

uniform vec4 color;

out vec4 out_color;

in vec2 uv;


void main() {
    float d_ = (length(uv) - 0.5) ;
    float d = 1 -  10*pow( d_ , 0.95) ;

    out_color = vec4(d,d,d, d) * color;
}
