in vec2 cc;
in vec2 tc;

#define   pi 3.141592653589793
#define _2pi 6.283185307179586

uniform sampler2D u_now;
uniform sampler2D u_prv;

out vec4 color;

float low = Ecoarse[0];
float mid = Ecoarse[1];
float hig = Ecoarse[2];

// double sht = 0.000001*(_elapsed_t + _w + _h + low + mid + hig + E[0] + _nband + labsX[0] + _nfreq);

const vec4 neongreen = vec4(0.22, 1, 0.08, 1);

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
    return fract(sin(dot(co,vec2(12.9898,78.233))) * 43758.5453);
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

vec3 hsv2rgb(vec3 c) {
    vec3 p = abs(fract(c.xxx + vec3(0.0, 2.0/3.0, 1.0/3.0)) * 6.0 - 3.0);
    return c.z * mix(vec3(1.0), clamp(p - 1.0, 0.0, 1.0), c.y);
}

vec3 rainbow(float t) {
    // t in [0,1] -> rainbow color
    return hsv2rgb(vec3(t, 1.0, 1.0));
}

vec4 rainbow4(float t) {
    return vec4( rainbow(t), 1.0 );
//     return vec4( hsv2rgb(vec3(t, 1.0, 1.0)) , 1.0);
}
