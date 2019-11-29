#version 330 core

in vec2 uv;

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

// float dt = 0.24;// fuer die Diffusionsgleichung
// float dt = 0.25;// hier wird's instabil
// float dt = 0.26;

// float dt = 0.25;

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

void main() {
// 	float d = (uv.x-0.5)*(uv.x-0.5)*1*low +(uv.y-0.5)*(uv.y-0.5)*0.3*(mid+hig);
	float d = (uv.x-0.5)*(uv.x-0.5)*3*(low) +(uv.y-0.5)*(uv.y-0.5)*9*(mid+hig);

    // approx wave equation middle time, middle space
	color = r*(  prev(-dx, 0) + prev( dx,0) + prev(0,-dx) + prev(0, dx) )

		  // Diffusionsterm
		  //+ (1-4*r)*texelFetch( u_now, ivec2(     gl_FragCoord.x   ,         gl_FragCoord.y      ), 0 );

		  // Wellenterm
		  + (2-4*r)*texelFetch( u_now, ivec2(     gl_FragCoord.x   ,         gl_FragCoord.y      ), 0 )
		  -         texelFetch( u_prv, ivec2(     gl_FragCoord.x   ,         gl_FragCoord.y      ), 0 );

	//if (!(abs(uv.x-0.8)>0.005) && abs(uv.y-0.8)>0.04 && abs(uv.y-0.6)>0.04) {
		//color = vec4( 0, 0, 0, 1);
	//}

	if (d<0.00050){ //&&d>0.00045){
		//color = 0.1*log(low+1)*vec4( 0.4*log(float(E[1])+1), 0.4*log(float(E[5]+1)), 0.4*log(float(E[10]+1)), 1)*(1-100*d);
		//color = 0.1*log(low+1)*vec4( 0, 0, 1, 1)*(1-100*d);
		//color = vec4( 0, 0.0005+50*d*sin(0.01* (float(_elapsed_t)+0.000001*low) ), 0, 1);
		color = vec4( 10*low, 10*mid, 10*hig, 1.0 + 0.00000001*_nband*E[1]*_elapsed_t);
	}

	color *= damp;
}
