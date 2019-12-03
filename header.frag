#version 450 core

in vec2 cc;
in vec2 tc;

uniform int _w;
uniform int _h;

uniform sampler2D u_now;
uniform sampler2D u_prv;

out vec4 color;

uniform float mid;
uniform float hig;
uniform float low;

uniform int _elapsed_t;

uniform int _nband;
uniform float E[9]; // XXX define

uniform float normX[513]; // XXX define
uniform int _nfreq;

double sht = 0.000001*(_elapsed_t + _w + _h + low + mid + hig + E[0] + _nband + normX[0] + _nfreq);

vec4 prev( float dx, float dy ) {
    return texelFetch( u_now, 
                       ivec2( gl_FragCoord.x+dx, gl_FragCoord.y+dy ),
                       0 );
}

vec4 pprev( float dx, float dy ) {
    return texelFetch( u_prv, 
                       ivec2( gl_FragCoord.x+dx, gl_FragCoord.y+dy ),
                       0 );
}

vec4 prev_mod( float dx, float dy ) {
    return texelFetch( u_now, 
                       ivec2( mod(gl_FragCoord.x+dx,_w), mod(gl_FragCoord.y+dy,_h) ),
                       0 );
}

vec4 pprev_mod( float dx, float dy ) {
    return texelFetch( u_prv, 
                       ivec2( mod(gl_FragCoord.x+dx,_w), mod(gl_FragCoord.y+dy,_h) ),
                       0 );
}
