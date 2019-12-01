
// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 pos;

// Output data ; will be interpolated for each fragment.
out vec2 tc; // texture coordinates,  [ 0, 0 ] x [ 1, 1 ]
out vec2 cc; // centered coordinates, [-0.5, 0.5] x [-0.5, 0.5]

float sc = 1;

void main() {
	gl_Position = vec4(sc*pos.x, sc*pos.y, pos.z, 1.0);
	tc = ( pos.xy + vec2(1,1) )/2.0;
	cc = pos.xy/2;
}


