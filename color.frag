#version 330

// layout(location = 0) out vec4 color
out vec4 color;

uniform float mid;
uniform float hig;
uniform float low;

uniform float _w;
uniform float _h;

// ivec2 cpos = ivec2( gl_FragCoord.x-_h/2, gl_FragCoord.y-_w/2 );

vec2 cpos = vec2( float(gl_FragCoord.x/_w)-0.5, float(gl_FragCoord.y/_h)-0.5);

void main(){
	//     color = vec4(low*0.2, mid*0.4, hig*2.9, 1.0);
	color = vec4(1,0,0,0);

//     color = vec4( sin( 5*float(cpos.x^2+cpos.y^2) ), 0.1, 0.2, 1);
//     color = vec4( sin( 1.2*(cpos.x*cpos.x+cpos.y*cpos.y) ), 0.1, 0.2, 1);
//     color = vec4( 0.002*(gl_FragCoord.y-_h) , 0.1, 0.2, 1);
}
