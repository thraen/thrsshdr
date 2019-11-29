#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 pos;

// layout(location = 1) in vec2 uv;

// Output data ; will be interpolated for each fragment.
out vec2 uv;

uniform float E[6];

float sc = 1;

void main(){
	gl_Position = vec4(sc*pos.x, sc*pos.y, pos.z, 1.0);
	uv = ( pos.xy + vec2(1,1) )/2.0;
	//uv = pos.xy; //ich checks nicht
}


