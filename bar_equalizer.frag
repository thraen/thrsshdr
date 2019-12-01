
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
