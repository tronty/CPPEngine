#define Rot(a) mat2(cos(a),-sin(a),sin(a),cos(a))
#define antialiasing(n) (1.0*n)/min(iResolution.y,iResolution.x)
#define S(d,b) smoothstep(antialiasing(1.0),b,d)
#define  uicol vec3(0.2,0.75,0.7)*1.3
#define hash(h) fract(sin(h) * 43758.5453123)

float sdBox( in vec2 p, in vec2 b )
{
    vec2 d = abs(p)-b;
    return length(max(d,0.0)) + min(max(d.x,d.y),0.0);
}

float sdTriangleIsosceles( in vec2 p, in vec2 q )
{
    p.x = abs(p.x);
    vec2 a = p - q*clamp( dot(p,q)/dot(q,q), 0.0, 1.0 );
    vec2 b = p - q*vec2( clamp( p.x/q.x, 0.0, 1.0 ), 1.0 );
    float s = -sign( q.y );
    vec2 d = min( vec2( dot(a,a), s*(p.x*q.y-p.y*q.x) ),
                  vec2( dot(b,b), s*(p.y-q.y)  ));
    return -sqrt(d.x)*sign(d.y);
}

float sdPie( in vec2 p, in vec2 c, in float r )
{
    p.x = abs(p.x);
    float l = length(p) - r;
    float m = length(p-c*clamp(dot(p,c),0.0,r)); // c = sin/cos of the aperture
    return max(l,m*sign(c.y*p.x-c.x*p.y));
}

float sdSegment( in vec2 p, in vec2 a, in vec2 b )
{
    vec2 pa = p-a, ba = b-a;
    float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
    return length( pa - ba*h );
}

// http://mercury.sexy/hg_sdf/
vec2 pModPolar(inout vec2 p, float repetitions, float fix) {
    float angle = 2.0*3.14159265/repetitions;
    float a = atan(p.y, p.x) + angle/2.;
    float r = length(p);
    float c = floor(a/angle);
    a = mod(a,angle) - angle/2.*fix;
    p = vec2(-cos(a), sin(a))*r;

    return p;
}

vec3 UI(vec2 uv, vec3 col, float t) {
    vec2 uiuvref = uv;

    uv = mod(uv,0.2)-0.1;
    float d = sdBox(uv,vec2(0.0005,0.01));
    float d2 = sdBox(uv,vec2(0.01,0.0005));
    d = min(d,d2);
    col = mix(col,uicol*0.4,S(d,0.0)); 
    
    // center UI
    mat2 r = Rot(radians(t*10.0));

    uv = uiuvref;
    uv*= 0.4;
    uv*=Rot(radians(t*10.0)+radians(-3.0));
    uv.yx = pModPolar(uv.yx,60.0,1.8);
    d = sdBox(uv,vec2(0.0002,0.17));
    uv = uiuvref;

    uv*= 0.4;
    uv*=r;
    uv.yx = pModPolar(uv.yx,60.0,1.8);
    d2 = sdBox(uv,vec2(0.0002,0.16));

    d = min(d,d2);
    d = max(-(length(uv)-0.15),d);
    col = mix(col,(uicol),S(d,-0.001));
    
    uv = uiuvref;
    d = abs(length(uv)-0.36)-0.001;
    col = mix(col,(uicol),S(d,-0.001));

    d = abs(length(uv)-0.08)-0.01;
    d2 = sdPie(uv*Rot(radians(-t*50.0)),vec2(0.2,0.21),0.12);
    d = max(d2,d);
    col = mix(col,uicol,S(d,-0.001));

    uv*=Rot(radians(t*50.0));
    d = abs(length(uv)-0.345)-0.002;
    d2 = sdBox(uv,vec2(0.05,0.7));
    d = max(d2,d);
    col = mix(col,uicol,S(d,-0.001));
    
    uv = uiuvref;
    uv*=Rot(radians(-sin(t*1.5)*270.0));
    d = abs(length(uv)-0.32)-0.002;
    d2 = sdBox(uv,vec2(0.07,0.7));
    d = max(d2,d);
    col = mix(col,uicol,S(d,-0.001));
    
    uv = uiuvref;
    uv*=Rot(radians(-sin(t*1.2)*180.0));
    d = abs(length(uv)-0.05)-0.002;
    d2 = sdBox(uv,vec2(0.04,0.8));
    d = max(-d2,d);
    col = mix(col,uicol,S(d,-0.001));    
    
    uv = uiuvref;
    uv*= 0.55;
    uv*=Rot(radians(-t*15.0));
    uv.yx = pModPolar(uv.yx,70.0,1.8);
    d = sdBox(uv,vec2(0.0005,0.17));
    d = max(-(length(uv)-0.15),d);
    col = mix(col,uicol,S(d,-0.001));
    
    
    uv = uiuvref;
    uv*= 0.85;
    uv*=Rot(radians(t*20.0));
    uv.yx = pModPolar(uv.yx,40.0,1.8);
    d = sdBox(uv,vec2(0.0005,0.17));
    d = max(-(length(uv)-0.15),d);
    col = mix(col,uicol,S(d,-0.001));    
    
    uv = uiuvref;
    d = length(uv)-0.03+sin(t*2.0)*0.005;
    col = mix(col,uicol,S(d,-0.001)); 
    
    uv = uiuvref;
    uv*=1.0+sin(t*5.0)*0.05;
    uv*=Rot(radians(t*50.0));
    d =abs(sdBox(uv,vec2(0.11)))-0.001;
    d2 = sdBox(uv,vec2(0.07,0.21));
    d = max(-d2,d);
    d2 = sdBox(uv,vec2(0.21,0.07));
    d = max(-d2,d);
    col = mix(col,uicol,S(d,-0.001)); 
    
    uv = uiuvref;
	float numBar = 20.0;
    float deg = 360.0/numBar;
    vec2 pos = vec2(0.0,0.0);
    for(float i = 0.0; i<numBar; i+=1.0) {
		float rotVal = radians(i*deg+t*10.0)+radians(2.5);
        float rotVal2 = radians(i*deg+t*10.0)+radians(10.0);
        
    	mat2 m = Rot(rotVal);
        mat2 m2 = Rot(rotVal);
        float animateVal = sin(hash(i)*(i*deg)*t*0.1)*0.01;
        float animateVal2 = sin(hash(i)*(i*deg)*t*0.07)*0.02;
        float bdist = 0.45;
        float bdist2 = 0.46;
        float x = pos.x+cos(rotVal)*(bdist+animateVal);
        float y = pos.y+sin(rotVal)*(bdist+animateVal);
        float bar = sdBox((uv+vec2(y, x))*m, vec2(0.001,0.03+animateVal));
        x = pos.x+cos(rotVal2)*(bdist2+animateVal2);
        y = pos.y+sin(rotVal2)*(bdist2+animateVal2);
        float b = length(uv+vec2(y, x))-0.01;
        b =  min(b,bar);
    	col = mix( col, uicol, S(b,0.0) );
    }
    
    uv = uiuvref;
    deg = 360.0/20.0;
    for(float i = 0.0; i<numBar; i+=1.0) {
		float rotVal = radians(i*deg-t*30.0)+radians(45.0);
    	mat2 m = Rot(rotVal);
        float bdist = 0.25;
        float x = pos.x+cos(rotVal)*bdist;
        float y = pos.y+sin(rotVal)*bdist;
        float b = sdTriangleIsosceles((uv+vec2(y, x))*m, vec2(0.015,0.02));
    	col = mix( col, uicol, S(b,0.0) );
    }
    
    float dist = 0.33;
    uv = uiuvref;
    uv*= Rot(radians(-sin(t*1.1)*150.0));
    uv.x -= dist;
    uv.y -= dist;
    
    d = sdTriangleIsosceles(uv*Rot(radians(-140.0)),vec2(0.015,0.02));
    col = mix(col,uicol,S(d,-0.001)); 
    
    uv = uiuvref;
    uv*= Rot(radians(-sin(t*1.3)*160.0)+radians(45.0));
    uv.x -= dist;
    uv.y -= dist;
    
    d = sdTriangleIsosceles(uv*Rot(radians(-140.0)),vec2(0.015,0.02));
    col = mix(col,uicol,S(d,-0.001)); 
        
    uv = uiuvref;
    uv*= Rot(radians(sin(t*1.4)*180.0)+radians(90.0));
    uv.x -= dist;
    uv.y -= dist;
    
    d = sdTriangleIsosceles(uv*Rot(radians(-140.0)),vec2(0.015,0.02));
    col = mix(col,uicol,S(d,-0.001)); 
            
    uv = uiuvref;
    uv*= Rot(radians(sin(-t*1.6)*270.0)+radians(120.0));
    uv.x -= dist;
    uv.y -= dist;
    
    d = sdTriangleIsosceles(uv*Rot(radians(-140.0)),vec2(0.015,0.02));
    col = mix(col,uicol,S(d,-0.001)); 
    
    // graph
    uv = uiuvref;
    uv.x = abs(uv.x);
    uv.x -= 0.67;
    uv.y += 0.3;
    
    uv.y = mod(uv.y,0.042)-0.021;
    d = sdBox(uv,vec2(0.1,0.01));
    
    uv = uiuvref;
    uv.x = abs(uv.x);
    uv.x -= 0.67;
    uv.y += 0.3;
    d = max(uv.y-0.5,d);
    d = max(-(uv.y-0.09),d);
    col = mix(col,uicol*0.5,S(d,-0.001)); 
    
    uv = uiuvref;
    uv.x = abs(uv.x);
    uv.x -= 0.67;
    uv.y -= 0.12;
    
    for(float i = 1.0; i<=10.0; i+=1.0) {
        float x = 0.0;
        float y = i*0.042-0.105;
        float w = abs(sin(hash(i)*t*3.0)*0.1);
        d = sdBox(uv+vec2(-0.1+w,y),vec2(w,0.01));
        col = mix(col,uicol,S(d,-0.001)); 
    }
    
    dist = 0.205;
    uv = uiuvref;
    uv*= Rot(radians(t*30.0)+radians(170.0));
    uv.x -= dist;
    uv.y -= dist;
    
    d = abs(length(uv)-0.015)-0.001;
    col = mix(col,uicol,S(d,-0.001)); 
    
    uv = uiuvref;
    uv = abs(uv);
    uv.x -=1.2;
    uv.y -=0.7;
    
    d = sdSegment(uv,vec2(-0.43,-0.3),vec2(-0.65,-0.3))-0.001;
    d2 = sdSegment(uv,vec2(-0.65,-0.3),vec2(-0.77,-0.4))-0.001;
    d = min(d,d2);
    col = mix(col,uicol,S(d,-0.001)); 
    
    uv = uiuvref;
    uv = abs(uv);
    uv.x -=1.39;
    uv.y -=0.73;
    
    d = sdBox(uv+vec2(0.74,0.36),vec2(0.010+sin(t*5.0)*0.003+0.007));
    d2 = sdBox(uv+vec2(0.69,0.36),vec2(0.010+sin(t*6.0)*0.003+0.007));
    d = min(d,d2);
    float d3 = sdBox(uv+vec2(0.64,0.36),vec2(0.010+sin(t*7.0)*0.003+0.007));
    d = min(d,d3);
    col = mix(col,uicol,S(d,-0.001)); 
    
    return col;
}

void main( )
{
    vec2 uv = (2.0*xlv_TEXCOORD0.xy-iResolution.xy)/min(iResolution.y,iResolution.x);
    uv*=1.6;
    vec2 uiuvref = uv;
    //vec3 col = .94*max(mix(vec3(0.0,0.2,0.3)+(.1-length(uv)/3.),vec3(1),.1),0.9);
    vec3 col = vec3(0.9,0.9,0.9);
    
    // bg color
    //vec3 col = vec3(0.9,0.9,0.9);
    //vec3 col = vec3(0.9,0.9,0.9);
    vec3 bcol = vec3(0.3,0.75,0.7);
    
	float r3 = length( uv+vec2(-0.5,0.75) )*3.0;
    float r4 = length( uv+vec2(-0.5,0.75) )*3.5;
    
    col = mix( col, vec3(0.2,0.2,0.2), 1.0-smoothstep(0.98,1.0,r3) );
    col = mix( col, bcol, 1.0-smoothstep(0.98,1.0,r4) );
    float t = mod(iTime,8000.0);
    
    // UI
    float uvy = sin((t+2.0)*0.6)*1.2;
	if (uv.y+uvy > 0.5 && uv.y+uvy < 0.52) {
        uv.x += sin(uv.y*2.0)*0.1;
    }
    col = UI(uv,col,t);
    
    float dt = dot(uv, vec2(12.9898, 78.233));
    float noise = fract(sin(dt) * 43758.5453 + t);
    vec3 grain = vec3(noise) * (1.0 - col);
    col+=grain*0.15;    
    
    gl_FragColor = vec4(col,1.0);
}

