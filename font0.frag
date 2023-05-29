#line 2


const uint  M = 4;
const uint  N = 4;

const int[M][N] A = {
    {0,0,0,0},
    {0,1,0,0},
    {0,1,1,0},
    {0,0,0,0}
};

const int[M][N] B = {
    {0,0,0,0},
    {0,1,1,0},
    {0,0,1,0},
    {0,0,0,0}
};

const int L = 3;
const int C = 6;

const int[L][C][M][N] str = {
    { B, B, B, B, B, B },
    { A, A, A, A, A, A },
    { B, B, B, B, B, B },
};

float t    = 0.001*(_elapsed_t);

void main() {
    vec2 tc_ = vec2( tc.x, 1-tc.y );

    int l = int( floor( tc_.y*L ) );

    float tcx = mod( tc_.x + cos(l-1)*t , 1 );
    int c = int( floor( tcx*C ) );

    int i = int( floor( tc_.y*M*L) );
    int j = int( floor( tcx*N*C) );

    float m = str[l][c-l][i-c][j];

    color = 0.5*vec4(m,m,m,1);
}
