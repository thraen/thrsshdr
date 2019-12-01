
void triangle() {
    float s = E[1];
    if ( abs(cc.x) < 0.5*(0.5-cc.y) ) {
        color = vec4(E[1],E[3],E[5],1);
    }
    else
        color = vec4(0,0,0,1);
}

void main(){
    triangle();
}
