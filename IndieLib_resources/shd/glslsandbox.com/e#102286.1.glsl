#if 0
#ifdef GL_ES
precision highp float;
#endif

#extension GL_OES_standard_derivatives : enable

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;
#endif
float rect(vec2 p, vec2 c, vec2 rad)
{
    vec2 d = abs(p - c) - rad;
    return max(d.x, d.y);
}

float sub(float a, float b)
{
    return max(a, -b);
}

float smin( float a, float b, float k )
{
    float h = clamp( 0.5+0.5*(b-a)/k, 0.0, 1.0 );
    return mix( b, a, h ) - k*h*(1.0-h);
}

vec2 rot(vec2 v, float angle)
{
    float c = cos(angle);
    float s = sin(angle);
    return v*mat2(c, -s, s, c);
}

float intersect(float a, float b){return max(a,b);}

float box( vec3 p, vec3 b ){
	vec3  di = abs(p) - b;
	float mc = max(di.x, max(di.y, di.z));
	return min(mc,length(max(di,0.0)));}
float sphere(vec3 p, float r){return length(p) - r;}
float sdRoundBox( in vec3 p, in vec3 b, in float r ) 
{
    vec3 q = abs(p) - b;
    return min(max(q.x,max(q.y,q.z)),0.0) + length(max(q,0.0)) - r;
}
float hash12(vec2 p) {
	float h = dot(p,vec2(127.1,311.7));	
    return fract(sin(h)*43758.5453123);
}

// 3d noise
float noise_3(in vec3 p)
{
    vec3 i = floor(p);
    vec3 f = fract(p);	
	vec3 u = f*f*(3.0-2.0*f);
    
    vec2 ii = i.xy + i.z * vec2(5.0);
    float a = hash12( ii + vec2(0.0,0.0) );
	float b = hash12( ii + vec2(1.0,0.0) );    
    float c = hash12( ii + vec2(0.0,1.0) );
	float d = hash12( ii + vec2(1.0,1.0) ); 
    float v1 = mix(mix(a,b,u.x), mix(c,d,u.x), u.y);
    
    ii += vec2(5.0);
    a = hash12( ii + vec2(0.0,0.0) );
	b = hash12( ii + vec2(1.0,0.0) );    
    c = hash12( ii + vec2(0.0,1.0) );
	d = hash12( ii + vec2(1.0,1.0) );
    float v2 = mix(mix(a,b,u.x), mix(c,d,u.x), u.y);
    return max(mix(v1,v2,u.z),0.0);
}

float fbm(vec3 x)
{
    float r = 0.0;
    float w = 1.0, s = 1.0;
    for (int i=0; i<5; i++)
    {
        w *= 0.5;
        s *= 2.0;
        r += w * noise_3(s * x);
    }
    return r;
}

vec3 clouds(vec3 rd)
{
    float ctime = time *0.01;
    vec2 uv = rd.xz / (rd.y + 0.4);
    vec3 clouds = vec3(fbm(vec3(uv.yx * 1.4 + vec2(ctime, 0.), ctime) ) * 2.5);
    clouds = pow(clouds, vec3(2.));
	vec3 other = vec3(0.2,0.0,0.3);
	float m = clamp(rd.y+0.2,0.0,1.0);
	return mix(other,clouds,m);
}

#define TAU 6.283185

float ground(vec3 p)
{
    float t = fract(time*0.41)*TAU;
    float t2 = fract(time*0.24)*TAU;
    float t3 = fract(time*0.332)*TAU;
    float h = -4.;
    h += sin(p.z*0.1+t3-p.x*0.23)*0.25;
    h += sin(t2+p.x*0.231)*0.42;
    h += cos(t+0.55+p.z*0.31)*0.83;
    h += sin(t+0.+p.z*0.16)*0.45;
    return p.y-h;
}

float objID;

float map(vec3 p)
{
   float g = ground(p);
   p.x += (sin(time+p.z)*0.2);
   p.y += (cos(time+p.z)*0.2);
    
   p.y -= 20.0;
   float d = 1e10;
   d = sphere(p*vec3(4.0, 4.0, 1.0), 1.0)*0.25;
   float a = atan(p.x/p.y);
   float m = (1.0-abs(p.z))*0.75;
   d+=abs(sin(a*20.0)*(0.005*m));
   float f1 = box(p+vec3(0.0,-0.15,0.5), vec3(0.01, 0.25, 0.2));
   float f2 = box(p+vec3(0.0,0.0,0.5), vec3(0.5, 0.01, 0.2));
   f1 = min(f1,f2);
   f1 = intersect(f1, sphere(p - vec3(0.0, -0.1, -1.55), 1.2));
   d = smin(f1, d, 0.025);
   vec2 pp = rot(p.yz+vec2(-0.305, 0.56),1.0);

   float dep = 0.015;


   float d3 = sdRoundBox(p+vec3(0.0,0.23,-0.4),vec3(0.025,0.025,0.161),0.03);
   d = smin(d3,d,0.01);
   objID=2.0;
   if (d<g)
   {
       objID=2.0;
   }

   return min(d,g);
}
// http://iquilezles.org/www/articles/normalsSDF/normalsSDF.htm
vec3 calcNormal( in vec3 pos )
{
    vec2 e = vec2(1.0,-1.0)*0.5773;
    const float eps = 0.0005;
    return normalize( e.xyy*map( pos + e.xyy*eps ) + 
					  e.yyx*map( pos + e.yyx*eps ) + 
					  e.yxy*map( pos + e.yxy*eps ) + 
					  e.xxx*map( pos + e.xxx*eps ) );
}

vec3 render( vec2 p )
{
    objID=0.0;
	float zoom = sin(time*0.33)*0.2;
	float an = -1.05+sin(time*0.1)*0.21;
	float y = 0.5+sin(time*0.45)*0.29;
	vec3 ro = vec3( zoom+2.0*cos(an), (0.7-y*0.5)+20.0, zoom+2.5*sin(an) );
    vec3 ta = vec3( 0.0, 20.0, 0.0 );
    // camera matrix
    vec3 ww = normalize( ta - ro );
    vec3 uu = normalize( cross(ww,vec3(0.0,1.0,0.0) ) );
    vec3 vv = normalize( cross(uu,ww));
    // create view ray
    vec3 rd = normalize( p.x*uu + p.y*vv + 3.5*ww );
    // raymarch
    const float tmax = 490.0;
    float t = 0.0;
    for( int i=0; i<150; i++ )
    {
        vec3 pos = ro + t*rd;
        float h = map(pos);
        if( h<0.0001 || t>tmax ) break;
        t += h;
    }
	float id = objID;
	vec3 objcol = vec3(0.85,0.7,0.7);
	if (id>1.0)
		objcol = vec3(0.13,0.23,1.1);
	if (id>2.0)
		objcol = vec3(0.7+sin(fract(time)*TAU)*0.15,0.04,0.04);
	
    // shading/lighting	
    vec3 background = clouds(rd);
    vec3 col = background;
    if( t<tmax )
    {
		vec3 l = vec3(0.0,1.0,0.0);
        vec3 pos = ro + t*rd;
        vec3 nor = calcNormal(pos);
        float dif = clamp( dot(nor,vec3(0.57703)), 0.0, 1.0 );
        float amb = 0.5 + 0.5*dot(nor,l);
		float spe = pow(clamp(dot(reflect(l, nor), rd), 0.0, 1.0), 16.0);
        col = vec3(0.1,0.1,0.1)*amb + objcol*dif;
		col += spe;
        float m = exp(-0.00001*(t*t));
        col = mix(background,col,m);
    }
    // gamma        
    return sqrt(col);
}

void main()
{
	vec2 p = (-resolution.xy + 2.0*xlv_TEXCOORD0.xy)/resolution.y;
	vec3 col = render(p);
	float m = step(length(col),0.0);
        float r = clamp(hash12(p+fract(time*0.7))+0.5,0.0,1.0);
        float rr = 0.5+sin(sin(r+time*0.9-p.y*3.14*p.x)+p.x*(1.2+r)+fract(time*0.3-r)*TAU)*0.5;
        col += (rr+0.3)*0.141;
        col -= 0.571 * max( sin(xlv_TEXCOORD0.y*0.85 - time * 12.1)-r*0.79, 0.1);
        col *= clamp(mod(xlv_TEXCOORD0.x*xlv_TEXCOORD0.y, 1.0),0.85,1.0);
    vec2 q = xlv_TEXCOORD0.xy/resolution.xy;
    col *= 0.3 + 0.7*pow(16.0*q.x*q.y*(1.0-q.x)*(1.0-q.y),0.25);
	gl_FragColor = vec4( col, 1.0 );
}

