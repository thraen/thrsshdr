void triangle() {
    float m = 0.5;
    float t = 0.1;

    if (      cc.x > m*(cc.y-m) + t
         &&  -cc.x > m*(cc.y-m) + t
         && m*tc.y > t
         ) {
        color = vec4(1,1,1,0);
    }
    else
        color = vec4(0,0,0,0);

}

void main(){
    triangle();
}
