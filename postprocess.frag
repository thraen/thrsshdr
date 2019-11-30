#version 450

in vec2 cc;
in vec2 tc;

uniform sampler2D render_texture;

out vec3 color;

uniform float mid;
uniform float hig;
uniform float low;
uniform int _w;
uniform int _h;
uniform int _elapsed_t;

uniform float E[9];

// float sc = E[2];
float sc = 1;

void main(){
	color = texture( render_texture, vec2( sc*tc.x, sc*tc.y ) ).xyz;
// 	color = texture( render_texture, vec2( sc*(gl_FragCoord.x/_w), sc*(gl_FragCoord.y/_h) )).xyz;
}
