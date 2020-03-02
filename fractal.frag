
float t = float(_elapsed_t);

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
		float x = (z.x * z.x - z.y * z.y) + c.x+0.019*(sumf[12] + sumf[20]);
		float y = (z.y * z.x + z.x * z.y) + c.y;
		z.x = x;
		z.y = y;

		if(length(z.y) > (hig)*100000.0) break;
	}
    float c1 = float(i)/ (float(iter));
    float c2 = 0.4*float(i)/ (float(iter)*0.5);
    float c3 = float(i)/ (float(iter)*0.4);

	color = vec4( c1, c2, c3, 1);
// 	color = vec4( (i == iter) ? 1.0 : 0.0, 0, 0, 1);

// 	color = vec4( 5/float(i), 5/float(i), 0, 1);
	//color = vec4( float(i) / float(iter) , float(i)/hig/10, gl_FragCoord.x*0.001, 1);
}
