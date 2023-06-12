uniform sampler3D iChannel1;
#define texture(a1,a2) texture2D(a1,a2)

#define PI 3.1415926
#define TAU (2.0 * PI)

#define SPLIT_THICK_LINES 1

// Egg 2d sdf
float sdEgg2( in vec2 p)
{
    p.x = abs(p.x);
    return   ((p.y <= 0.)           ? length(p)   - 1.0 :
              ((p.y-1.0) > p.x)     ? length(p-vec2( 0.0,1.0)) - (2.-sqrt(2.)) : 
                                        length(p-vec2(-1.0,0.0)) - 2.);  
}

// Egg 2d param [-1,1]
float egg_u(in vec2 p )
{
    float s0 = (p.y >0.0 && p.x >0.0 && ((p.y-1.0) <= p.x)) ? (atan(p.y, p.x+1.0)*2.0):0.0;
    const float s0t = PI/2.0;
    //
    float s1t = sqrt(2.0)*PI/8.0; 
    float s1 = (p.y >0.0 && ((p.y-1.0) > p.x) && ((p.y-1.0) >-p.x)) ?s0t+ atan(p.y-1.0, p.x)*sqrt(2.0)/2.0 - s1t
                                                                    :0.0;
    //                                                               
    s1t*=2.0;
    float s2 = (p.y >0.0 && p.x <0.0 && (p.y-1.0) <-p.x) ?s1t+s0t+ atan(p.y, p.x-1.0)*2.0 - 3.0*PI/2.0:0.0;
    float s2t = s0t*2.0 +s1t; 
    //
    return  p.y <=0.0 ? atan(p.y, p.x)/PI: (s0+ s1 +s2)/s2t;
}

// Circle 2d param [-1,1]
float circ_v(in vec2 p)
{
    float vp = atan( p.y,p.x);
   
    return vp /(PI);
}

// Egg 3d sdf
float sdEgg3(vec3 pos)
{
    vec2 pxy= vec2(length(pos.xz), pos.y);
    return sdEgg2(pxy);
}

// Egg normal
vec3 Negg3( in vec3 pos )
{
    const float ep = 0.01;
    vec2 e = vec2(1.0,-1.0);
    return normalize( e.xyy*sdEgg3( pos + e.xyy*ep ) + 
					  e.yyx*sdEgg3( pos + e.yyx*ep ) + 
					  e.yxy*sdEgg3( pos + e.yxy*ep ) + 
					  e.xxx*sdEgg3( pos + e.xxx*ep ) );
}


float sdThickCircle(in vec2 p, float r , float r_in)
{
   
     float d0 =length(p)-r;
     float d1 = length(p)-r_in;
     return max(d0,-d1);
}



float sdThickCircleHalf(in vec2 p, float r,float r_in)
{
    float d0 =p.y>0.0 ?length(p)-r : length(abs(p)-vec2(r,0.0));
    float d1 =p.y>0.0 ?length(p)-r_in : length(abs(p)-vec2(r,0.0));
    return max(d0,-d1);
}


#define R .2
#define R38 (R/38.0)
#define R76 (R38/2.0)
#define M (2.0*R/18.0)
#define C_OFF R/2.0

float traceArc(vec2 uv, int o, int n)
{
    float rLoc = R-R/12.0;
    float mLoc = 2.0*rLoc/18.0;
    float r = mLoc*float(n)/2.0;
    vec2 c = vec2(-rLoc+mLoc*float(o)+r,0.0);
    return sdThickCircleHalf(vec2(uv.x,uv.y)+c, r+R76, r-R76);
}


float c_dir(in vec2 p, float r, float r_in,vec2 dir)
{
    float d0 = (p.x*dir.y) - (p.y*dir.x) >0.0 ? min(length(p-dir*r), length(p+dir*r)) :length(p)-r;
    float d1 = (p.x*dir.y) - (p.y*dir.x) >0.0 ? min(length(p-dir*r_in), length(p+dir*r_in)) :length(p)-r_in;
    return max(d0,-d1);
}

float c_dir1(in vec2 p, float r, float r_in,vec2 dir, float off)
{
    
    float d0 = p.y<-off &&(p.x*dir.y) - (p.y*dir.x) >0.0 ? min(length(p-dir*r), length(p+dir*r)) :length(p)-r;
    float d1 = p.y<-off &&(p.x*dir.y) - (p.y*dir.x) >0.0 ? min(length(p-dir*r_in), length(p+dir*r_in)) :length(p)-r_in;
    return max(d0,-d1);
}

float sd_l(vec2 p, vec2 d, float l, float w)
{
    float h= clamp(dot(p,d),0.0,l);
    return length(d*h-p)-w  ; 
}

float leaf(in vec2 p)
{
    float _r=R-R/4.0;
    p.x = abs(p.x);
    
    float sq32 = sqrt(3.0)/2.0;
    
    // vertical line 
    float dvert = sd_l( p+vec2(0.0,_r), vec2(0.0,1.0),_r*3.0/2.0,R76);
    
    // main dir 
    vec2 dr_dir_n = vec2(.5,-sq32);
    vec2 dr_dir =  _r*sq32*dr_dir_n;
    // main dir ortho
    vec2 dr_ortho_dir_n = vec2(sq32,.5);
    vec2 dr_ortho_dir = _r* dr_ortho_dir_n;
    
    //
    float dr0 =  sd_l( p+vec2(0.0,-_r/2.0), dr_ortho_dir_n,_r*.5,R76);
    float dr = sd_l( p+vec2(0.0,_r), dr_ortho_dir_n,_r,R76);
    float dr1 = sd_l( p+vec2(0.0,0.0), dr_ortho_dir_n,_r-_r/4.0,R76);
    float dr2 = sd_l( p+vec2(0.0,_r/2.0), dr_ortho_dir_n,_r,R76);
   
    //
    vec2 p0= (dr_ortho_dir*3.0/4.0+dr_dir*1.0/4.0);
    vec2 p1= (dr_ortho_dir*3.0/4.0-dr_dir*1.0/4.0);
    vec2 p2= (dr_ortho_dir*3.0/4.0+dr_dir*1.0/4.0) - vec2(0.0,_r/2.0);
    vec2 p3= vec2(0.0,_r*3.0/4.0);
    
    float rc = _r*sq32* .25;
    float rc1 = _r*.25;
    float p0_d = c_dir(p-p0,rc+R76, rc-R76,dr_dir_n);
    float p1_d = c_dir(p-p1, rc+R76, rc-R76,dr_dir_n);
    float p2_d = c_dir(p-p2, rc+R76, rc-R76,dr_dir_n);
    float p3_d = c_dir1(p-p3,  rc1+R76, rc1-R76,vec2(1.0,0.0),_r*.125);
    
    float dist = min(dvert,min(dr0,min(dr2,min(dr1,min(p3_d,min(p2_d,(min(p1_d, min(p0_d,dr)))))))));
    return dist;
}

float trefoil(in vec2 p)
{
    float r_ = R/2.0 - R/10.0;
    p.y*=2.0;
    p.x*=.7;
    p.y+= r_-r_/8.0;
    
    float sq32 = sqrt(3.0)/2.0;
    // main dir 
    vec2 ds_n = vec2(sq32,-.5);
   
    // folds
    // y dir
    //p.y *= smoothstep(0.0,r_,p.y);
    p.x = abs(p.x);
    // ds_n dir 
    p= (p.x*ds_n.y)-(p.y*ds_n.x) >0.0 ? p+ 2.0*(dot(ds_n,p)*ds_n-p): p;
   
    float border = .5*r_*(sq32-1.0);
    float c0= length(p)-r_;
    float c1 = length(p)-(r_+border);
    float c = max(c0,-c1);
    
    float tr0= (length(p-r_*vec2(sqrt(3.0)/4.0,1.0/4.0))-.5*r_);
    float tr1 = (length(p-r_*vec2(sqrt(3.0)/4.0,1.0/4.0))-(.5*r_+border));
    
    
    
    float tr = min(c, max(tr0,-tr1));
   
    
   return tr;
}

float waves(in vec2 uv)
{
    vec2 uv_f = uv*vec2(1.0,-1.0);
    float sp0 =1.0000;

    sp0 =min(sp0, traceArc(uv,3, 1));
    sp0 = min(sp0, traceArc(uv_f,3, 1));

    sp0 = min(sp0, traceArc(uv,0, 4)); 
    sp0 = min(sp0, traceArc(uv,2, 2)); 
    sp0 = min(sp0, traceArc(uv_f,2, 4)); 
    sp0 = min(sp0, traceArc(uv_f,0, 12)); 
    sp0 = min(sp0, traceArc(uv_f,2, 10)); 

    sp0 = min(sp0, traceArc(uv,6, 12)); 
    sp0 = min(sp0, traceArc(uv,6, 10)); 
    sp0 = min(sp0, traceArc(uv,6, 4)); 

    sp0 = min(sp0, traceArc(uv,8, 2)); 
    sp0 = min(sp0, traceArc(uv_f,8, 2)); 
    sp0 = min(sp0, traceArc(uv_f,8, 4)); 

    sp0 = min(sp0, traceArc(uv,12, 4)); 
    sp0 = min(sp0, traceArc(uv_f,14, 2)); 
    sp0 = min(sp0, traceArc(uv_f,14, 1)); 
    sp0 = min(sp0, traceArc(uv,14, 1));
    sp0 = min(sp0, traceArc(uv_f,14, 4));
    
    return sp0;
}

float mapParam(in vec3 pos)
{
    pos.y+=.3;
    // 
    float v = egg_u(vec2(length(pos.xz),pos.y));
    float u = circ_v(pos.xz);



    //float vi = floor(v*4.0);
    float r =.2;
    float r_in = r-r/12.0;
    float r2 = r*2.0;
    //
    float u4 = fract((u)*3.0);
    float u4_90 = fract((u+1.0/6.0)*3.0);

    float u_= u4*r2-r;
    float u_90= u4_90*r2-r;
    //
    float cCenter = sdThickCircle(vec2(u_,v-.1), r, r_in) ;
    float cTop = sdThickCircle(vec2(.5*(u_90), (v-.4)), r/2.0, r/2.0-r/12.0) ;
    float cBottom = sdThickCircle(vec2(u_90, (v+.3)), r, r-r/6.0);


    float lb = abs(v+.1)-r/24.0;
    float lt = abs(v-.3)-r/36.0;

    //leaf
    vec2 uv = vec2(u_90, (v+.3));
    float leaf = leaf(uv);

    //trefoil
    uv = vec2(u_90, (v-.4));
    float trefoil = trefoil(uv);

    //waves 
    uv = vec2(u_,v-C_OFF);
    float waves = waves(uv);

    //
    return min(trefoil,min(leaf,min(waves,min(cCenter,min(cTop,min(cBottom, min(lb, lt)))))));

}

// from Shane https://www.shadertoy.com/view/tdtyDs
//
// Tri-Planar blending function: Based on an old Nvidia writeup:
// GPU Gems 3 - Ryan Geiss: http://http.developer.nvidia.com/GPUGems3/gpugems3_ch01.html
vec3 tex3D(sampler2D tex, in vec3 p, in vec3 n){    
    
    // Ryan Geiss effectively multiplies the first line by 7. It took me a while to realize that 
    // it's largely redundant, due to the division process that follows. I'd never noticed on 
    // account of the fact that I'm not in the habit of questioning stuff written by Ryan Geiss. :)
    n = max(n*n - .2, .001); // max(abs(n), 0.001), etc.
    n /= dot(n, vec3(1)); 
    //n /= length(n); 
    
    // Texure samples. One for each plane.
    vec3 tx = texture(tex, p.yz).xyz;
    vec3 ty = texture(tex, p.zx).xyz;
    vec3 tz = texture(tex, p.xy).xyz;
    
    // Multiply each texture plane by its normal dominance factor.... or however you wish
    // to describe it. For instance, if the normal faces up or down, the "ty" texture sample,
    // represnting the XZ plane, will be used, which makes sense.
    
    // Textures are stored in sRGB (I think), so you have to convert them to linear space 
    // (squaring is a rough approximation) prior to working with them... or something like that. :)
    // Once the final color value is gamma corrected, you should see correct looking colors.
    return mat3(tx*tx, ty*ty, tz*tz)*n; // Equivalent to: tx*tx*n.x + ty*ty*n.y + tz*tz*n.z;

}


// https://iquilezles.org/articles/normalsSDF
vec3 calcNormal( in vec3 pos )
{
    const float ep = 0.001;
    vec2 e = vec2(1.0,-1.0);
    return normalize( e.xyy*sdEgg3( pos + e.xyy*ep )+ 
					  e.yyx*sdEgg3( pos + e.yyx*ep )+ 
					  e.yxy*sdEgg3( pos + e.yxy*ep )+ 
					  e.xxx*sdEgg3( pos + e.xxx*ep ));
}

#define AA 2

void main( )
{
    vec2 m =  (2.0*iMouse.xy-iResolution.xy)/iResolution.y;
    //
    vec3 tot = vec3(0.0);
    #if AA>1
    for( int mm=0; mm<AA; mm++ )
    for( int nn=0; nn<AA; nn++ )
    {
        // pixel coordinates
        vec2 o = vec2(float(mm),float(nn)) / float(AA) - 0.5;
        vec2 p2 = (2.0 * (xlv_TEXCOORD0.xy+o)/iResolution.xy -1.0 ); 
        #else    
        vec2 p2 = (2.0 * xlv_TEXCOORD0.xy/iResolution.xy -1.0 );
        #endif
         
        p2.y+=.25;
        p2.x*=(iResolution.x/iResolution.y);
        // camera
        vec3 up = vec3(0.0,1.0,0.0);
        vec3 ro = vec3(3.0*sin(7.0*(m.x+(iTime/10.0))), 3.0*cos(4.0*(m.x+(iTime/10.0))),3.0*cos(7.0*(m.x +(iTime/10.0))));
        vec3 ta = vec3(0.0,0.0,0.0);
        vec3 ro_ta = normalize(ta -ro); 
        vec3 ri = normalize(cross(ro_ta,up));
        vec3 rj = normalize(cross(ri,ro_ta));
        vec3 rd = normalize(p2.x*ri +p2.y*rj+ ro_ta*2.0);   

        float t = 0.0;
        for( int i=0; i<256;++i)
        {
            vec3 p = ro + t*rd;
            float h = sdEgg3(p);

            if( abs(h)<0.001 || t>10.0 ) break;
            t += h;
        }

        vec3 col = vec3(0.0);
        if( t<10.0  )
        {
            //
            vec3 pos = ro + t*rd;
            vec3 nor = calcNormal(pos);
            vec3 ref = reflect(rd, nor);
            vec3 specColor = vec3(.7,.8,1.0);
           
            vec4 tx_ = texture3D(iChannel1, pos, 0.0);// ???, nor);
		vec3 tx=tx_.xyz;
            tx = smoothstep(.0, .5, tx);
            
            vec3  lig = normalize(vec3(1.0,2.0,-2.0));
            vec3  lig1 = normalize(vec3(1.0,2.0,2.0));
            float dif = clamp(dot(nor,lig),0.0,1.0);
            float dif1 = clamp(dot(nor,lig1),0.0,1.0);
            
            
            float dp =mapParam(pos);
            float sf = 0.001;
            float dps = smoothstep(0.0,sf,dp);
            float dps1 = smoothstep( -sf*1.1,-sf,dp);
            col = mix(vec3(1.0,.7,.3),vec3(1.0,.96,.85),dps1);
            #if defined(SPLIT_THICK_LINES)
            col = mix(col,vec3(.27,0.05,0.047),abs(pow(.5*(sin(dp*1000.0)+1.0),6.0)));
            #endif
            col = mix(col, vec3(.27,0.05,0.047),dps);
            // fresnel
            float exponent = 8.; 
            float fresnel = .5*pow(clamp(1. - dot(nor, -rd), 0., 1.), exponent);
            col += fresnel *vec3(1, 1, 1) ;
           
            // spec term 
            float pp = clamp(dot(ref, lig), 0.0, 1.0);
            float spec = .5* pow(pp, 128.0*.3 );
            float pp1 = clamp(dot(ref, lig1), 0.0, 1.0);
            float spec1 = .3* pow(pp1, 128.0*.8 );
            col = col + dif*1.7*vec3(.27,0.05,0.047) +   specColor* spec;
            col +=  dif1*1.7*vec3(.27,0.05,0.047) +  specColor * spec1;
            
            // small amount of texture.
            float av = length(tx);
            col *= tx*.3 + .7;
            
            tot += col;
        }
        else 
            tot+=col;

         #if AA>1
        }
    tot /= float(AA*AA);
    #endif
    
	gl_FragColor = vec4(tot, 1.0);
}
