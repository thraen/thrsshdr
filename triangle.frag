#version 450 core

out vec4 color;

in vec2 cc;
in vec2 tc;

uniform sampler2D u_now;
uniform sampler2D u_prv;

uniform float mid;
uniform float hig;
uniform float low;

uniform int _w;
uniform int _h;
uniform int _elapsed_t;

uniform float E[9];
uniform int _nband;

void triangle() {
    float s = E[1];
    if ( abs(cc.x) < 0.5*(0.5-cc.y) ) {
        color = vec4(E[1],E[3],E[5],1);
    }
    else
        color = vec4(0,0,0,1);

}

void main(){
    triangle();
}
