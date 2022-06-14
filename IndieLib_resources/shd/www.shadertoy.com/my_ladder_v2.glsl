//----------image
//por jorge2017a2-
//my ladder v2....12-jun-2022

//referencia
//https://iquilezles.org/articles/distfunctions2d/
//https://iquilezles.org/articles/distfunctions/

#define MAX_STEPS 100
#define MAX_DIST 100.
#define MIN_DIST 0.001
#define EPSILON 0.001
#define Ka 0.5
#define Kd 0.4
vec3 GetColorYMaterial(vec3 p,  vec3 n, vec3 ro,  vec3 rd, int id_color, float id_material);
vec3 getMaterial( vec3 pp, float id_material);
vec3 light_pos1;  vec3 light_color1 ;
vec3 light_pos2;  vec3 light_color2 ;

//operacion de Union  por FabriceNeyret2
#define opU2(d1, d2) ( d1.x < d2.x ? d1 : d2 )

float sdBox( vec3 p, vec3 b )
	{ vec3 d = abs(p) - b;   return length(max(d,0.0))+ min(max(d.x,max(d.y,d.z)),0.0); }


float Intersect(float distA, float distB)
	{ return max(distA, distB);}
float Union(float distA, float distB)
	{ return min(distA, distB);}
float Difference(float distA, float distB)
	{ return max(distA, -distB);}

float opRep1D( float p, float c )
	{ float q = mod(p+0.5*c,c)-0.5*c; return  q ;}

vec3 rotate_x(vec3 p, float phi)
{   float c = cos(phi);	float s = sin(phi);
    return vec3(p.x, c*p.y - s*p.z, s*p.y + c*p.z);
}
vec3 rotate_y(vec3 p, float phi)
{	float c = cos(phi);	float s = sin(phi);
	return vec3(c*p.x + s*p.z, p.y, c*p.z - s*p.x);
}
vec3 rotate_z(vec3 p, float phi)
{	float c = cos(phi);	float s = sin(phi);
	return vec3(c*p.x - s*p.y, s*p.x + c*p.y, p.z);
}

float dot2( in vec2 v ) { return dot(v,v); }
float dot2( in vec3 v ) { return dot(v,v); }
float ndot( in vec2 a, in vec2 b ) { return a.x*b.x - a.y*b.y; }


float sdStairs( in vec2 p, in vec2 wh, in float n )
{
    // base
    vec2 ba = wh*n;
    float d = min(dot2(p-vec2(clamp(p.x,0.0,ba.x),0.0)), 
                  dot2(p-vec2(ba.x,clamp(p.y,0.0,ba.y))) );
    float s = sign(max(-p.y,p.x-ba.x) );

    // steps repetition
#if 1
    float dia = length(wh);
    p = mat2(wh.x,-wh.y, wh.y,wh.x)*p/dia;
    float lid = clamp(round(p.x/dia),0.0,n-1.0);
    p.x = p.x - lid*dia;
    p = mat2(wh.x, wh.y,-wh.y,wh.x)*p/dia;
#else
    float dia2 = dot(wh,wh);
    p = mat2(wh.x,-wh.y,wh.y,wh.x)*p;
    float lid = clamp(round(p.x/dia2),0.0,n-1.0);
    p.x = p.x - lid*dia2;
    p = mat2(wh.x,wh.y,-wh.y,wh.x)*p/dia2;
#endif    

    // single step
    float hh = wh.y/2.0;
    p.y -= hh;
    
    if( p.y>hh*sign(p.x) ) s=1.0;
    p = (lid<0.5 || p.x>0.0) ? p : -p;

    d = min( d, dot2(p-vec2(0.0,clamp(p.y,-hh,hh))) );
    d = min( d, dot2(p-vec2(clamp(p.x,0.0,wh.x),hh)) );
    
    return sqrt(d)*s;
}

float opScaleEscalera( in vec3 p )
{   float s=0.125;
    return sdStairs(p.xy/s, vec2(10.0,10.0), 10.0 )*s;
}

vec2 DosParedes(vec3 p)
{	vec2 res= vec2(9999.0, -1.0);  vec3 p0=p;
   float d1=opScaleEscalera( p-vec3(-5.0,5.0,0.0));
    float d2= sdBox(p-vec3(0.0,15.0,0.0), vec3(10.0,10.0,2.0) );
    float d3= sdBox(p-vec3(0.0,-4.0,0.0), vec3(7.5,10.0,2.0) );
    d1= Intersect(d1, d2);
    d1= Union(d1, d3);
    res =opU2(res, vec2(d1,100.0));
    return res;
}

vec2 UnCuarto(vec3 p)
{	vec2 res= vec2(9999.0, -1.0);  vec3 p0=p;
     p.x=abs(p.x)-5.0;
    p.z=abs(p.z)-10.0;
    vec2 dp1= DosParedes(p);
    p=p0;
    p.z=abs(p.z)-5.0;
    p.x=abs(p.x)-10.0;
    vec2 dp2= DosParedes(p.zyx);
    res =opU2(res, dp1);
    res =opU2(res, dp2);
    return res;
}

vec2 cuartoEscala(vec3 p, float s,float pmat)
{	vec2 res= vec2(9999.0, -1.0);  vec3 p0=p;
    vec2 r2= UnCuarto(p/s);
    r2.x*=s;r2.y=pmat;
    res =opU2(res, r2);
    return res;
}

vec2 GetDist(vec3 p  ) 
{	vec2 res= vec2(9999.0, -1.0);  vec3 p0=p;
	float planeDist1 = p.y+14.0;  //piso inf
    vec2 uc1= UnCuarto(p);
    vec2 uc2=cuartoEscala(p-vec3(0.0,10.0,0.0), 3./4.0,101.);
    vec2 uc3=cuartoEscala(p-vec3(0.0,20.0,0.0), 2./4.0,102.);
    res =opU2(res, uc1);
    res =opU2(res, uc2);
    res =opU2(res, uc3);
    return res;
}

vec3 GetNormal(vec3 p)
{   float d = GetDist(p).x;
    vec2 e = vec2(.001, 0);
    vec3 n = d - vec3(GetDist(p-e.xyy).x,GetDist(p-e.yxy).x,GetDist(p-e.yyx).x);
    return normalize(n);
}

vec2 RayMarch(vec3 ro, vec3 rd, int PMaxSteps)
{   vec3 p;
    vec2 hit, object=vec2(0.1,0.0);
    for(int i=0; i <= PMaxSteps; i++) 
    { p = ro + rd*object.x;
      hit = GetDist(p);
      object.x += hit.x;
      object.y = hit.y;
      if (abs(hit.x) < EPSILON || object.x > MAX_DIST) break;
    }    
    return object;
}


float getSoftShadow(vec3 p, vec3 lightPos) {
    float res = 9999.0;
    float dist = 0.01;
    float lightSize = 0.03;
    for (int i = 0; i < MAX_STEPS; i++) {
      float hit = GetDist(p + lightPos * dist).x;
      res = min(res, hit / (dist * lightSize));
      dist += hit;
      if (hit < 0.0001 || dist > 60.0) break;
    }
    return clamp(res, 0.0, 1.0);
}

float occlusion(vec3 pos, vec3 nor)
{   float sca = 2.0, occ = 0.0;
    for(int i = 0; i < 10; i++) {    
      float hr = 0.01 + float(i) * 0.5 / 4.0;
      float dd = GetDist(nor * hr + pos).x;
      occ += (hr - dd)*sca;
      sca *= 0.6;
    }
    return clamp( 1.0 - occ, 0.0, 1.0 );    
}

vec3 lightingv3(vec3 normal,vec3 p, vec3 lp, vec3 rd, vec3 ro,vec3 lightColor, float t) 
{   vec3 lightPos=lp;
    vec3 worldPos = p;
    vec3 V = -rd;
    vec3 N = normal;
    vec3 L = normalize (lightPos - worldPos);
    vec3 R = reflect (-L, N);

    float lightDist = max(length(L), .001);
    float atten=1.0 / (1.0 + lightDist * 0.125 + lightDist * lightDist * .05);
    L /= (lightDist*atten);

    float shadow = getSoftShadow(worldPos, L);// shadows
        
    float occ = occlusion(worldPos, N);// occ
    vec3 ambient = Ka + Ka * dot(normal, vec3(0., 1., 0.))*lightColor;
    ambient*=0.5;

    vec3 fresnel =  lightColor *  pow(clamp(1.0 + dot(rd, N), 0.0, 1.0), 2.0);;
    float diff= clamp(dot(N, L), 0.0, 1.0);
    vec3 diffuse =  lightColor * diff;
    float shininess=10.0;
    float specular    = pow(max(dot(R, V), 0.0), shininess);

    vec3 back = 0.5 * lightColor * clamp(dot(N, -L), 0.0, 1.0); // back
    vec3 colOut = occ*lightColor*(ambient+diffuse*shadow+.25 +back) + vec3(.7,.9,1)*specular*specular;
    return colOut;
}

vec3 getColorTextura( vec3 p, vec3 nor,  int i)
{	vec3 col;
    if (i==100 )
        { col=tex3D(iChannel0, p/32., nor); return col*2.0; }
	if (i==101 ) 
        { col=tex3D(iChannel0, p/32., nor); return hsv(col*vec3(0.7,0.7,0.3))*4.0; }
	if (i==102 ) 
        { col=tex3D(iChannel0, p/32., nor); return hsv(col*vec3(0.2,0.9,0.6))*3.0; }
}

vec3 chequer(vec2 uv) 
{    return vec3(sin(uv.x*3.1416*3.),sin(uv.x*3.1416*2.),sin(uv.x*3.1416*1.)); }

vec3 render_sky_color(vec3 rd)
{   float t = (rd.x + 1.0) / 2.0;
    vec3 col= vec3((1.0 - t) + t * 0.3, (1.0 - t) + t * 0.5, (1.0 - t) + t);
    vec3  sky = mix(vec3(.0, .1, .4)*col, vec3(.3, .6, 1.2), 1.0 - rd.y);
    sky= chequer(sky.xy*0.5);
    return sky;
}

vec3 GetMaterial(vec3 p,  vec3 nor, vec3 ro,  vec3 rd, int id_color)
{  	vec3 colobj; 
     if (id_color>=100 ){ return  getColorTextura( p, nor,id_color); }      
}

vec3 linear2srgb(vec3 c) 
{ return mix(12.92 * c,1.055 * pow(c, vec3(1.0/1.8)) - 0.055, step(vec3(0.0031308), c)); }

vec3 exposureToneMapping(float exposure, vec3 hdrColor) 
{ return vec3(1.0) - exp(-hdrColor * exposure); }

vec3 ACESFilm(vec3 x)
{   float a,b,c,d,e;
    a = 2.51; b = 0.03; c = 2.43;d = 0.59; e = 0.14;
    return (x*(a*x+b))/(x*(c*x+d)+e);
}

vec3 Render(vec3 ro, vec3 rd)
{  vec3 col = vec3(0);
  vec3 p;
     vec2 hit=RayMarch(ro,rd, MAX_STEPS);
      if(hit.x<MAX_DIST)
       {   p = (ro + rd * hit.x );
        vec3 nor=GetNormal(p);
        vec3 colobj;
        colobj=GetMaterial( p, nor, ro, rd,  int(hit.y));
        vec3 result;
         result= lightingv3(nor, p,light_pos1, rd,ro,colobj,hit.x)*light_color1;
        result+= lightingv3(nor, p,light_pos2,rd, ro,colobj,hit.x)*light_color2;
        col= result/2.0;
        col= (ACESFilm(col)+linear2srgb(col)+col+ exposureToneMapping(3.0, col))/4.0 ;
        
    }
    else if(hit.x>MAX_DIST)
    col= render_sky_color(rd);
   return col;
}

Ray RotarEnCirculo(vec3 ro,vec3 rd)
{
   float t=iTime*0.5;
   float r=30.0-abs(10.0*sin(t));
   float veltime=30.0;  
    float tt = radians( -t*veltime);
    vec3  pos=vec3(0.0,0.0,25.0);
    vec3 rotation1 = pos+vec3(r*sin(tt), 0.0, r*cos(tt));
    ro +=rotation1; 
    rd=rotate_y( vec3(-rd.x,rd.y,-rd.z), tt);
    return  Ray(ro,rd);
}


void main()
{  vec2 uv = (xlv_TEXCOORD0-.5*iResolution.xy)/iResolution.y;
    float t;
    t=mod(iTime*1.0,360.0);
 	light_pos1= vec3(-10.0, 50.0, 25.); light_color1=vec3( 1.0,1.0,1.0 );
 	light_pos2= vec3(10.0, 20.0, -25.0 ); light_color2 =vec3( 1.0,1.0,1.0 ); 
    vec3 ro=vec3(0.0,20.0,-25.0);
   vec3 rd=normalize( vec3(uv.x,uv.y,1.0));
   Ray ray= RotarEnCirculo(ro,rd);
    ro=ray.ro;
    rd=ray.rd;
    //light_pos1+=ro;
    //light_pos2+=ro;
    vec3 col= Render( ro,  rd);
    gl_FragColor = vec4(col,1.0);
}

