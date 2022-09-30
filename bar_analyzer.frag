// Pergolesi׃ Stabat mater, for 

float timef( int p ) {
    return 0.5+0.5*sin(_elapsed_t *0.001/p);
}

// #define ARR labsX
// float W = 1/(1.0*_nfreq/3);
// const float hscale = 20;

#define ARR labsX
float W = 1/(0.3*_nfreq);
const float hscale = 100;

// #define ARR E
// float W = 1/(1.0*(_nband+1));
// const float hscale = 0.1;

void main() {
    color = vec4( 0, 0, 0, 1 );

//     color = 
//      + 0.7*  prev( 0 , 1) * vec4( timef(5), timef(71), timef(19), 1.0 )
//      + 0.2* pprev( 0 , 1) * (1+tc.x)
//     ;

    int k = int(floor((tc.x+W)/W)); // +W cuts out the zeroth frequency bin. it's constant mean anyways

    if ( hscale*ARR[k] > tc.y && k < _nfreq)
//         color = 0.5*vec4( 1, 1, 1, 1 )+0.3*prev(0,0)+0.2*pprev(0,0);
        color = vec4( 1, 1, 1, 1 );

// 	color *= 0.0;
}
