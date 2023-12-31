/*
#extension GL_OES_standard_derivatives : enable

precision highp float;
//uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;
*/
vec3 BRICK_COLOR = vec3(192.0 / 2.0, 106.0 / 255.0, 59.0 / 255.0);
vec3 BRICK_COLOR_VARIATION = vec3( 30.0 /255.0, 20.0/255.0, 20.0/255.0);
vec3 MORTAR_COLOR = vec3(232.0 / 255.0, 216.0 / 255.0, 195.0 / 255.0);
vec2 BRICK_SIZE = vec2(0.08, 0.06);
vec2 BRICK_PCT = vec2(0.95, 0.9);
void main( void ) {

	vec2 uv = xlv_TEXCOORD0.xy / resolution.xy;
    
    vec2 position = uv / BRICK_SIZE;
    
    if(fract(position.y * 0.5) > 0.5)
    {
     	position.x += 0.5;   
    }
    
    position = fract(position);
    
    vec2 useBrick = step(position, BRICK_PCT);
    
   
    vec3 color = mix(MORTAR_COLOR, BRICK_COLOR, useBrick.x * useBrick.y) + BRICK_COLOR_VARIATION ;

gl_FragColor = vec4( vec3( color ), 1.0 );

}

//https://www.shadertoy.com/view/4tyGzz
