#line 2
// Legowelt-Dark Days Full Album 32.30


// const int  M = 8;
// const int  N = 18;
// const int[M][N] A = {
//     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//     {0,0,1,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0},
//     {0,0,1,1,0,1,0,1,0,1,1,1,0,1,0,1,0,0},
//     {0,0,1,0,0,1,0,1,0,1,0,0,0,1,1,0,0,0},
//     {0,0,1,0,0,1,1,1,0,1,1,1,0,1,0,1,0,0},
//     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
// };


// const int  M = 8;
// const int  N = 27;
// const int[M][N] A = {
//     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//     {0,0,0,1,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
//     {0,0,0,1,1,1,0,1,1,0,0,1,0,0,1,0,0,1,1,1,0,0,0,0,0,0,0},
//     {0,0,0,1,0,1,0,1,1,0,0,1,0,0,1,0,0,1,0,1,0,0,0,0,0,0,0},
//     {0,0,0,1,0,1,0,1,1,1,0,1,1,0,1,1,0,1,1,1,0,0,0,0,0,0,0},
//     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
// };

const int  M = 8;
const int  N = 94;
const int[M][N] A = {
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,1,1,1,0,1,1,0,1,1,0,0,1,1,0,0,1,1,0,1,0,1,1,1,0,1,1,1,0,0,1,1,0,0,1,1,0,1,1,1,0,0,1,1,1,1,1,0,1,1,1,0,1,0,1,0,1,1,0,0,1,1,0,1,1,0,1,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,1,0,1,0,1,0,0,1,0,0,0,1,0,0,0,1,0,0,1,0,1,0,1,0,1,0,1,0,0,1,0,0,0,1,0,0,1,0,1,0,0,1,0,1,0,1,0,1,0,1,0,1,1,0,0,1,0,0,1,0,1,0,1,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,1,1,1,0,1,0,0,1,1,1,0,1,1,1,0,1,0,0,1,0,1,0,1,0,1,1,1,0,1,1,0,0,0,1,0,0,1,1,1,0,0,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,0,0,1,1,1,0,1,0,0,1,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

// const int  M = 8;
// const int  N = 33;
// const int[M][N] A = {
//     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//     {0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
//     {0,1,1,0,1,1,1,0,1,1,0,0,1,0,0,1,1,0,0,1,0,1,0,1,0,1,1,1,0,1,1,1,0},
//     {0,1,0,0,1,0,1,0,1,0,0,0,1,0,0,1,0,0,0,1,1,0,0,1,0,1,0,1,0,1,0,1,0},
//     {0,1,0,0,1,0,1,0,1,0,0,0,1,0,1,1,0,0,0,1,0,1,0,1,0,1,0,1,0,1,1,1,0},
//     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0},
//     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0},
//     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
// };

// const uint  M = 3;
// const uint  N = 3;
// const int[M][N] A = {
//     {0,1,0},
//     {0,1,1},
//     {0,0,0}
// };

float  t  = 0.001*(_elapsed_t);
float  wt = 0.001*(_elapsed_t%6000 - 3000);

void main() {

//     float y = 1 - tc.y;
//     float x = tc.x;

    float x = cc.x ;
    float y = cc.y 
            // wave 
//               + 0.2*sin(x-(0*sumf[20]+2*t))
    ;
//     float y = (1 - tc.y);

    float f  = 1;
    float z0 = 3;
    float z;

//     float R = 0.3;
    float t_ = 0.072*(sumf[13]);
//     float t_ = wt;

    // wave packet
    float env = 1.0*exp( -200*(tc.x-t_)*(tc.x-t_));
    float pkg = 1.0*sin( ( tc.x - t_ )/0.01 );

    float R = 1.2
// 		*(1.1+sin(t))
// 		+  env * pkg
        // dispersion
//         * sqrt(t_)
        ;

    float R_2 = R*R;
    float R_3 = R_2*R;
    float R_4 = R_2*R_2;

    float y_2 = y*y;
    float z0_2 = z0*z0;

    // calculate azimuth from projection y
    float a = -acos( ( R_2*y_2 + R*y_2*z0 + sqrt(R_4 - 2*R_3*y_2*z0 - R_2*y_2*z0_2))
            / ( R_2 + R_2 * y_2 )
			)
    // remove artifact 
        -0.0001 
        ;

    z = z0 - R*( 1-cos(a) );

    x = x* f/z
        + 1*mod(0.06*t,100) +2*t_
        + 0.0015*E[3]*sin(300.0*a) 
        ;

	a = -tc.y+ 0.1*sin(2*t-2*x);
// 	a = -tc.y + 0.2*sin(x-(0*sumf[20]+2*t));
    int j = int( floor(x*N) + 0*t ) % (N);
    int i = int( floor(a*M) + 0*((j)/4)  ) % M;

//     float m = float( (i+j) %2 );
    float m = A[i][j];
//     float m = float(i == 1 && j==0);

    float c = a*a;//*a*a;
    color = vec4(m-c,m-c,m-c,1);
//     color = vec4(m,m,m,1);

}
