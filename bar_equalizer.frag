#version 450

in vec2 cc;
in vec2 tc;

uniform sampler2D u_now;
uniform sampler2D u_prv;

out vec4 color;

uniform float mid;
uniform float hig;
uniform float low;

uniform int _w;
uniform int _h;
uniform int _elapsed_t;

uniform float E[9];
uniform int _nband;

uniform float normX[513];
uniform int _nfreq;


double sht = 0.000001*(_elapsed_t + _w + _h + low + mid + hig + E[0] + _nband);

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

float timef( int p ) {
    return 0.5+0.5*sin(_elapsed_t *0.01/p);
}

#define ARR normX
float wid = 1/(1.0*_nfreq);
const float off = 0.01;
const float hscale = 20;

// #define ARR E
// float wid = 1/(1.0*_nband);
// float off = 0.1;

void main() {
    color = vec4( 0.0, 0.0, 0.0, 1.0 );

    color = 
     + 0.6*  prev(-3.0 /tc.x , +0.3) * vec4( timef(5), timef(71), timef(19), 1.0 )
     + 0.2* pprev(-3.0 /tc.x , +0.3) * (1+tc.x)
    ;

    int k = int(floor((tc.x-off)/wid));

    if ( hscale*ARR[k] > tc.y && k < _nfreq )
        color = vec4( 1.0, 1.0, 1.0, 1.0 + sht);

// 	color *= 0.8;
}
