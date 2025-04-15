#line 2

#define ARR labsX
float W = 10;
float H = 10;

// #define ARR E
// float W = 1/(1.0*(_nband+2));
// float off = -0.0;

float t    = 0.001*(_elapsed_t);

float elip( float a, float b, float x, float y ) {
    return sqrt( (a*x)*(a*x) + (b*y)*(b*y) );
}

float elip( float a, float b, vec2 v ) {
    return sqrt( (a*v.x)*(a*v.x) + (b*v.y)*(b*v.y) );
}

const float scx = 10;

float thr( float x ) {
    if ( x > 0.01) 
        return x;
    else
        return 0;
}

void main() {

    float t_ = prev(0,0).a + 0.0001+ 0.8*thr(E[1]);

//     int k = int( floor(tc.x/20) );

//     float a = 1/cos(t);
//     float b = 1/sin(t);
    
    float a = 1/scx;
    float b = 1/sin(t_);

    float x = sin(5*cc.x-t_);
    float y = cc.y-0.3*sin(10*cc.x-t/10);

    float dx = 1;
    float dy = sin(t);

    if ( elip( a, b, x, y ) < 0.05 )
        color = vec4(1,2*tc.y,2*tc.x,t_);
    else 
//         color.rgb = 0.94*prev(dx, dy).rgb;
//         color.a = t_;
        color = vec4(0,0,0,t_);


//     vec2 c = 0.2*vec2(sin(t), cos(t));
//     if ( elip( 1/cos(t), 1/sin(t), c - cc ) < 0.05 )
//         color = vec4(1,1,1,1);
//     else 
//         color = vec4(0,0,0,0);

}
