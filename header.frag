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

uniform float E[_nband];
uniform float absX[_nfreq];

double sht = 0.000001*(_elapsed_t + _w + _h + low + mid + hig + E[0] + _nband + absX[0] + _nfreq);

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

float rand(vec2 co) {
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

// xxx does that work
float rand(float f) {
    return fract(sin(f*(12.9898)) * 43758.5453);
}

// xxx does that owrk? 
float rand(vec3 co) {
    return fract(sin(dot(co ,vec3(12.9898,78.233,39.2))) * 43758.5453);
}

float ssin(float x) {
    return 0.5+0.5*sin(x);
}
