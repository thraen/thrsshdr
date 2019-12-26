
void triangle() {
    float m = 0.5;
    float t = 0.1;
//     float k = 0.1*log( 1+E[2] );
    float k = low;
    t = (k > 0.01) ? -k : t;
    t = -500*low;
    if (      cc.x > m*(cc.y-m) + t
         &&  -cc.x > m*(cc.y-m) + t
         && m*tc.y > t
         ) {
        color = vec4(E[3],10000*low,E[5],1);
    }
    else
        color = vec4(0,0,0,0);
}

void main(){
    triangle();
}
