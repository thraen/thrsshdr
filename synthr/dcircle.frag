#version 410

uniform vec4 color;

out vec4 out_color;

in vec2 uv;


void main() {
    const float a = 0.04;
    const float b = 0.08;
    float
    ax = sign( uv.x +a -b*uv.y)
        -sign( uv.x -a +b*uv.y)
    ;

    float d_ = (length(uv) - 0.5) ;
    float d = clamp(1 -  10*pow( d_ , 0.95), 0, 1);

    out_color = vec4(d+ax,d+ax,d+ax,d+ax) * color;
//     out_color = vec4(ax,ax,ax,ax) * color;
}
