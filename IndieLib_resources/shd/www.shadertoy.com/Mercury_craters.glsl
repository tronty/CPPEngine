// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
// Created by S.Guillitte

#define PI 3.14159265358979

int windows = 3;//0=noise,1=abs(noise),2=fbm,3=fbmabs


float hash( in vec3 p ) 
{
    return fract(sin(p.x*15.32758341+p.y*39.786792357+p.z*59.4583127+7.5312) * 43758.236237153)-.5;
}

vec3 hash3( in vec3 p )
{
    return vec3(hash(p),hash(p+1.5),hash(p+2.5));
}



float voronoi(in vec3 p)
{    
    vec3 ip = floor(p);
    vec3 fp = fract(p);
	float rid = -1.;
    vec2 r = vec2(2.);
    for (int i=-1; i<=0; i++) 
	for (int j=-1; j<=0; j++) 
	for (int k=-1; k<=0; k++) 
	{
		vec3 g = vec3(i, j, k);
        //float h = hash(ip - g);
        vec3 pp = fp +g +hash3(ip - g)*.6;
        float d = dot(pp, pp);

        if (d < r.x)
		{
            r.y = r.x;
            r.x = d;
           // rid = h +.5;
		}
        else if(d < r.y)
		{
    		r.y = d;
		}
	}    
    return r.x;
}

float craters(vec3 p){
    float v = voronoi(p);
    return sin(sqrt(v)*PI*2.)*exp(-4.*v);
}

float stars(vec3 p){
    float v = voronoi(p);
    return pow(exp(.1-5.*v),18.);
}
  

float fbmabs( vec3 p ) {
	
	float f=1.2;
   
	float r = 0.0;	
    for(int i = 0;i<5;i++){	
		r += abs(craters( p*f ))/f;       
	    f *=2.7;
	}
	return r/2.;
}

float fbm( vec3 p ) {
	
	float f=1.;
   
	float r = 0.0;	
    for(int i = 0;i<3;i++){	
		r += stars( p*f )/f;       
	    f *=2.;
	}
	return r/4.;
}

float map(vec3 p){

    
    return 1.2*fbmabs(p);
}

vec3 calcNormal(in vec3 p)
{
	const vec2 e = vec2(0.00001, 0.0);
	return normalize(p*.00004+vec3(
		map(p + e.xyy) - map(p - e.xyy),
		map(p + e.yxy) - map(p - e.yxy),
		map(p + e.yyx) - map(p - e.yyx)));
}


mat2 rot(float a) {
	return mat2(cos(a),sin(a),-sin(a),cos(a));	
}

vec2 iSphere( in vec3 ro, in vec3 rd, in vec4 sph )//from iq
{
	vec3 oc = ro - sph.xyz;
	float b = dot( oc, rd );
	float c = dot( oc, oc ) - sph.w*sph.w;
	float h = b*b - c;
	if( h<0.0 ) return vec2(-1.0);
	h = sqrt(h);
	return vec2(-b-h, -b+h );
}

vec3 makeMercuryCraters(vec2 uv)
{
	float time = iTime;
    vec2 q = uv.xy / iResolution.xy;
    vec2 p = -1.0 + 2.0 * q;
    p.x *= iResolution.x/iResolution.y;
    vec2 m = vec2(0.);
	if( iMouse.z>0.0 )m = iMouse.xy/iResolution.xy*3.14;
    m-=.5;

    // camera

    vec3 ro = vec3(4.);
    ro.yz*=rot(m.y);
    ro.xz*=rot(m.x+ 0.1*time+1.);
    vec3 ta = vec3( 0.0 , 0.0, 0.0 );
    vec3 ww = normalize( ta - ro );
    vec3 uu = normalize( cross(ww,vec3(0.0,1.0,0.0) ) );
    vec3 vv = normalize( cross(uu,ww));
    vec3 rd = normalize( p.x*uu + p.y*vv + 4.0*ww );
	vec3 li = normalize(vec3(0.5, .8, 3.0));
    
    vec2 tmm = iSphere( ro, rd, vec4(0.,0.,0.,1.5) );

	float c;
    
   	if (tmm.x<0.)c =  fbm(30.*rd)+fbm(40.*rd)+.2*exp(-64.*(.9-dot(li,rd)));
    else c= map(ro+rd*tmm.x)/2.;
    vec3 col = vec3( c*c*1.3,c*c,c*.5);
    if (tmm.x>0.){
        
        float k = dot(ro+rd*tmm.x, li);
        col*=.4;
        if(k>0.){
            
    		vec3 nor = calcNormal(ro+rd*tmm.x);
            col += pow(max(dot(li, nor)*1.2, 0.05), .75)*k*vec3(.95,.55,.4);
        }
    }
	
	// shade
    
    //col =  1.5 *(log(1.+col));
    col = clamp(col,0.,1.);
    return col;
}

void main( ) 
{
	vec3 col=makeMercuryCraters(xlv_TEXCOORD0);
	gl_FragColor = vec4( col, 1.0 );
}

