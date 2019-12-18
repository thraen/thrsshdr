
float timef( int p ) {
    return 0.5+0.5*sin(_elapsed_t *0.001/p);
}

#define ARR normX
float W = 1/(1.0*_nfreq/3);
// float W = 1/(1.0*_nfreq/12);
const float off = 0.01;
const float hscale = 1;

// #define ARR normX
// float W = 1/(1.0*_nfreq);
// const float off = 0.01;
// const float hscale = 20;

// #define ARR E
// float W = 1/(1.0*(_nband+2));
// float off = -0.0;
// const float hscale = 0.1;

void main() {
    color = vec4( 0, 0, 0, 1 );

    color = 
     + 0.7*  prev( 0 , 1) * vec4( timef(5), timef(71), timef(19), 1.0 )
     + 0.2* pprev( 0 , 1) * (1+tc.x)
    ;

    int k = int(floor((tc.x-off)/W));

    if ( hscale*log(ARR[k]+1) > tc.y && k < _nfreq && k > 0) // zeroth freq is constant -> mean
        color = vec4( 1, 1, 1, 1 );

    //// snowy thing
//     if ( abs(100*log(ARR[k]+1) - tc.y) < H && k < _nfreq && k > 0) // zeroth freq is constant -> mean
//         color = vec4( 1, 1, 1, 1 );

// 	color *= 0.0;
}
