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

uniform float E[9];

const float dx = 0.5;
const float dt = 0.200;
//const float dt = 1.0;

//const float r  = dt/(dx*dx);		
//wasn jetzt
const float r  = dt/dx;		

float a1 =  +0.4;
float a2 =  +0.0;

float t  = float(_elapsed_t);

vec4 fluss_naiv( vec4 x1, vec4 x2, vec4 y1, vec4 y2 ){
	return 0.5*( a1*(x1+x2) + a2*(y1+y2) );
}

vec4 naiv( vec4 uxm, vec4 uym, vec4 u, vec4 uxp, vec4 uyp ){
	return u - 0.5*r*( a1*(uxp-uxm) + a2*(uyp-uym) );
}

const float wght = 120;
vec4 laxf( vec4 uxm, vec4 uym, vec4 u, vec4 uxp, vec4 uyp ){
	return 
		(1.0/ (4+wght) )* ( uxp + uxm + uyp + uym + wght*u)
		- 0.5*r* ( a1* (uxp - uxm) + a2* (uyp - uym) );
}

vec4 fluss_lxw1( float a, vec4 u, vec4 v ){
	// meins
	return 0.5* ( a*(u+v) + r*a*a*(u-v) );
}

vec4 fluss_lxw( vec4 x1, vec4 x2, vec4 y1, vec4 y2 ){
	return 0.5* ( 	  a1*(x1+x2) + r*a1*a1*(x1-x2) 
	 				+ a2*(y1+y2) + r*a2*a2*(y1-y2) 	
				);
}
vec4 laxw( vec4 uxm, vec4 uym, vec4 u, vec4 uxp, vec4 uyp ){
	return 
		u + 0.5*(
			- r*( a1* (uxp - uxm) + a2* (uyp - uym) )
			+ r*r*( a1*a1* (uxp + uxm - 2*u) + a2*a2* (uyp + uym - 2*u) )
		);
}

vec4 fluss_upw1( float a, vec4 u, vec4 v ){
	return (a>=0) ? (a*u) : (a*v);
}

vec4 fluss_upw( vec4 x1, vec4 x2, vec4 y1, vec4 y2 ){
	return fluss_upw1( a1, x1, x2 ) + fluss_upw1( a2, y1, y2 );
}

vec4 upwind( vec4 uxm, vec4 uym, vec4 u, vec4 uxp, vec4 uyp ){
	vec4 retx = (a1>=0) ? (u - uxm)*a1 : (uxp - u)*a1;
	vec4 rety = (a2>=0) ? (u - uym)*a2 : (uyp - u)*a2;
	return 
		u  
		- r*( retx + rety );
// 		- 0.5*r*( a1* (uxp - uxm) + a2* (uyp - uym) )  
// 		+ 0.5*r*( abs(a1)* (uxp + uxm - 2*u) + abs(a2)* (uyp + uym - 2*u) );
}

const vec4 zeros = vec4(0.0, 0.0, 0.0, 0.0);
const vec4 ones  = vec4(1.0, 1.0, 1.0, 1.0);
const vec4 twos  = vec4(2.0, 2.0, 2.0, 2.0);
const vec4 infts = vec4(9999.0, 9999.0, 9999.0, 9999.0);

vec4 theta( vec4 um, vec4 u, vec4 up ){
	// komponentenweise? ist das richtig so?
	if ( (up-u) == 0 )
		return infts;
	else
		return (u - um) / (up - u);
}

// Suberbee-Flusslimiter
vec4 sbee( vec4 theta ){
	return max( zeros, max( min(ones, 2*theta), min(theta, twos) ) );
}

vec4 fluss_lim1( float a, vec4 um, vec4 u, vec4 up ){
	vec4 thet = theta(um, u, up);
// 	vec4 thet = ones;
// 	vec4 thet = infts;

	return 	  (1- sbee( thet ) )* fluss_upw1(a, u, up)
			+     sbee( thet )  * fluss_lxw1(a, u, up);
}

vec2 feld( float t, vec4 koo){
	float x		= koo.x / _w;
	float y		= koo.y / _h;
	float div	= 1;
	return vec2( 
		//sin(t*0.001)*(x-0.5+sin(t*0.0005)+low)*0.08,
		//cos(t*0.001)*(y-0.5+sin(t*0.0002)+low)*0.08 

		// 0.5*(y-0.5),
		//-0.5*(x-0.5)

		 0.5*(y-0.5) + div*sin(t*0.0005)*(x-0.5),
		-0.5*(x-0.5) + div*sin(t*0.0005)*(y-0.5)
		);
}

void main(){

	vec4 u   = texelFetch( u_now, ivec2(     gl_FragCoord.x      ,     gl_FragCoord.y       ), 0 );
	vec4 uxp = texelFetch( u_now, ivec2( mod(gl_FragCoord.x+1,_w),     gl_FragCoord.y       ), 0 );
	vec4 uxm = texelFetch( u_now, ivec2( mod(gl_FragCoord.x-1,_w),     gl_FragCoord.y       ), 0 );
	vec4 uyp = texelFetch( u_now, ivec2(     gl_FragCoord.x	     , mod(gl_FragCoord.y+1,_h) ), 0 );
	vec4 uym = texelFetch( u_now, ivec2(     gl_FragCoord.x	     , mod(gl_FragCoord.y-1,_h) ), 0 );

	vec4 uxmm= texelFetch( u_now, ivec2( mod(gl_FragCoord.x-2,_w),      gl_FragCoord.y      ), 0 );
	vec4 uymm= texelFetch( u_now, ivec2(     gl_FragCoord.x	     , mod(gl_FragCoord.y-2,_h) ), 0 );

	vec4 uxpp= texelFetch( u_now, ivec2( mod(gl_FragCoord.x+2,_w),      gl_FragCoord.y      ), 0 );
	vec4 uypp= texelFetch( u_now, ivec2(     gl_FragCoord.x	     , mod(gl_FragCoord.y+2,_h) ), 0 );

	vec2 a	 = feld( t, gl_FragCoord );
	vec2 axp = feld( t, vec4( mod(gl_FragCoord.x+1, _w), gl_FragCoord.y  , 0, 0 ));
	vec2 axm = feld( t, vec4( mod(gl_FragCoord.x-1, _w), gl_FragCoord.y  , 0, 0 ));
	vec2 ayp = feld( t, vec4(     gl_FragCoord.y       , mod(gl_FragCoord.y+1, _h), 0, 0 ));
	vec2 aym = feld( t, vec4(     gl_FragCoord.y       , mod(gl_FragCoord.y-1, _h), 0, 0 ));

	float a1 = a.x;
	float a2 = a.y;

	vec4 anteilx = (a1 >= 0) 
		? fluss_lim1(  a1, uxm, u, uxp ) - fluss_lim1(  a1, uxmm, uxm, u ) 
		: fluss_lim1( -a1, uxp, u, uxm ) - fluss_lim1( -a1, uxpp, uxp, u ) ;

	vec4 anteily = (a2 >= 0) 
		? fluss_lim1(  a2, uym, u, uyp ) - fluss_lim1(  a2, uymm, uym, u ) 
		: fluss_lim1( -a2, uyp, u, uym ) - fluss_lim1( -a2, uypp, uyp, u ) ;

	float diva = 0.5*( (axp.x - axm.x) + (ayp.y - aym.y) )*dt/dx ;

	color = u - r* ( anteilx + anteily )  
		//;
		- u*diva;

	// test divergenz
	//if(abs(axp.x - axm.x)>0) {
		//color = vec4(1,0,0,1);
	//}
	//if(abs(ayp.y - aym.y)>0) {
		//color = vec4(1,0,0,1);
	//}
	//color = vec4(0, 1000*diva, 0, 1);

	// test CFL-Bedingung dx^2?
	//color -= vec4(1, 0, 0, 1);
	//if ( (abs(a.x)+abs(a.y))*dt/dx >0.5){
	//if ((a.x+a.y)*dt/dx >0.5){
		//color = vec4(1,0,0,1);
	//}

// 	color = u - r* ( 
// 		  fluss_lim1( a1, uxm, u, uxp ) - fluss_lim1( a1, uxmm, uxm, u ) 
// 		+ fluss_lim1( a2, uym, u, uyp ) - fluss_lim1( a2, uymm, uym, u ) 
// 	);
// 	color += vec4( 0, 1, 1, 1);

// 	color = upwind( uxm, uym, u, uxp, uyp );
// 	color = u - r* ( fluss_upw( u, uxp, u, uyp ) - fluss_upw( uxm, u, uym, u ) );

// 	color = laxw( uxm, uym, u, uxp, uyp );
// 	color = u - r* ( fluss_lxw( u, uxp, u, uyp ) - fluss_lxw( uxm, u, uym, u ) );
// 	color = u - r* ( fluss_lxw_( u, uxp, u, uyp ) - fluss_lxw_( uxm, u, uym, u ) );
}

