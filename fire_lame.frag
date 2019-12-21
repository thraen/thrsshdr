#line 2

#define ARR normX
float W = 20/(1.0*_nfreq);
float H = 20/(1.0*_nfreq);
const float off = 0.01;

// #define ARR E
// float W = 1/(1.0*(_nband+2));
// float off = -0.0;

float ts = 1000;
float t = (0.0001*(_elapsed_t%10000));

void main() {
    int k = int( floor( (tc.x)/W ) );
//     int j = int( floor( (tc.y)/H ) );

    float e = log(ARR[k]+1);

    float v = 1+1*e;
    float c = 80*e;

    color = 
     + 0.97*  prev( 0 , -1.0*v)
     + 0.005* pprev( 0 , -1.0)
    ;

    if ( abs(tc.y) < H && k < _nfreq && k > 0) // zeroth freq is constant -> mean
        color = vec4( c, c, c, 1 );

//     else
//         color = vec4( 0, 0, 0, 1 );

//     color *=0.9;
}
