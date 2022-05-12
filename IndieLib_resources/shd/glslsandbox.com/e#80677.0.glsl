#ifdef GL_ES
//precision mediump float;
#endif

#define PI 3.14159265359
#define TWO_PI 6.28318530718
#define EPS 0.001

const vec3 c_cyan = vec3(0.4,0.9,0.9);
const vec3 c_white = vec3(0.9,0.9,0.9);
const vec3 c_yellow = vec3(0.6,0.6,0.0);
const vec3 c_red = vec3(0.6,0.0,0.0);

/*
uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform float u_time;

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;
*/

float plot(vec2 st, float pct){
  return  smoothstep( pct-0.1, pct, st.y) -
          smoothstep( pct, pct+0.1, st.y);
}

float step2(float step_value, float value)
{
    return smoothstep(
        step_value - EPS,
        step_value + EPS,
        value);
}

vec3 draw_arc(in vec2 point,
              in float radius, in  vec2 center, in float start_angle,in  float end_angle, in float width,
              in vec3 currentColor, in  vec3 color )
{
    point = center - point;
    float r = length(point);
    float a = fract( (atan(point.y,point.x) - start_angle) / TWO_PI + 2.5  ) * TWO_PI ;
    
    end_angle -= start_angle;
    
    return mix(currentColor,  color , 
            (1.0 - step(end_angle,a)) 
               *
            (step2(radius - width/ 2.0,r)- step2(radius + width / 2.0,r))
              );
    
}

vec3 circle_outline(vec2 pos,float width, float radius, vec2 center, vec3 current_color, vec3 stroke_color)
{
    vec2 tC = center-pos;
    float r1 = radius * 2.0 - width /2.0;
    float r2 = radius * 2.0 + width /2.0;
    float dist = tC.x*tC.x+tC.y*tC.y;
    return mix(current_color, stroke_color,
        step2(0.25*r1*r1, dist) - step2(0.25*r2*r2, dist));
}

vec3 circle_outline_small(vec2 pos,float width, float radius, vec2 center, vec3 current_color, vec3 stroke_color)
{
    vec2 tC = center-pos;
    float r1 = radius * 2.0 - width /2.0;
    float r2 = radius * 2.0 + width /2.0;
    float dist = tC.x*tC.x+tC.y*tC.y;
    return mix(current_color, stroke_color,
        step(0.25*r1*r1, dist) - step(0.25*r2*r2, dist));
}


mat2 rotate2d(float _angle){
    return mat2(cos(_angle),-sin(_angle),
                sin(_angle),cos(_angle));
}

vec2 rotate2d(in vec2 point,in  mat2 matrix, in vec2 origin)
{
    point -= origin;
    point = matrix * point;
    point += origin;   
    return point;
}

vec2 rotate2d(in vec2 point,in  float angle,in  vec2 origin)
{
    mat2 matrix = rotate2d(angle);
    return rotate2d(point, matrix, origin);
}

vec3 box(in vec2 _st, in vec2 _size, in vec3 currentColor, in  vec3 color){
    _size = vec2(0.5) - _size*0.5;
    vec2 uv = smoothstep(_size,
                        _size+vec2(0.001),
                        _st);
    uv *= smoothstep(_size,
                    _size+vec2(0.001),
                    vec2(1.0)-_st);
    return mix(currentColor, color, uv.x*uv.y);
}

vec2 translate2d(vec2 point, vec2 offset)
{
    return point - offset;
}

vec3 draw_sector(in vec2 point,
              in float radius, in  vec2 center, in float start_angle,in  float end_angle,
              in vec3 currentColor, in  vec3 color )
{
    point = center - point;
    float r = length(point);
    float a = fract( (atan(point.y,point.x) - start_angle) / TWO_PI + 2.5  ) * TWO_PI ;
    
    end_angle -= start_angle;
    
    return mix(currentColor,  color ,  
               (smoothstep(0.0,end_angle,a))
               *
               (1.0-step(end_angle,a))
               *
               (1.0 - step2(radius,r))
              );
}

vec3 circle(vec2 pos,float radius, vec2 center,
             vec3 current_color, vec3 stroke_color)
{
    vec2 tC = center-pos;
    return mix(current_color, stroke_color, 
               1.0 - smoothstep(0.25*radius*radius-0.00001, 0.25*radius*radius+ 0.00001, tC.x*tC.x+tC.y*tC.y));
}

void main() {
    vec2 u_resolution = resolution.xy;
    float u_time = time;
    vec2 st = xlv_TEXCOORD0.xy/u_resolution;

    
    vec3 color = vec3(0.0);
    
    color = draw_sector(st,0.3,vec2(0.5),u_time - PI/2.0 , u_time, color, c_cyan*0.5);
    
    float c_time = u_time * 0.09;
    color = circle(st, 0.01, vec2(0.5)+ vec2(sin(c_time * 1.1)*cos(c_time*1.2), cos(c_time*1.3)*sin(c_time*0.4)) / 4.0, color, c_white);
    c_time = u_time * 0.11;
    color = circle(st, 0.01, vec2(0.5)+ vec2(sin(c_time * 1.012)*cos(c_time*1.432), cos(c_time*1.148)*sin(c_time*0.624)) / 4.0, color, c_white);
    c_time = u_time * 0.2;
    color = circle(st, 0.02, vec2(0.5)+ vec2(sin(c_time * 1.012)*cos(c_time*1.432), cos(c_time*1.148)*sin(c_time*0.624)) / 4.0, color, c_red);
    
    color = box(rotate2d(st,PI/4.0, vec2(0.5)), vec2(0.003,0.6), color, c_white*0.3);
    color = box(rotate2d(st,-PI/4.0, vec2(0.5)), vec2(0.003,0.6), color, c_white*0.3);
    
	color = circle_outline_small(st, 0.005, 0.02, vec2(0.5), color, c_cyan*0.3);
    
    vec2 st2 = st;
    st2 = rotate2d(st2, u_time -PI/2.0 , vec2(0.5));
    st2 = translate2d(st2,vec2(0.0, 0.15));
    color = box(st2, vec2(0.005,0.3), color, c_cyan);
    
    
    color = circle_outline(st, 0.005, 0.25, vec2(0.5), color, c_cyan);
    color = circle_outline(st, 0.005, 0.3, vec2(0.5), color, c_white);
    color = circle_outline(st, 0.005, 0.35, vec2(0.5), color, c_cyan);
    
    color = draw_arc(st, .36, vec2(0.5), 0.0*PI/2.0 - u_time /4. , 0.0*PI/2.0 - u_time/4. + PI /3.0 , 0.008, color, c_yellow);
    color = draw_arc(st, .36, vec2(0.5), 1.0*PI/2.0 - u_time /4. , 1.0*PI/2.0 - u_time/4. + PI /3.0 , 0.008, color, c_yellow);
    color = draw_arc(st, .36, vec2(0.5), 2.0*PI/2.0 - u_time /4. , 2.0*PI/2.0 - u_time/4. + PI /3.0 , 0.008, color, c_yellow);
    color = draw_arc(st, .36, vec2(0.5), 3.0*PI/2.0 - u_time /4. , 3.0*PI/2.0 - u_time/4. + PI /3.0 , 0.008, color, c_yellow);
    color = circle_outline(st, 0.006, 0.37, vec2(0.5), color, c_cyan);

    gl_FragColor = vec4(color,1.0);
}

