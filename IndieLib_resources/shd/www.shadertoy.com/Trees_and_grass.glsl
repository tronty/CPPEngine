
float col=5.,col2=1e3,col3=1e3;


// Signed distance field function.
float f(vec3 p)
{
    float ter = fbm(p.xz+4.)*.1; // Ground
    col3=p.y-ter;
    p.y-=ter;
    float d=max(col3,p.y);
    
    
    if(length(p)<10.)p.xz = mod(p.xz+2.,5.)-3.;// Multiply trees
      
    float s=1.,s1=s,g=1.6,g1=1.7;
       
    mat2 m1 = r2D(2.2);
    mat2 m2 = r2D(1.05);
    mat2 m3 = r2D(-.1);

    int j=7;
    float h=3.;
    float scale=.6;
    p/=scale;
    
    //float a = length(p-vec3(0,h/2.,0))-h/2.;
    //if(a>2.)return min(d, a); 
    
    
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
      if(l<d)col=max(0.,l), d=l;      
      // return to trunc
      p.y-=1.5;
      p.xz *= -m1,
      s1*=g1;
      p*=g1;
      j -=1;}
    
    return d;
}



void main( )
{
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = xlv_TEXCOORD0/iResolution.xy -.5;
    uv.x*=iResolution.x/iResolution.y;


    float zoom=1.;
   
    // Set up camera and primary ray.
    vec3 ro=vec3(-2.5+cos(iTime/4.),1.5+ iMouse.y/iResolution.y,3.5);
    vec3 rd=normalize(vec3(uv.xy,zoom));
    vec3 target=vec3(0,1.3,0);

    vec3 w=normalize(target-ro);
    vec3 u=normalize(cross(w,vec3(0,1,0)));
    vec3 v=normalize(cross(w,-u));

    rd=mat3(u,v,w)*rd;
    
    // Sky
    float y = max(rd.y,0.);
    fragColor.rgb = mix(vec3(.1,.2,1.)*.7,vec3(1),fbm2(rd.xz/(.1+y)));
    fragColor.rgb = mix(fragColor.rgb,vec3(.5,.5,.9),pow(1.-y,10.));
    
    
    

    // Signed distance field raymarch.
    float t=0.,d=0.;
    for(int i=0;i<100;++i)
    {
        d=f(ro+rd*t);
        if(d<1e-3)break;
        t+=d;
        if(t>30.)return;
    }

    // Colourise ground, branch/trunk, or cherry blossom.
    {
        vec3 rp=ro+rd*t;
        fragColor.rgb=vec3(.75,.6,.4)/1.5;//wood
        //if(col<2e-3)fragColor.rgb=vec3(1.,.7,.8);//blossoms
        //if(col<2e-3)fragColor.rgb=vec3(1.,.5,.1);//red leaves
        if(col<2e-3)fragColor.rgb=vec3(.1,.5,.2);//green leaves
        if(col3<1e-1||t>10.)//grass
            fragColor.rgb=vec3(.6,1.,.6)/3.,
            fragColor.rg +=fbm1(rp.xz)/8.;
    }

    // Lighting.
    vec3 ld=normalize(vec3(1.,3.,1.));
    float e=1e-2;
    float d2=f(ro+rd*t+ld*e);
    float l=max(0.,(d2-d)/e);

    float d3=f(ro+rd*t+vec3(0,1,0)*e);
    float l2=max(0.,.5+.5*(d3-d)/e);

    vec3 rp=ro+rd*(t-1e-3);

    // Directional shadow.
    t=0.1;
    float sh=1.;
    for(int i=0;i<30;++i)
    {
        d=f(rp+ld*t)+.01;
        sh=min(sh,d*50.+0.3);
        if(d<1e-4)break;
        t+=d;
    }

    fragColor.rgb*=1.5*sh*(.2+.8*l)*vec3(1.,1.,.9)*.7+l2*vec3(.85,.85,1.)*.4;
    gl_FragColor.rgb=clamp(fragColor.rgb,0.,1.);
}

