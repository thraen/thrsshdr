
float dx = 1;

float dt = 0.005;

float r  = dt/(dx*dx);

const float damp = 0.998;

void main() {
    float d = cc.x*cc.x*(low) +cc.y*cc.y*(mid+hig);

    // approx wave equation middle time, middle space
	color = r*(  prev_mod(-dx, 0) + prev_mod( dx,0) + prev_mod(0,-dx) + prev_mod(0, dx) )

		  //// add diffusion
// 		  + (1-4*r)*texelFetch( u_now, ivec2(     gl_FragCoord.x   ,         gl_FragCoord.y      ), 0 );

		  + (2-4*r)* prev_mod( 0, 0 )
		  -         pprev_mod( 0, 0 );

	if (d<0.00005){ 
		color = vec4( 5*low, 5*mid, 5*hig, 1);
	}

	color *= damp;
}
