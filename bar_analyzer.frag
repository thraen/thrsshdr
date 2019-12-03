
float timef( int p ) {
    return 0.5+0.5*sin(_elapsed_t *0.001/p);
}

#define ARR normX
float wid = 1/(1.0*_nfreq);
const float off = 0.01;
const float hscale = 20;

// #define ARR E
// float wid = 1/(1.0*(_nband+2));
// float off = -0.0;
// const float hscale = 0.1;

void main() {
    color = vec4( 0.0, 0.0, 0.0, 1.0 );

    color = 
     + 0.1*  prev( 0 , 1) * vec4( timef(5), timef(71), timef(19), 1.0 )
     + 0.1* pprev(-1 , 1) * (1+tc.x)
    ;

    int k = int(floor((tc.x-off)/wid));

    if ( hscale*log(ARR[k]+1) > tc.y && k < _nfreq && k > 0) // zeroth freq is constant -> mean
        color = vec4( 1.0, 1.0, 1.0, 1.0 + sht);

// 	color *= 0.0;
}
