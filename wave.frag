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

uniform double E[400];

float sc = 1;

float dx = 1;

// float dt = 0.24;// fuer die Diffusionsgleichung
// float dt = 0.25;// hier wird's instabil
// float dt = 0.26;

// float dt = 0.25;

float dt = 0.005;

float r  = dt/(dx*dx);

float damp = 0.99999999;
// float damp = 1;

void main(){
	float d = (uv.x-0.5)*(uv.x-0.5)*5*low +(uv.y-0.5)*(uv.y-0.5)*1*(mid+hig);

// 	// Wellengleichung middle time middle space
	color = r* (    texelFetch( u_now, ivec2( mod(gl_FragCoord.x-dx,_w),     gl_FragCoord.y      ), 0 )
		  +			texelFetch( u_now, ivec2( mod(gl_FragCoord.x+dx,_w),     gl_FragCoord.y      ), 0 )
		  +			texelFetch( u_now, ivec2(     gl_FragCoord.x       , mod(gl_FragCoord.y-dx,_h)), 0 )
		  +			texelFetch( u_now, ivec2(     gl_FragCoord.x       , mod(gl_FragCoord.y+dx,_h)), 0 )
		  )

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
		color = vec4( low, mid, hig, 1);
	}

	color *= damp;
}
