float fst_layerf = 0.5;
float snd_layerf = 0.25;

// float af = fst_layerf;
// float af = snd_layerf;
float af = intBitsToFloat( floatBitsToInt(fst_layerf) | floatBitsToInt(snd_layerf) );

void triangle() {
    float m = 0.5;
    float t = 0.1;

//     float k = 0.1*log( 1+E[2] );
//     float k = 1;
//     t = (k > 0.01) ? -k : t;
//     t = -500*low;

//     if ( 
//             abs(cc.x)*2 < (0.5-cc.y)
//        ) {
    if (      cc.x > m*(cc.y-m) + t
         &&  -cc.x > m*(cc.y-m) + t
         && m*tc.y > t
         ) {
//         color = vec4(E[3],10000*low,1,a);
        color = vec4(1,1,1,af);
    }
    else
        color = vec4(0,0,0,af);

}

void main(){
    triangle();
}
