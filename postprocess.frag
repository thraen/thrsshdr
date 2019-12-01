uniform sampler2D render_texture;

// out vec3 color;

float sc = 1;

void main(){
	color = texture( render_texture, vec2( sc*tc.x, sc*tc.y ), 1 ).xyzw;
// 	color = texture( render_texture, vec2( sc*(gl_FragCoord.x/_w), sc*(gl_FragCoord.y/_h) )).xyz;
}
