#version 400

in vec2 uv;

uniform sampler2D u_now;
uniform sampler2D u_prv;

out vec4 color;

uniform float mid;
uniform float hig;
uniform float low;

uniform int _w;
uniform int _h;
uniform int _elapsed_t;

uniform float E[6];
uniform int _nband;

uniform float normX[1024];
uniform int _nfreq;

float bwid = 0.12;

double sht = 0.000001*(_elapsed_t + _w + _h + low + mid + hig);

vec4 prev( float dx, float dy ) {
    return texelFetch( u_now, 
                       ivec2( mod(gl_FragCoord.x+dx,_w), mod(gl_FragCoord.y+dy,_h) ),
                       0 );
}

vec4 pprev( float dx, float dy ) {
    return texelFetch( u_prv, 
                       ivec2( mod(gl_FragCoord.x+dx,_w), mod(gl_FragCoord.y+dy,_h) ),
                       0 );
}

float timef( int p ) {
    return 0.5+0.5*sin(_elapsed_t *0.01/p);
}

void main(){
//     color = vec4( low, mid, hig, 1.0);
//     float c = float(10000*E[5] + mid);
//     color = vec4( c, c, c, 1.0);
    color = vec4( 0.1*low, 0.1*mid, 0.1*hig, 1.0) 
     + 0.6*  prev(-3.0 /uv.x , +0.7) * vec4( timef(5), timef(71), timef(19), 1.0 )
     + 0.2* pprev(-3.0 /uv.x , +0.7) * (1+uv.x)
    ;

    for(int i=0; i<_nband; i++) {
        float h = E[i];
//         if (i == 0) h *= 3;
        if (0.01*h > uv.y && (abs( i*bwid - uv.x+0.15) < bwid) ) {
            color = vec4( 1.0, 1.0, 1.0, 1.0 + sht);
        }
    }
// 	color *= 0.8;
}
