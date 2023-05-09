#line 2

vec4 violet = vec4(1,0,1,1);
vec4 blue =   vec4(0,0,1,1);

float t = 0.001*_elapsed_t;

void main() {
//     //blacklight

//     float a = distance(vec2(0, -0.4), 0.2*vec2(cos(t), sin(t)));

    float b = sumf[3] + sumf[ 6] + sumf[12] + sumf[24] + sumf[ 48] + sumf[ 96];
    float a = sumf[4] + sumf[ 8] + sumf[16] + sumf[32] + sumf[ 64] + sumf[128];
    float c = sumf[5] + sumf[10] + sumf[20] + sumf[40] + sumf[ 80] + sumf[160];
    a*=5;
    b*=5;
    c*=5;

    float r = distance(cc, 0.2*(1+sin(c))*vec2(cos(a), sin(b)));
    color = vec4(1/r, 1/r, 2/r, 1)/100;
// 
//     // shit
//     float ax =   
//                  smoothstep(0, 2, cc.x+0.2)
//                - smoothstep(0, 2, cc.x-0.2)
//         ;

//     float d_ = (length(cc) - 0.25) ;
//     float d = 1 -  10*pow( d_ , 0.55) ;
//     float d =  pow( d_ , 0.09) ;

//     color = vec4(d,d,d,d);

//     const float a = 0.04;
//     const float b = 0.08;
//     float
//     ax = sign( cc.x +a -b*cc.y)
//         -sign( cc.x -a +b*cc.y)
//     ;
//     color = vec4(ax,ax,ax,ax);

//     color = vec4(d+ax,d+ax,d+ax,d+ax);
}

