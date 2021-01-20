#line 2

#define ARR labsX
float W = 50/(1.0*_nfreq);
float H = 20/(1.0*_nfreq);
const float off = 0.01;

// #define ARR E
// float W = 1/(1.0*(_nband+2));
// float off = -0.0;

// float t    = 0.001*(_elapsed_t);
float wt      = (0.0001*(_elapsed_t%10000));
// float sint = ssin(t);

void main() {
    int k = int( floor(tc.x/W) );

    float e = log(ARR[k]+1) + 0.002*rand(_elapsed_t*0.001);

    float c = 10*e;

    float y_ = abs(1-tc.y-wt);

    if ( y_-H < 0 && k < _nfreq && k > 0) // zeroth freq is constant -> mean
        color = vec4( c, c, c, 1 );
    else
        color = prev_mod(0, 0);
}
