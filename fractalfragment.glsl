#version 330

out vec4 FragColor;
uniform float mid;
uniform float hig;
uniform float low;

// uniform vec2 center;
// uniform float scale;
// uniform int iter;

layout(pixel_center_integer) in vec4 gl_FragCoord;
void main() {
    vec2 z, c;
	int iter = 50;
// 	float scale = 0.0009;
	float scale = 0.0013;

//     c.x = (gl_FragCoord.x -10) * scale - 1;
//     c.y = (gl_FragCoord.y +300) * scale - 0;

//     c.x = 1.3333 * (gl_FragCoord.x - 0.5) * scale - 1;
//     c.y = (gl_FragCoord.y - 0.5) * scale - 0;

    c.x = (gl_FragCoord.x +200) * scale+low*2  - 1;
    c.y = (gl_FragCoord.y +300) * scale+low*2 - 0;

    int i;
    z = c;
    for(i=0; i<iter; i++) {
        float x = (z.x * z.x - z.y * z.y) + c.x;
        float y = (z.y * z.x + z.x * z.y) + c.y;

        if((x * x + y * y) > 4.0) break;
        z.x = x;
        z.y = y;
    }

	float d = mid+low+hig;

//     FragColor = vec4( (i == iter) ? 1.0 : 0.0, 0.00000001*d, 0, 1);
//     FragColor = vec4( float(i) / float(iter) , 0.00000001*d, 0.001, 1);
    FragColor = vec4( float(i) / float(iter) , 0.00000001*d, gl_FragCoord.x*0.001, 1);
}

// void main(){
//     FragColor = vec4(low*1.0, mid*1.0, hig*1.0, 1.0);
// }
