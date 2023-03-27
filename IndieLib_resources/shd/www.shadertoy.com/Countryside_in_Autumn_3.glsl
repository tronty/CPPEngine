
float ma1=5.,ma2=1e3;

mat3 m3 = mat3( 0.00,  0.80,  0.60,
              -0.80,  0.36, -0.48,
              -0.60, -0.48,  0.64 ); 

mat2 r2D(float a)
{
	float si = sin(a);
	float co = cos(a);
	return mat2(si, co, -co, si);
}

float fbm(vec2 p)//ground
{
    float s=.3;
    mat2 m = r2D(1.);
    vec2 r= vec2(0.5);
    for(int i=0;i<7;i++)r+=sin(p), p=m*(p*s+.2*r+.1),s*=1.6;
    return (r.x+r.y)*.8 - 2.;
}

float fbm1(vec2 p, float t)//grass
{
    float s=2.;
    mat2 m = r2D(1.);
    vec2 r= vec2(0.);
    for(int i=0;i<8;i++)r+=(sin(p.yx+.5+.1*t))/s, p=m*p*s+cos(r.yx*s), s*=1.02;
    return (r.x+r.y)/s;
}


float fbm3(vec3 p, float t)//clouds
{
    float s=1., r= -4.;
    p += 4.*cos(.07*p)+5.*sin(.05*p.zyx+1.);
    p.xz *=.7;
    vec3 n = vec3(1);
    for(int i=0;i<8;i++)
        p = m3*p.zxy,      
        n+=.8*cos(p*s+.02*t), 
        r+=abs(dot(sin(p*s+n.zxy-1.)/s,vec3(1.))),
        s*=-1.9;       
    return -r;
}


// Signed distance field function.
float field(vec3 p)
{
    float ter = -fbm(.2*p.xz+2.)*.3; // Ground
    ma2=p.y-ter;
    p.y-=ter;
    float d=max(ma2,p.y);
    
    if(length(p)<10.)p.xz = mod(p.xz+3.,5.)-2.;// Multiply trees
      
    float s=1.,s1=s,g=1.4,g1=1.4;
       
    mat2 m1 = r2D(2.1);
    mat2 m2 = r2D(1.15);
    mat2 m3 = r2D(-.1);

    int j=7;
    float h=4.;
    float scale=.7;
    p/=scale;
    
    // Evaluate the tree 
    d=min(d,scale*max(p.y-h,max(-p.y,length(p.xz)-.1/(p.y+.7)))/s);// trunc 
    for(int k=0;k<int(h);k++){
      vec3 q=p;
      s=s1;
      // Evaluate the tree branches
      for(int i=0;i<j;
        q.x=abs(q.x),
        q.z=(i==0)?q.z:abs(q.z), 
        q.y-=1.,
        q.xz *= m2,
        q.xy *= m1,
        q.xz *= m3,
        q*=g,
        s*=g,
        d=min(d,scale*max(q.y-1.,max(-q.y,length(q.xz)-.1/(q.y+.7)))/s),
        ++i);
      // Leaves
      q.y-=1.;      
      float l = (length(q*vec3(.6,.3,1.))-.3)/s;
      if(l<d)ma1=max(0.,l), d=l;      
      // return to trunc
      p.y-=1.6;
      p.xz *= -m1,
      s1*=g1;
      p*=g1;
      j -=1;
      }
    return d;
}

const vec3 sundir = normalize( vec3(-.3,.5,-1.) );
const vec3 suncol = vec3(1.,.8,.5);
const float low = 5.;
const float high = 8.;

float cloud(in vec3 p) {
	float e=max(low-p.y,0.)+max(p.y-high,0.)-.3;
    p.z += .2*iTime;
    e += fbm3(p,iTime);
    return .5*e;
}

vec3 sky( in vec3 ro, in vec3 rd ,in vec2 res)
{ 
    // background sky : modified from IQ clouds        
    float sun = max( dot(sundir,rd), 0.0);    
    vec3 col = vec3(0.6,0.6,0.78) - abs(rd.y)*0.5*vec3(1.0,0.4,.05); 
    float k = res.x, c = res.y;
    col += suncol*pow( sun, 500.0 )*(1.-10.*c); 
    // clouds           
    if(c>.0)
       col *= 1.-.5*c,
       col += 3.*(.5+k)*c*suncol,
       col += 2.*vec3(0.2,0.08,0.04)*pow( sun, 3.0 )*k;    
    // godrays
    float sh = 1., t, d;    
    vec3 q = ro+20.*rd;
    for(int i=0; i<20; ++i)    
        t = max((1.+low-q.y)/(.5+rd.y), 0.),
        d = cloud(q+sundir*t),
        sh = min(sh,1./(1.+exp(-d))),        
        q += rd*.6;    
    col += .7*pow((sh-1.)*sun,2.)*suncol;    
    return col;
}


vec3 fields( in vec3 ro, in vec3 rd ,in float t)
{
     
     vec3 col;
    // Colourise ground, branch/trunk, or cherry blossom.
    
    vec3 p=ro+rd*t;
    col =vec3(.75,.6,.4)/1.5;//wood
    col.rgb +=fbm1(p.zy*.9,0.)/6.;
    if(ma1<2e-3)col=vec3(1.,.5,.1);//red leaves
    if(ma2<1e-2||t>12.)//grass
        col =vec3(.6,1.,.6)/3.,
        col.rg +=fbm1(p.xz,iTime)/8.;             
    
    // Lighting.
    float d =field(p);
    float e=1e-2;
    float d2=field(ro+rd*t+sundir*e);
    float l=max(0.,(d2-d)/e);

    float d3=field(ro+rd*t+vec3(0,1,0)*e);
    float l2=max(0.,.5+.5*(d3-d)/e);

    p=ro+rd*(t-1e-3);

    // Directional shadow.
    t=.1;
    float sh=1.;
    for(int i=0;i<30;++i)
    {
        d=field(p+sundir*t);
        sh=min(sh,50.*d+0.3);
        if(d<1e-4)break;
        t+=d;
    }

    col *=1.5*sh*(.2+.8*l)*vec3(1.,1.,.9)*.7+l2*vec3(.85,.85,1.)*.4;
    return col;
    
}

vec3 raycast( in vec3 ro, vec3 rd )
{
    float t = 4.,e,e1,f,de=0.,df,d,c=1.,dt=.2,r=1.;
    vec3 col= vec3(0.),p,skycol;
    for( int i=0; i<64; i++ )
	{                
        p = ro+t*rd;
        e = cloud(p), f=field(p),             
        d = min(f,max(e,.05));
        t += .9*d;
        if(e<.001)
            e1 = cloud(p + dt*sundir),
            de += max(e1-e,-.0)/dt/(1.+ exp(-32.*e1));
        c *= 1./(1.+ exp(-32.*e));        	
    }
    skycol = sky(ro,rd,vec2(.1*de,(1.-c)*.1));        
    if(f>.5)return skycol;
    col = fields( ro, rd, t);
    float maxd = 30.;    
    col = mix(col, skycol, smoothstep(.4, .99, min(t, maxd)/maxd));
    return col;
}


void main()
{
	vec4 fragColor;
	vec2 fragCoord=xlv_TEXCOORD0.xy;

    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = fragCoord.xy/iResolution.xy - .5;
    uv.x*=iResolution.x/iResolution.y;
    vec2 m = iMouse.xy/iResolution.xy;

    float zoom=.8;
   
    // Set up camera and primary ray.
    vec3 ro=1.*vec3(sin(3.0*m.x),0.*m.y+2.,cos(3.0*m.x));
    vec3 rd=normalize(vec3(uv.xy,zoom));
    vec3 target=vec3(0,2.,0);

    vec3 w=normalize(target-ro);
    vec3 u=normalize(cross(w,vec3(0,1,0)));
    vec3 v=normalize(cross(w,-u));

    rd=mat3(u,v,w)*rd;       
    
    fragColor.rgb=clamp(raycast(ro,rd),0.,1.);
	gl_FragColor=fragColor;
}

