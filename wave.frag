
float sc = 1;

float dx = 1;

float dt = 0.005;

float r  = dt/(dx*dx);

float damp = 0.99;

float _low = 2*log(100*E[1] + 100*E[2] + E[3] + 100*E[4] + E[5] + E[6] + E[7] +1);
float _mid = 2*log(E[6] + E[7] +1);
float _hig = 2*log(E[7] + E[8]+1);

void main() {
    float d = cc.x*cc.x*(_low) +cc.y*cc.y*(_mid+_hig);

    // approx wave equation middle time, middle space
	color = r*(  prev_mod(-dx, 0) + prev_mod( dx,0) + prev_mod(0,-dx) + prev_mod(0, dx) )

		  // add diffusion
		  //+ (1-4*r)*texelFetch( u_now, ivec2(     gl_FragCoord.x   ,         gl_FragCoord.y      ), 0 );

		  + (2-4*r)* prev_mod( 0, 0 )
		  -         pprev_mod( 0, 0 );

	//if (!(abs(cc.x-0.8)>0.005) && abs(cc.y-0.8)>0.04 && abs(cc.y-0.6)>0.04) {
		//color = vec4( 0, 0, 0, 1);
	//}

	if (d<0.00050){ 
		color = 0.8*vec4( _low,_mid,_hig, 1.0 + sht )
            + 0.1 * prev(0, 0)
            + 0.1 * pprev(0, 0);
	}

	color *= damp;
}
