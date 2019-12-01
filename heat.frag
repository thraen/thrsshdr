

float dx = 1;

float dt = 0.05;

float r  = dt/(dx*dx);		

float a1 =  -0.1;
float a2 =  -0.2;

float t  = float(_elapsed_t);

float damp = 0.99999999999999;

void main(){
	float d = cc.x*cc.x*mid +cc.y*cc.y*10*low;

	// heat equation
	color = r* (    texelFetch( u_now, ivec2( mod(gl_FragCoord.x-dx,_w),     gl_FragCoord.y      ), 0 )
		  +			texelFetch( u_now, ivec2( mod(gl_FragCoord.x+dx,_w),     gl_FragCoord.y      ), 0 )
		  +			texelFetch( u_now, ivec2(     gl_FragCoord.x       , mod(gl_FragCoord.y-dx,_h)), 0 )
		  +			texelFetch( u_now, ivec2(     gl_FragCoord.x       , mod(gl_FragCoord.y+dx,_h)), 0 )
		  )
		  //+ (1-4*r)*texelFetch( u_now, ivec2(     gl_FragCoord.x   ,         gl_FragCoord.y      ), 0 )

		  // 9 sample stencil approximation for Laplace(u)
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
