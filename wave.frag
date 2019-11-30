#version 450 core

in vec2 cc;

uniform sampler2D u_now;
uniform sampler2D u_prv;

uniform int _w;
uniform int _h;
uniform int _elapsed_t;

uniform float low;
uniform float mid;
uniform float hig;

uniform float E[6];
uniform int _nband;

// uniform float normX[1024];
// uniform int _nfreq;

float sc = 1;

float dx = 1;

float dt = 0.005;

float r  = dt/(dx*dx);

float damp = 0.99;
// float damp = 1;

out vec4 color;

// float low = E[0];
// float mid = E[3];
// float hig = E[5];

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

void main() {
	float d = cc.x*cc.x*3*(low) +cc.y*cc.y*9*(mid+hig);

    // approx wave equation middle time, middle space
	color = r*(  prev(-dx, 0) + prev( dx,0) + prev(0,-dx) + prev(0, dx) )

		  // add diffusion
		  //+ (1-4*r)*texelFetch( u_now, ivec2(     gl_FragCoord.x   ,         gl_FragCoord.y      ), 0 );

		  + (2-4*r)* prev( 0, 0 )
		  -         pprev( 0, 0 );

	//if (!(abs(cc.x-0.8)>0.005) && abs(cc.y-0.8)>0.04 && abs(cc.y-0.6)>0.04) {
		//color = vec4( 0, 0, 0, 1);
	//}

	if (d<0.00050){ 
		color = vec4( 10*low, 10*mid, 10*hig, 1.0 + 0.00000001*_nband*E[1]*_elapsed_t);
	}

	color *= damp;
}
