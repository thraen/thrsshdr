#version 330 core

out vec4 color;

in vec2 uv;

uniform sampler2D u_now;
uniform sampler2D u_prv;

uniform float mid;
uniform float hig;
uniform float low;

uniform int _w;
uniform int _h;
uniform int _elapsed_t;

uniform float E[6];
uniform int _nband;

void triangle() {
//     if ( 5*uv.x < abs(uv.y+0.5)) color = vec4(1,1,1,1);
    float s = E[1];
    if ( abs(uv.x-0.5) < (1-uv.y)/2 ) {
//     if ( s*abs(uv.x-0.5) < uv.y) {
//     if ( 0.01*h > uv.y ) {
        color = vec4(E[1],E[3],E[5],1);
//         color = vec4(0,0,1,1);
    }
    else
        color = vec4(0,0,0,1);

}

void main(){
    triangle();
}
