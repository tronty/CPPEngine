// Fancy ties by nimitz (twitter: @stormoid)
// https://www.shadertoy.com/view/ltfGDs
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License
// Contact the author for other licensing options

/*
	Somewhat complex modelling in a fully procedural shader that runs decently fast.
	I kinda cheated on the wings, the real ones are less hexagonal than this.
	Not doing proper occlusion checking for the lens flares to keep it fast.
*/

#define ITR 100
#define FAR  165. // 155.
#define time iTime

const float fov = 4. ; // De 1.5;    how to zoom

//Global material id (keeps code cleaner)
float matid = 0.;

//--------------------Utility, Domain folding and Primitives---------------------
float tri(in float x){return abs(fract(x)-.5);}
mat3 rot_x(float a){float sa = sin(a); float ca = cos(a); return mat3(1.,.0,.0,    .0,ca,sa,   .0,-sa,ca);}
mat3 rot_y(float a){float sa = sin(a); float ca = cos(a); return mat3(ca,.0,sa,    .0,1.,.0,   -sa,.0,ca);}
mat3 rot_z(float a){float sa = sin(a); float ca = cos(a); return mat3(ca,sa,.0,    -sa,ca,.0,  .0,.0,1.);}
vec3 rotz(vec3 p, float a){
    float s = sin(a), c = cos(a);
    return vec3(c*p.x - s*p.y, s*p.x + c*p.y, p.z);
}

/////////////////////////////////////////////////////////////
float hash(vec2 p){ float h = dot(p,vec2(127.1,311.7));
                   return fract(sin(h)*43758.5453) ; }
vec2 nmzHash22(vec2 q) {
    return vec2(hash(q),hash(q.xy));}
            
 
float hash(vec3 p){ float h = dot(p,vec3(127.1,311.7, 758.5453123));
                   return fract(sin(h)*43758.5453123); }
vec3 nmzHash33(vec3 p){
    return vec3(hash(p),hash(p.zyx),hash(p.yxz)); }
////////////////////////////////////////////////////////////////
//2dFoldings, inspired by Gaz/Knighty  see: https://www.shadertoy.com/view/4tX3DS
vec2 foldHex(in vec2 p)
{
    p.xy = abs(p.xy);
    const vec2 pl1 = vec2(-0.5, 0.8657);
    const vec2 pl2 = vec2(-0.8657, 0.5);
    p -= pl1*2.*min(0., dot(p, pl1));
    p -= pl2*2.*min(0., dot(p, pl2));
    return p;
}

vec2 foldOct(in vec2 p)
{
    p.xy = abs(p.xy);
    const vec2 pl1 = vec2(-0.7071, 0.7071);
    const vec2 pl2 = vec2(-0.9237, 0.3827);
    p -= pl1*2.*min(0., dot(p, pl1));
    p -= pl2*2.*min(0., dot(p, pl2));
    
    return p;
}

float sbox( vec3 p, vec3 b )
{
  vec3 d = abs(p) - b;
  return min(max(d.x,max(d.y,d.z)),0.0) + length(max(d,0.0));
}

float cyl( vec3 p, vec2 h )
{
  vec2 d = abs(vec2(length(p.xz),p.y)) - h;
  return min(max(d.x,d.y),0.0) + length(max(d,0.0));
}

float torus( vec3 p, vec2 t ){
  return length( vec2(length(p.xz)-t.x,p.y) )-t.y;
}

//using floor() in a SDF causes degeneracy.
float smoothfloor(in float x, in float k)
{
    float xk = x+k*0.5;
    return floor(xk-1.) + smoothstep(0.,k,fract(xk));
}

float hexprism(vec3 p, vec2 h){
    vec3 q = abs(p);
    return max(q.z-h.y,max((q.y*0.866025+q.x*0.5),q.x)-h.x);
}

// ------------------------------------------------------- adition
mat2 rot(float a) {
    float c = cos(a), s = sin(a);
    return mat2(c, s, -s, c);
}

float sdBox(vec3 p, vec3 b) {
    vec3 q = abs(p) - b;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}
/////////////////////////////////////////////------------ adition dr2
const float pi = 3.14159;
const vec4 cHashA4 = vec4 (0., 1., 57., 58.);
const vec3 cHashA3 = vec3 (1., 57., 113.);
const float cHashM = 43758.54;

vec4 Hashv4f (float p)
{
  return fract (sin (p + cHashA4) * cHashM);
}

vec4 Hashv4v3 (vec3 p)
{
  const vec3 cHashVA3 = vec3 (37.1, 61.7, 12.4);
  const vec3 e = vec3 (1., 0., 0.);
  return fract (sin (vec4 (dot (p + e.yyy, cHashVA3), dot (p + e.xyy, cHashVA3),
     dot (p + e.yxy, cHashVA3), dot (p + e.xxy, cHashVA3))) * cHashM);
}

float Noisefv2 (vec2 p)
{
  vec2 i = floor (p);
  vec2 f = fract (p);
  f = f * f * (3. - 2. * f);
  vec4 t = Hashv4f (dot (i, cHashA3.xy));
  return mix (mix (t.x, t.y, f.x), mix (t.z, t.w, f.x), f.y);
}

float Noisefv3a (vec3 p)
{
  vec3 i, f;
  i = floor (p);  f = fract (p);
  f *= f * (3. - 2. * f);
  vec4 t1 = Hashv4v3 (i);
  vec4 t2 = Hashv4v3 (i + vec3 (0., 0., 1.));
  return mix (mix (mix (t1.x, t1.y, f.x), mix (t1.z, t1.w, f.x), f.y),
              mix (mix (t2.x, t2.y, f.x), mix (t2.z, t2.w, f.x), f.y), f.z);
}

float Fbm3 (vec3 p)
{
  const mat3 mr = mat3 (0., 0.8, 0.6, -0.8, 0.36, -0.48, -0.6, -0.48, 0.64);
  float f, a, am, ap;
  f = 0.;  a = 0.5;
  am = 0.5;  ap = 4.;
  p *= 0.5;
  for (int i = 0; i < 6; i ++) {
    f += a * Noisefv3a (p);
    p *= mr * ap;  a *= am;
  }
  return f;
}

float Fbmn (vec3 p, vec3 n)
{
  vec3 s = vec3 (0.);
  float a = 1.;
  for (int i = 0; i < 5; i ++) {
    s += a * vec3 (Noisefv2 (p.yz), Noisefv2 (p.zx), Noisefv2 (p.xy));
    a *= 0.5;
    p *= 2.;
  }
  return dot (s, abs (n));
}

vec3 VaryNf (vec3 p, vec3 n, float f)
{
  vec3 e = vec3 (0.2, 0., 0.);
  float s = Fbmn (p, n);
  vec3 g = vec3 (Fbmn (p + e.xyy, n) - s,
     Fbmn (p + e.yxy, n) - s, Fbmn (p + e.yyx, n) - s);
  return normalize (n + f * (g - n * dot (n, g)));
}

float SmoothBump (float lo, float hi, float w, float x)
{
  return (1. - smoothstep (hi - w, hi + w, x)) * smoothstep (lo - w, lo + w, x);
}

vec2 Rot2D (vec2 q, float a)
{
  return q * cos (a) * vec2 (1., 1.) + q.yx * sin (a) * vec2 (-1., 1.);
}

float PrSphDf (vec3 p, float s)
{
  return length (p) - s;
}

float PrCapsDf (vec3 p, float r, float h)
{
  return length (p - vec3 (0., 0., h * clamp (p.z / h, -1., 1.))) - r;
}

float PrCylDf (vec3 p, float r, float h)
{
  return max (length (p.xy) - r, abs (p.z) - h);
}

float PrFlatCylDf (vec3 p, float rhi, float rlo, float h)
{
  return max (length (p.xy - vec2 (rhi *
     clamp (p.x / rhi, -1., 1.), 0.)) - rlo, abs (p.z) - h);
}

float PrRoundBoxDf (vec3 p, vec3 b, float r) {
  return length (max (abs (p) - b, 0.)) - r;
}

int idObj;
mat3 flMat;
vec3 flPos, qHit, qHitTransObj, sunDir;
float fusLen, wSpan, flyVel, tCur;
const float dstFar = 350.;
const int idFus = 11, idPipe = 12, idWing = 13, idStrut = 14,
   idHstab = 15, idFin = 16, idLeg = 17, idAxl = 18, idWhl = 19,
   idNose = 20, idCkpit = 21, idPlt = 22, idWing1= 25 ,idWhl2 =26  ;


//------------------------------------------------------------------------

vec3 position(in vec3 p)
{
    float dst = 2.5 ; //   de 7.;
    float id =  floor (p.z/dst*.1);  // body expand  or shrink
    p.xy += (sin(id*10.+time));          // flat flight
    p.z += abs(sin(id*10.+time*0.9))*.5;
    p = rotz(p,sin(time*0.5)*1.+id*0.1);   // no rotation
    p.z = (abs(p.z)-dst) *sign(p.z) ;
    return p;
}

float map(vec3 p)
{
                            
    ////////////////////////////////////////////////////////////  
    // -------------------------------------------- dr2 Yellow Moth airplane ad
    float dMin=dstFar;
    vec3 q, qq;  matid=1.;
  float d, wr;
    p *=0.65 ;
  q = p;
  wr = -0.2 + q.z / fusLen;
  d = PrRoundBoxDf (q, vec3 (0.07 * (1. - 0.8 * wr * wr),
     0.11 * (1. - 0.6 * wr * wr), 1.) * fusLen, 0.05 * fusLen);
  q -= vec3 (0., 0.1, 0.3) * fusLen;
  d = max (d, - PrRoundBoxDf (q, vec3 (0.05, 0.1, 0.15) * fusLen,
     0.03 * fusLen)); 
    
  if (d < dMin) { dMin = min (dMin, d);  idObj = idFus;  qHit = q; }// idFus=body
    
  q = p;  q -= vec3 (0., 0.08, 0.3) * fusLen;
//  d = PrRoundBoxDf (q, vec3 (0.05, 0.02, 0.15) * fusLen, 0.03 * fusLen); 
    d =min(d,PrRoundBoxDf (q, vec3 (0.05, 0.02, 0.15) * fusLen, 0.03 * fusLen)); 
 // if (d < dMin) { dMin = min (dMin, d);  idObj = idCkpit;  qHit = q; }
  q = p;  q.z = abs (q.z - 0.33 * fusLen) - 0.08 * fusLen;
  q -= vec3 (0., 0.17, 0.) * fusLen;
//  d = PrSphDf (q, 0.04 * fusLen); 
    d =min(d,PrSphDf (q, 0.04 * fusLen)); 
//  if (d < dMin) { dMin = min (dMin, d);  idObj = idPlt;  qHit = q; }
    
  q = p - fusLen * vec3 (0., 0.03, 0.8);
  q.x = abs (q.x) - 0.1 * fusLen;
//  d = PrCapsDf (q, 0.02 * fusLen, 0.15 * fusLen);
    d =min(d,PrCapsDf (q, 0.02 * fusLen, 0.15 * fusLen));
  //  if (d < dMin) { dMin = min (dMin, d);  idObj = idPipe;  qHit = q; }
    
  q = p - fusLen * vec3 (0., 0.03, 1.05);
//  d = PrCapsDf (q, 0.05 * fusLen, 0.02 * fusLen);
    d =min(d,PrCapsDf (q, 0.05 * fusLen, 0.02 * fusLen));
 //  if (d < dMin) { dMin = d;  idObj = idNose;  qHit = q; }
    //---------------------------------------------------- Propel
    
    
    //------------------------------------------------------
    
    
  q = p - fusLen * vec3 (0., 0.09, 0.2);
  qq = q;  qq.y = abs (qq.y) - 0.21 * fusLen;
  wr = q.x / wSpan;
    //-------------------------------------------------- check wing color
  float d1 = PrFlatCylDf (qq.zyx, 0.24 * (1. - 0.2 * wr * wr) * fusLen,
     0.01 * (1. - 0.8 * wr * wr) * fusLen, wSpan);
    if (d1 < dMin) { dMin = min (dMin, d1);  idObj = idWing1;  qHit = q; }
    //---------------------------------------------------
    
    d =min(d,PrFlatCylDf (qq.zyx, 0.24 * (1. - 0.2 * wr * wr) * fusLen,
          0.01 * (1. - 0.8 * wr * wr) * fusLen, wSpan));                     
  if (d < dMin) { dMin = min (dMin, d);  idObj = idWing;  qHit = q; }
    
  q = p - fusLen * vec3 (0., 0.09, 0.25);
  q.xz = abs (q.xz) - fusLen * vec2 (0.5, 0.1);
 // d = PrCylDf (q.xzy, 0.01 * fusLen, 0.21 * fusLen);
    d =min(d,PrCylDf (q.xzy, 0.01 * fusLen, 0.21 * fusLen));
 // if (d < dMin) { dMin = min (dMin, d);  idObj = idStrut;  qHit = q; }
    
  q = p - fusLen * vec3 (0., 0.15, 0.25);
  q.x = abs (q.x) - 0.1 * fusLen;
//  d = PrCylDf (q.xzy, 0.01 * fusLen, 0.15 * fusLen);
    d =min(d, PrCylDf (q.xzy, 0.01 * fusLen, 0.15 * fusLen));
 // if (d < dMin) { dMin = min (dMin, d);  idObj = idStrut;  qHit = q; }
    
    //////////////////////////////////////////////////pennding
  float tSpan = 0.35 * wSpan;
  q = p - fusLen * vec3 (0., 0., - 0.9);
  wr = q.x / tSpan;

  d =min(d,PrFlatCylDf (q.zyx, 0.15 * (1. - 0.25 * wr * wr) * fusLen,
     0.007 * (1. - 0.2 * wr * wr) * fusLen, tSpan));

  q.x = abs (q.x);
  d = max (d, 0.02 * fusLen - 1.5 * q.x - q.z);
 //   d =min(d,max (d, 0.02 * fusLen - 1.5 * q.x - q.z));
 // if (d < dMin) { dMin = min (dMin, d);  idObj = idHstab;  qHit = q; }
   //
    /////////////////////////////////////////////////pennding
    
  float fSpan = 0.32 * wSpan;
  q = p - fusLen * vec3 (0., 0.2, - 0.87);
  q.yz = Rot2D (q.yz, 0.35);
  wr = q.y / fSpan;
//  d = PrFlatCylDf (q.zxy, 0.15 * (1. - 0.3 * wr * wr) * fusLen,
//     0.007 * (1. - 0.3 * wr * wr) * fusLen, fSpan);
    
    d =min(d,PrFlatCylDf (q.zxy, 0.115 * (1. - 0.3 * wr * wr) * fusLen,
          0.007 * (1. - 0.3 * wr * wr) * fusLen, fSpan*0.4));                
 //----------------------------------------------------------   ad Fin color
    if (d < dMin) { dMin = min (dMin, d);  idObj = idFin;  qHit = q; }
  //---------------------------------------------------------  
  //  d = max (d,  -0.2*q.y-0.1);           //  ???? cut by y axis=0. plane

    
//  if (d < dMin) { dMin = min (dMin, d);  idObj = idFin;  qHit = q; }
 
   q = p - fusLen * vec3 (0., -0.25, 0.5);
  q.x = abs (q.x) - 0.14 * fusLen;
  q.xy = Rot2D (q.xy, -0.55);  q.yz = Rot2D (q.yz, 0.15);
 // d = PrCylDf (q.xzy, 0.013 * fusLen, 0.12 * fusLen);
    d =min(d,PrCylDf (q.xzy, 0.013 * fusLen, 0.12 * fusLen));

//  if (d < dMin) { dMin = d;  idObj = idLeg;  qHit = q; }
    
  q = p - fusLen * vec3 (0., -0.34, 0.515);
  q.x = abs (q.x) - 0.22 * fusLen;
 // d = PrCylDf (q.yzx, 0.01 * fusLen, 0.035 * fusLen);
    d =min(d,PrCylDf (q.yzx, 0.01 * fusLen, 0.035 * fusLen));

//  if (d < dMin) { dMin = d;  idObj = idAxl;  qHit = q; }
    
  q.x -= 0.01 * fusLen;
    //---------------------------------------------- Whl color ad
    float d2 = PrCylDf (q.yzx, 0.1 * fusLen, 0.015 * fusLen);
       if (d2 < dMin) { dMin = d2;  idObj = idWhl2;  qHit = q; }   
    //------------------------------------------------------
    
    d =min(d,PrCylDf (q.yzx, 0.1 * fusLen, 0.015 * fusLen));
    //-------------------------------------------------------
    
//  if (d < dMin) { dMin = d;  idObj = idWhl;  qHit = q; }  
    
    //	----------------------------------------------------Propela
   // q=p;
    q =p-fusLen*vec3(0.,0.02,1.07);
     q.xy *=rot(time*38.0);     // Propela rotation
   
    
     d =min(d, PrRoundBoxDf(q,vec3(0.35*fusLen,0.013*fusLen,0.007*fusLen),0.002));
   //------------------------------------- the second blade ad failure
    
  
    
  return  d  /*dMin */;
    
    
}

float march(in vec3 ro, in vec3 rd)
{
	float precis = 0.001;
    float h=precis ; // *2.0;
    float d = 0.;
    for( int i=0; i< 100 /*ITR */; i++ )
    {
        if( abs(h)<precis || d>FAR ) break;
        d += h;
        float res = map(position(ro+rd*d)) ; //*0.93;
        h = res;
    }
	return d;
}

//greeble-ish texture


//---------------------------------------- change

vec3 normal(const in vec3 p)
{  
    vec2 e = vec2(-1., 1.)*0.008;
	return normalize(e.yxx*map(p + e.yxx) + e.xxy*map(p + e.xxy) + 
					 e.xyx*map(p + e.xyx) + e.yyy*map(p + e.yyy) );   
}
   
//----------------------------------------

//form iq
float getAO( in vec3 pos, in vec3 nor )
{
	float occ = 0.0;
    float sca = 1.0;
    for( int i=0; i<5; i++ )
    {
        float hr = 0.01 + 0.13*float(i)/3.;
        vec3 aopos =  nor * hr + pos;
        float dd = map( aopos );
        occ += -(dd-hr)*sca;
        sca *= 0.95;
    }
    return clamp( 1. - 3.5*occ, 0.0, 1.0 );    
}

//smooth and cheap 3d starfield
vec3 stars(in vec3 p)
{
    vec3 c = vec3(0.);
    float res = iResolution.x*.85*fov;
    
    //Triangular deformation (used to break sphere intersection pattterns)
    p.x += (tri(p.z*50.)+tri(p.y*50.))*0.006;
    p.y += (tri(p.z*50.)+tri(p.x*50.))*0.006;
    p.z += (tri(p.x*50.)+tri(p.y*50.))*0.006;
    
	for (float i=0.;i<3.;i++)
    {
        vec3 q = fract(p*(.15*res))-0.5;
        vec3 id = floor(p*(.15*res));
        float rn = nmzHash33(id).z;
        float c2 = 1.-smoothstep(-0.2,.4,length(q));
        c2 *= step(rn,0.005+i*0.014);
        c += c2*(mix(vec3(1.0,0.75,0.5),vec3(0.85,0.9,1.),rn*30.)*0.5 + 0.5);
        p *= 1.15;
    }
    return c*c*1.5;
}

vec3 flare(in vec2 p, in vec2 pos, in vec3 lcol, in float sz)
{
	vec2 q = p-pos;
    q *= sz;
	float a = atan(q.x,q.y);
    float r = length(q);
    
    float rz= 0.;
    rz += .07/(length((q)*vec2(7.,200.))); //horiz streaks
    rz += 0.3*(pow(abs(fract(a*.97+.52)-0.5),3.)*(sin(a*30.)*0.15+0.85)*exp2((-r*5.))); //Spokes
	
    vec3 col = vec3(rz)*lcol;   
    col += exp2((1.-length(q))*50.-50.)*lcol*vec3(3.);
    col += exp2((1.-length(q))*20.-20.)*lcol*vec3(1,0.95,0.8)*0.5;    
    return clamp(col,0.,1.);
}


//A weird looking small moon
float slength(in vec2 p){ return max(abs(p.x), abs(p.y)); }
float moontex(in vec3 p)
{
    float r = length(p);
    vec3 q = vec3(r, acos(p.y/r), atan(p.z,p.x));
    q *= 6.5;
    vec3 bq = q;
    q.y = q.y*0.44-0.42;
    vec2 id = floor(q.zy);
    vec2 s = fract(q.zy)-0.5;
    
    float rz = 1.;
    float z = 0.25;
    for(int i=0;i<=3;i++)
    {
        vec2 rn = nmzHash22(id+vec2(i)+0.0019)*.6 + 0.4;
        s -= abs(s)-rn*0.45;
        rz -= smoothstep(0.5,0.45-float(i)*0.1,slength(s*rn*1.3))*z;
        q *= 3.5;
        z *= .85;
        id = floor(q.zy);
    	s = fract(q.zy)-0.5;
    }
    
    rz -= smoothstep(0.035,.03,abs(bq.y-10.15))*.3; //main trench
    return rz;
}

float sphr(in vec3 ro, in vec3 rd, in vec4 sph)
{
	vec3 oc = ro - sph.xyz;
	float b = dot(oc,rd);
	float c = dot(oc,oc) - sph.w*sph.w;
	float h = b*b - c;
	if (h < 0.) return -1.;
	else return -b - sqrt(h);
}

void main( )
{	
	vec2 p = xlv_TEXCOORD0.xy/iResolution.xy-0.5;
	p.x*=iResolution.x/iResolution.y;
	vec2 mo = iMouse.xy / iResolution.xy-.5;
    mo = (mo==vec2(-.5))?mo=vec2(-0.15,0.):mo;
	mo.x *= iResolution.x/iResolution.y;
    mo*=4.;
	mo.x += time*0.17+0.1;
    
    fusLen=1.;  wSpan=1.2*fusLen ;

    vec3 ro = vec3(0.,0.,17.);
    vec3 rd = normalize(vec3(vec2(-p.x,p.y),-fov));
    float cms = 1.-step(sin((time+0.0001)*0.5),0.);
    mat3 inv_cam = mat3(0);
    
    if ( mod(floor(time),40.)<20.0 /* cms < 0.5  */) // time interval change
    {
        mat3 cam = rot_x(-mo.y)*rot_y(-mo.x);
        inv_cam = rot_y(-mo.x)*rot_x(mo.y); 
        ro *= cam;rd *= cam;
    }
    else
    {
        float frct = fract(time*0.105915);  // de time*0.15915=1.=> time =7.1sec
        float frct2 = fract(time*0.15915+0.50001);
        float cms = 1.-step(sin((time+0.0001)*0.5),0.);
        ro = vec3(-15.,1.-(step(frct2,0.5))*frct2*10.,140.-frct*280.);
        vec3 ta = vec3(0);
    
        
        vec3 fwd = normalize(ta - ro);
        vec3 rgt = normalize(cross(vec3(0., 1., 0.), fwd ));
        vec3 up = normalize(cross(fwd, rgt));
        mat3 cam = mat3(rgt,up,-fwd);
        rd = normalize(vec3(vec2(p.x,p.y),-fov))*cam;
        inv_cam = transpose(cam);
    }
    
	float rz = march(ro,rd);
	
    vec3 lgt = normalize( vec3(.2, 0.1535, 0.7) ); // pos of sun
    vec3 col = vec3(0.0);
    float sdt = max(dot(rd,lgt),0.); 
    
    vec3 lcol = vec3(1,.85,0.73);
    col += stars(rd);
    
    vec3 fp = (-lgt*inv_cam);
    col += clamp(flare(p,-fp.xy/fp.z*fov*1.5/fov, lcol,0.5)*fp.z*1.1,0.,1.);
    
    //Another nearby star       
    vec3 lcol2 = vec3(0.25,0.66,0.234*sin(time*0.5));          // de (0.25,.38,1); blue+white
    vec3 lgt2 = normalize(vec3(-0.2,-.1,-0.8));   // pos of planet
    fp = (-lgt2*inv_cam);
    col += clamp(flare(p,-fp.xy/fp.z*fov*1.5/fov, lcol2,0.5*(1.+0.3*sin(time)))*fp.z*1.1,0.,1.);
    
    //A "moon"  /////////////////////////////////////////
    vec4 sph = vec4(2500,500,-00,1800);
    float mn = sphr(ro,rd,sph);
    
    if (mn > 0.)
    {
        vec3 pos = ro+rd*mn;
        vec3 nor = normalize(pos-sph.xyz);
        vec3 dif = clamp(dot( nor, lgt ), 0., 1.)*0.985*lcol;
        vec3 bac = clamp( dot( nor, lgt2), 0.0, 1.0 )*lcol2;
        col = moontex((pos-sph.xyz))*vec3(0.852,0.754 /**sin(time*0.4)*/,0.7)*0.93;
        col *= dif + bac*0.01 + 0.005;
    }
    
    
    if ( rz < FAR  )
    {
        float mat = matid;
        vec3 pos = ro+rz*rd;
        pos = position(pos);
        vec3 nor= normal(pos);
        float dif = clamp( dot( nor, lgt ), 0.0, 1.0 );
        float bac = clamp( dot( nor, lgt2), 0.0, 1.0 );
        float spe = pow(clamp( dot( reflect(rd,nor), lgt ), 0.0, 1.0 ),7.);
        float fre = pow( clamp(1.0+dot(nor,rd),0.0,1.0), 3.0 );
        vec3 brdf = /*vec3(0) ;*/ vec3 (0.95,0.904,0.02);
        brdf += bac*mix(lcol2,vec3(1),0.5)*0.06;
        brdf += 1.5*dif*lcol;
        col = vec3(0.52954,0.7956,0.2965)*1.5;
        col *= col;
        
        if(idObj ==idFus ) {col =vec3(0.,0.18,0.568) ; }
        else if(idObj ==idWing1 ){col =vec3(0.94,0.5045,0.753000) ; 
                                  
                //   brdf *= 0.510045;
               //    spe *= 0.135;
              //     fre *= 0.05;   
                /*     col=col*brdf+spe+fre; */   }  // check
        else if(idObj ==idFin ) {col=vec3(0.8,1.,0.3);    }
        else if(idObj ==idWhl2   ) {col =vec3(0.5,0.6,0.4) ;   }
        
 
        
        col = col*brdf + spe*.23 +.03*fre;
        col *= getAO(pos,nor);
    }
    
    col = clamp(col, 0.,1.);
    col = pow(clamp(col,0.,1.), vec3(0.416667))*1.055 - 0.055; //sRGB
	
	gl_FragColor = vec4( col, 1.0 );
}
