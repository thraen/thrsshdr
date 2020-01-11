#line 2

#define ARR absX
float W = 20/(1.0*_nfreq);
float H = 20/(1.0*_nfreq);
const float off = 0.01;

// #define ARR E
// float W = 1/(1.0*(_nband+2));
// float off = -0.0;

float ts = 1000;
float t = (0.0001*(_elapsed_t%10000));

void main() {
    int k = int( floor(tc.x/W) );

    float e = log(ARR[k]+1) + 0.002*rand(_elapsed_t*0.001);

    float vy = 0.8+40*e;
//     float vx = 1.8*sin(_elapsed_t*0.01*e);
    float vx = 0;
    float c = 80*e;

    color = 
     + 0.97*   prev( vx , 1.0*vy)
     + 0.005* pprev( vx , 1.0)
//      + 0.97*  prev( 0 , 1.0*vy)
//      + 0.005* pprev( 0 , 1.0)
    ;

    if ( abs(tc.y - 8*rand(vec2(k,_elapsed_t*0.001))) < H && k < _nfreq && k > 0) // zeroth freq is constant -> mean
        color = vec4( c, c, c, 1 );

//     else
//         color = vec4( 0, 0, 0, 1 );

//     color *=0.9;
}
