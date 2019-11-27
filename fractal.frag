#version 330
//	in vec2 uv;

out vec4 color;
uniform float mid;
uniform float hig;
uniform float low;

uniform int _w;
uniform int _h;
uniform int _elapsed_t;

float t = float(_elapsed_t);

layout(pixel_center_integer) in vec4 gl_FragCoord;
void main() {
	vec2 z, c;
	int iter	= 40;
// 	int iter	= int(200-55*low);
// 	float scale = 0.002 + 0.03*sin(t*0.001);
	float scale = 0.002;

	c.x = (gl_FragCoord.x -_w/2) * scale - 1;
	c.y = (gl_FragCoord.y -_h/2) * scale - 0.1;

	int i;
	z = c;
	for(i=0; i<iter; i++) {
		float x = (z.x * z.x - z.y * z.y) + c.x+0.001*mid;
		float y = (z.y * z.x + z.x * z.y) + c.y;

		if((x * x + y * y) > (hig)*100000.0) break;
		z.x = x;
		z.y = y;
	}
    float c1 = float(i)/ (float(iter));
    float c2 = 0.4*float(i)/ (float(iter)*0.5);
    float c3 = float(i)/ (float(iter)*0.4);

	color = vec4( c1, c2, c3, 1);
// 	color = vec4( (i == iter) ? 1.0 : 0.0, 0, 0, 1);

// 	color = vec4( 5/float(i), 5/float(i), 0, 1);
	//color = vec4( float(i) / float(iter) , float(i)/hig/10, gl_FragCoord.x*0.001, 1);
}
