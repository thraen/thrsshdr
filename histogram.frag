#line 2

#define ARR labsX
// #define ARR smoothf
// #define ARR difff
// #define ARR maxf

float W = 1/(1.0*_nfreq);
// float W = 1/(1.0*_nfreq);
float H = 0.04;

// #define ARR E
// float W = 1/(1.0*(_nband+2));

// float t    = 0.001*(_elapsed_t);
int scroll_period = 4000;
float wt      = (1/ ((1.0) *scroll_period))*(_elapsed_t%scroll_period);
// float sint = ssin(t);

void main() {
    int k = int(floor((tc.x+W)/W)); // +W cuts out the zeroth frequency bin. it's constant mean anyways

//     float e = log(ARR[k]+1); ///+ 0.002*rand(_elapsed_t*0.001);
    float e = ARR[k]; ///+ 0.002*rand(_elapsed_t*0.001);

//     float c = 80*log(k+1)*e; //? why does that work?
//     float c = k*log(k+10)*e; //? why does that work?
//     float c = 3*(k+1)*e; //? why does that work?
    float c = 0.5*((k+1)*log(k+1)+1)*e; //? why does that work?
//     float c = 60*e;

    float y_ = abs(1-tc.y-wt);

    if ( y_-H < 0 && k < _nfreq) // zeroth freq is constant -> mean
        color = vec4( c, c, c, 1 );
    else
        color = prev_mod(0, 0);
}
