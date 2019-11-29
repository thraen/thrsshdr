#version 330

in vec2 uv;

uniform sampler2D render_texture;

out vec3 color;

uniform float mid;
uniform float hig;
uniform float low;
uniform int _w;
uniform int _h;
uniform int _elapsed_t;

uniform float E[6];

// float sc = E[2];
float sc = 1;

void main(){
// 	color = texture( render_texture, vec2( sc*(gl_FragCoord.x/_w), sc*(gl_FragCoord.y/_h) )).xyz;

	// pass through
	color = texture( render_texture, vec2( sc*uv.x, sc*uv.y ) ).xyz;

	// dizzy
//	color = texture( render_texture, sc*uv + 0.05*vec2( sin(float(_elapsed_t*0.005)+ low),cos(_elapsed_t*0.005+ mid)) ).xyz;
}
