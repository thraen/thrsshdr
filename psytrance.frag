#line 2


const int N = 10;
const int M = 10;

float t  = 0.001 *(_elapsed_t);
float wt = 0.0001 *(_elapsed_t % 10000);

void main() {
    color = vec4(1,1,1,1);

    float x = abs(cc.x); // why i need abs? something wrng
    float y = cc.y
        + (0.1* sin(t))
        ;

    float y_2 = y*y 
//         * (0.1+ ssin(t))
        ;
    float x_2 = x*x;
    float r = sqrt(x_2+y_2);

//     float t_ = mod(sumf[6], 3);
    float t_ = wt;
    float env = 1.0*exp( -200*(r-t_)*(r-t_));
    float pkg = 1.0*sin( ( r - t_ )/0.2 );

    float R = 1.8 +  env * pkg;
//     float R   = 1;
    float R_2 = R*R;

    float f = 1.5;

    float x_ = sqrt( R_2 / ( 1 + y_2/x_2 ) );
    float y_ = sqrt( R_2 / ( 1 + x_2/y_2 ) );

    float z = 1*wt + x_ / x /f;
    float a = atan(x_, y_);

    int j = int( floor(z*N) ); //% N; // i don't get why i need the -1
    int i = int( floor(a*M) ); // % M;
    float m = float( (i+j) %2 );

    float c = 5.1*r*r;
//     color = vec4(m, m, m, 1);
    color = vec4(m*c, m*c, m*c,1);

//     color = vec4(m,m,m,1);

}
