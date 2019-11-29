#version 400

in vec2 cc;

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

float dt = 0.05;

float r  = dt/(dx*dx);		

float a1 =  -0.1;
float a2 =  -0.2;

float t  = float(_elapsed_t);

float damp = 0.99999999999999;
//float damp = 1;

vec4 fluss_naiv(ivec2 x1, ivec2 x2,  ivec2 y1,  ivec2 y2){
	vec4 X1 = texelFetch(u_now, x1, 0);
	vec4 X2 = texelFetch(u_now, x2, 0);
	vec4 Y1 = texelFetch(u_now, y1, 0);
	vec4 Y2 = texelFetch(u_now, y2, 0);

	return 0.5*( a1*(X1+X2) + a2*(Y1+Y2) );
}

void main(){
	float d = cc.x*cc.x*mid +cc.y*cc.y*10*low;

	// hitze-gleichung 2xforward
	color = r* (    texelFetch( u_now, ivec2( mod(gl_FragCoord.x-dx,_w),     gl_FragCoord.y      ), 0 )
		  +			texelFetch( u_now, ivec2( mod(gl_FragCoord.x+dx,_w),     gl_FragCoord.y      ), 0 )
		  +			texelFetch( u_now, ivec2(     gl_FragCoord.x       , mod(gl_FragCoord.y-dx,_h)), 0 )
		  +			texelFetch( u_now, ivec2(     gl_FragCoord.x       , mod(gl_FragCoord.y+dx,_h)), 0 )
		  )
		  //+ (1-4*r)*texelFetch( u_now, ivec2(     gl_FragCoord.x   ,         gl_FragCoord.y      ), 0 )

		  // bessere Appoximation fuer Laplace(u)
 		  +	r* (    texelFetch( u_now, ivec2( mod(gl_FragCoord.x-dx,_w), mod(gl_FragCoord.y+dx,_h)), 0 )
 		  +			texelFetch( u_now, ivec2( mod(gl_FragCoord.x+dx,_w), mod(gl_FragCoord.y-dx,_h)), 0 )
 		  +			texelFetch( u_now, ivec2( mod(gl_FragCoord.x-dx,_w), mod(gl_FragCoord.y-dx,_h)), 0 )
 		  +			texelFetch( u_now, ivec2( mod(gl_FragCoord.x+dx,_w), mod(gl_FragCoord.y+dx,_h)), 0 )
 		  )
 		  + (1-8*r)*texelFetch( u_now, ivec2(     gl_FragCoord.x   ,         gl_FragCoord.y      ), 0 );

	if (d<0.00050) {
		color = 0.1*log(low+1)*vec4( 0.4*log(float(E[1])+1), 0.4*log(float(E[5]+1)), 0.4*log(float(E[10]+1)), 1)*(1-100*d);
		color = 0.1*log(low+1)*vec4( 0, 0, 1, 1)*(1-100*d);
		color = vec4( 0, 0.0005+50*d*sin(0.01* (float(_elapsed_t)+0.000001*low) ), 0, 1);
		color = vec4( low, mid, hig, 1);
	}

	color *= damp;
}
