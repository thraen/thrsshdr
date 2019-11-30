#version 450
#
#define PI 3.1415926535897932384626433832795

in vec2 cc;

uniform sampler2D u_now;
uniform sampler2D u_prv;

out vec4 color;

uniform float mid;
uniform float hig;
uniform float low;
uniform int _w;
uniform int _h;
uniform int _elapsed_t;

uniform float E[6];

float t  = float(_elapsed_t);

void main(){
	float x	= cc.x;
	float y	= cc.y;

	float d	= x*x +y*y;

	// slotted disc
	if (d<0.022 && !(x>=-0.019 && abs(y) <=0.02 ) ) {
		color = vec4(0.5,0.5,0,1);
	}else{
		color = vec4(0,0,0,0);
	}
}

