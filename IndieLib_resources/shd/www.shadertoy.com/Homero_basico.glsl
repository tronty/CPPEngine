#define U(a,b) (a.x*b.y-b.x*a.y)
// Distance to Bezier
// inspired by [iq:https://www.shadertoy.com/view/ldj3Wh]
// calculate distance to 2D bezier curve on xy but without forgeting the z component of p
// total distance is corrected using pytagore just before return
vec2 sdBezier(vec2 m, vec2 n, vec2 o, vec2 p) {
	vec2 q = p.xy;
	m-= q; n-= q; o-= q;
	float x = U(m, o), y = 2. * U(n, m), z = 2. * U(o, n);
	vec2 i = o - m, j = o - n, k = n - m, 
		 s = 2. * (x * i + y * j + z * k), 
		 r = m + (y * z - x * x) * vec2(s.y, -s.x) / dot(s, s);
	float t = clamp((U(r, i) + 2. * U(k, r)) / (x + x + y + z), 0.,1.); // parametric position on curve
	r = m + t * (k + k + t * (j - k)); // distance on 2D xy space
	return vec2(sqrt(dot(r, r) + p.z * p.z), t); // distance on 3D space
}

///por jorge2017a2
///Homero basico
//3-ago-2023-

#define antialiasing(n) n/min(iResolution.y,iResolution.x)
#define S(d,b) smoothstep(antialiasing(0.8),b,d)
#define S2(d,b) smoothstep(8.0*antialiasing(1.0),b,d)
#define PI 3.141516


float intersectSDF(float distA, float distB)
	{ return max(distA, distB);}
float unionSDF(float distA, float distB)
	{ return min(distA, distB);}
float differenceSDF(float distA, float distB)
	{ return max(distA, -distB);}


///oneshade    
vec2 Rotate(in vec2 p, in vec2 o, in float r) 
{   float c = cos(r), s = sin(r);
    return (p - o) * mat2(c, -s, s, c) + o;
}

float sdCircle( vec2 p, float r )
{return length(p) - r; }

float sdBox( in vec2 p, in vec2 b )
{ vec2 d = abs(p)-b; return length(max(d,0.0)) + min(max(d.x,d.y),0.0); }

float sdSegment( in vec2 p, in vec2 a, in vec2 b )
{
vec2 pa = p-a, ba = b-a;
float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
return length( pa - ba*h );
}

vec3 DrawFigBorde(vec3 pColObj, vec3 colOut, float distObj )
{ colOut = mix(colOut,pColObj ,S2( distObj,0.0));
colOut = mix(colOut,vec3(0.0) ,S2(abs( distObj)-0.001,0.0));
return colOut;
}

vec3 DrawFigSolo(vec3 pColObj, vec3 colOut, float distObj ) 
{  colOut = mix(colOut,pColObj ,S( distObj,0.0)); return colOut; }



vec3 lineasBz(vec2 uv, vec3 col)
{
vec2 pc1p1=vec2(3.650,3.770);
vec2 pc1p2=vec2(4.360,4.290);
vec2 pc1p3=vec2(4.490,3.480);
vec2 pc2p1=vec2(3.980,3.750);
vec2 pc2p2=vec2(4.650,4.280);
vec2 pc2p3=vec2(4.680,3.320);
vec2 pc3p1=vec2(2.630-0.05,1.730);
vec2 pc3p2=vec2(2.800-0.05,1.720);
vec2 pc3p3=vec2(2.710-0.05,1.570);
vec2 pc4p1=vec2(4.540,1.410);
vec2 pc4p2=vec2(4.560,1.730);
vec2 pc4p3=vec2(4.690,1.620);
vec2 pc5p1=vec2(2.890,1.870);
vec2 pc5p2=vec2(3.010,2.050);
vec2 pc5p3=vec2(3.220,2.020);
vec2 pc6p1=vec2(2.990,1.620);
vec2 pc6p2=vec2(2.820,1.760);
vec2 pc6p3=vec2(2.890,1.880);
vec2 pc7p1=vec2(3.230,2.010);
vec2 pc7p2=vec2(3.340,2.050);
vec2 pc7p3=vec2(3.460,2.080);
vec2 pc8p1=vec2(4.490,1.690-0.05);
vec2 pc8p2=vec2(4.520,1.860-0.05);
vec2 pc8p3=vec2(4.720,1.770-0.05);
vec2 pc9p1=vec2(4.440,1.340);
vec2 pc9p2=vec2(4.500,1.250);
vec2 pc9p3=vec2(4.610,1.280);
vec3 colNegro=vec3(0.0);
float d1= sdBezier(uv,pc1p1,pc1p2,pc1p3);
float d2= sdBezier(uv,pc2p1,pc2p2,pc2p3);
float d3= sdBezier(uv,pc3p1,pc3p2,pc3p3);
float d4= sdBezier(uv,pc4p1,pc4p2,pc4p3);
float d5= sdBezier(uv,pc5p1,pc5p2,pc5p3);
float d6= sdBezier(uv,pc6p1,pc6p2,pc6p3);
float d7= sdBezier(uv,pc7p1,pc7p2,pc7p3);
float d8= sdBezier(uv,pc8p1,pc8p2,pc8p3);
float d9= sdBezier(uv,pc9p1,pc9p2,pc9p3);
col=DrawFigBorde(colNegro,col,d1); //pelo1
col=DrawFigBorde(colNegro,col,d2); //pelo2
col=DrawFigBorde(colNegro,col,d3); //oreja
col=DrawFigBorde(colNegro,col,d4);
col=DrawFigBorde(colNegro,col,d5);
col=DrawFigBorde(colNegro,col,d6);
col=DrawFigBorde(colNegro,col,d7);
col=DrawFigBorde(colNegro,col,d8);
col=DrawFigBorde(colNegro,col,d9);
    return col;
}


vec3 Ojos(vec2 p, vec3 col)
{ float t=iTime;
float px=abs(0.5*sin(t));
float py=abs(0.15*sin(t*cos(t*0.25)))-0.1;

float r15=0.46;
vec2 p15=vec2(3.89,2.28);
float d15= sdCircle(p-p15,r15);

float r16=0.44;
vec2 p16=vec2(3.07,2.29);
float d16= sdCircle(p-p16,r16);

float r17=0.1;
vec2 p17=vec2(2.72+px,2.46+py);
float d17= sdCircle(p-p17,r17);

float r18=0.1;
vec2 p18=vec2(3.52+px,2.38+py);
float d18= sdCircle(p-p18,r18);
    col= DrawFigBorde(vec3(1.0),col,d16);
    col= DrawFigBorde(vec3(1.0),col,d15);
    col= DrawFigBorde(vec3(0.0),col,d17);
    col= DrawFigBorde(vec3(0.0),col,d18);
    return col;
}

vec3 pelos02(vec2 p, vec3 col)
{
vec2 A,B;
p.y=4.15-p.y;
A= vec2(4.36,2.47);
B= vec2(4.6,1.84);
float d1=sdSegment(p,A,B);
A= vec2(4.61,1.87);
B= vec2(4.64,2.37);
float d2=sdSegment(p,A,B);

A= vec2(4.62,2.36);
B= vec2(4.86,1.9);
float d3=sdSegment(p,A,B);
A= vec2(4.84,1.91);
B= vec2(4.77,2.31);
float d4=sdSegment(p,A,B);
    col= DrawFigBorde(vec3(0.0),col,d1);
    col= DrawFigBorde(vec3(0.0),col,d2);
    col= DrawFigBorde(vec3(0.0),col,d3);
    col= DrawFigBorde(vec3(0.0),col,d4);
    return col;
}

vec3 Homero(vec2 p, vec3 col)
{float r1=1.0;
vec2 p1=vec2(3.81,2.83);
float d1= sdCircle(p-p1,r1);

float r2=1.0;
vec2 p2=vec2(3.87,2.84);
float d2= sdCircle(p-p2,r2);

float r3=0.9;
vec2 p3=vec2(3.65,1.35);
float d3= sdCircle(p-p3,r3);

    d1=min(d1,d2);
    d1=min(d1,d3);
    vec3 colc=vec3(0.94,0.72,0.01);

vec2 pos=vec2(3.21,2.07);
vec2 med=vec2(0.425,1.025);
float d4= sdBox(p-pos, med );

float ang = 5.0 * PI / 180.0;
vec2 pr = Rotate(p- pos, med/2.0, ang);
d4 = sdBox(pr, med);
    d1=min(d1,d4);

pos=vec2(4.24,1.96);
med=vec2(0.425,1.025);
ang = 10.0 * PI / 180.0;
pr = Rotate(p- pos, med/2.0, ang);
float d5 = sdBox(pr, med);
    d1=min(d1,d5);
    //da,d2,d3,d4,d5 =cabeza
    
pos=vec2(3.87,0.62);
med=vec2(0.67,0.65);
float d6= sdBox(p-pos, med );
    d1=min(d1,d6); //cuello

float r7=0.6;
vec2 p7=vec2(3.62,0.91);
float d7= sdCircle(p-p7,r7);
    d1=min(d1,d7);// barba
    vec3 colb=vec3(0.85,0.76,0.67);

float r8=0.46;
vec2 p8=vec2(3.08,1.12);
float d8= sdCircle(p-p8,r8);
    d7=min(d7,d8); //barba

pos=vec2(3.4,1.1);
med=vec2(0.79,0.48);
float d9=sdEllipse(p-pos,med);
d7=min(d7,d9);    

pos=vec2(3.08,0.65);
med=vec2(0.16,0.13);
float d10=sdEllipse(p-pos,med); //barba

    d7=min(d7,d10);

pos=vec2(2.84,0.81);
med=vec2(0.31,0.13);
 ang = -5.0 * PI / 180.0;
 pr = Rotate(p-pos, med/2.0, ang);
float d11  = sdEllipse(pr, med);
    d7=min(d7,d11);

pos=vec2(2.76,0.87);
med=vec2(0.24,0.14);
ang = -5.0 * PI / 180.0;
 pr = Rotate(p-pos, med/2.0, ang);
float d12  = sdEllipse(pr, med);
    d7=min(d7,d12);

//oreja
pos=vec2(4.4,1.58);
med=vec2(0.25,0.19);
ang = -60. * PI / 180.0;
 pr = Rotate(p-pos, med/2.0, ang);
float d13  = sdEllipse(pr, med);
    d1=min(d1,d13);


pos=vec2(2.77,1.5);
med=vec2(0.25,0.19);
ang = 60.0 * PI / 180.0;
pr = Rotate(p-pos, med/2.0, ang);
float d14  = sdEllipse(pr, med);

float py=abs(0.05*sin(iTime));
float r19=0.08;
vec2 p19=vec2(3.09,0.65+py);
float d19= sdCircle(p-p19,r19);

pos=vec2(2.81,2.7);
med=vec2(0.17,0.18);
float d20=sdEllipse(p-pos,med);
    d1=min(d1,d20);

//nariz
pos=vec2(3.29,1.8);
med=vec2(0.4,0.2);
float d1a=sdEllipse(p-pos,med);

 pos=vec2(3.5,1.82);
 med=vec2(0.158,0.2);
 ang = -20.0 * PI / 180.0;
 pr = Rotate(p-pos, med, ang);
float d2a  = sdEllipse(pr, med);

 pos=vec2(3.14,1.82);
 med=vec2(0.25,0.18);
float d3a=sdEllipse(p-pos,med);
    d1a=min(d1a,d2a);
    d1a=min(d1a,d3a);
    
    col= DrawFigBorde(colc, col, d14);
    col= DrawFigBorde(colc, col, d1);
    col= DrawFigBorde(colb, col, d7);//barba
    col= DrawFigBorde(vec3(0.0),col,d19); //boca
    col=Ojos(p,col);
    col=lineasBz(p,col);
    col= DrawFigSolo(colc, col, d1a);
    col= pelos02(p,col);
    return col;
}

vec3 escena(vec2 p, vec3 col)
{ col= Homero(p,col);  return col; }

void main( )
{  vec2 uv = -1.0 + 2.0 * xlv_TEXCOORD0.xy/iResolution.xy;
    uv.x *= iResolution.x/iResolution.y;
    vec2 uv2=uv*5.0;
    uv-=vec2(-1.0,-0.5);
    uv*=4.0;
    vec3 col=vec3(0.,0.58,0.99);
    col=escena(uv,col);
    col=escena(vec2(-uv.x+4.0,uv.y)+vec2(sin(iTime),0.0),col);
    gl_FragColor = vec4(col,1.0);
}

