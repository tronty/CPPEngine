// Planetary nebula by nimitz 2023 (twitter: @stormoid)
// https://www.shadertoy.com/view/fl2Bzd
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License
// Contact the author for other licensing options

/*
    The main trick for the nebula rendering is to compute SDF curvature to allow for rendering of "ambient occlusion"
    of the star light in the surrounding volume, which really helps the overall look.
    
    The star function maps to cube faces allowing for star patterns while only being a few layers of displaced dots
*/

mat3 rot_x(float a){float sa = sin(a); float ca = cos(a); return mat3(1.,.0,.0,    .0,ca,sa,   .0,-sa,ca);}
mat3 rot_y(float a){float sa = sin(a); float ca = cos(a); return mat3(ca,.0,sa,    .0,1.,.0,   -sa,.0,ca);}
const mat3 m3 = mat3(0.33338, 0.56034, -0.71817, -0.87887, 0.32651, -0.15323, 0.15162, 0.69596, 0.61339)*2.1;
mat2 mm2(in float a){float c = cos(a), s = sin(a);return mat2(c,s,-s,c);}

//#define HIGH_QUALITY

float map(vec3 p)
{
    p.yz *= mm2(-0.37);
    vec3 dz = vec3(p.z*.0,p.z*.0,p.z*.75);
    float cl = -abs(dot(p ,p-dz*1. )-10.0) + 2.0;
    float cl2 = -dot(p.xy,p.xy) + 7.0;
    cl = mix(cl,cl2,.35);
    
    float d = 0.;
    p *= 0.54;
    float z = 3.5;
    float trk = 1.;
    for(int i = 0; i < 5; i++)
    {
        p += sin(p.zxy*trk - 0.5)*0.25;
        d -= abs(dot(cos(p), sin(p.zxy + 0.6)) - .3)*z - 1.8;
        z *= 0.7;
        p = p*m3;
        trk *= 1.45;
    }

    return clamp(d*.9 + cl*.9,-20.,1000.);
}

float curv(in vec3 p, in float p0, in float w)
{
    vec2 e = vec2(-1., 1.)*w;   
    float t1 = map(p + e.yxx), t2 = map(p + e.xxy);
    float t3 = map(p + e.xyx), t4 = map(p + e.yyy);
    return .25/e.y*(t1 + t2 + t3 + t4 - 4.0*p0);
}

float linstep(in float mn, in float mx, in float x){return clamp((x - mn)/(mx - mn), 0., 1.);}

vec4 render( in vec3 ro, in vec3 rd )
{
	vec4 rez = vec4(0);
#ifdef HIGH_QUALITY
    float t = 8;
	for(int i=0; i<120; i++)
#else
    float t = 9.;
    for(int i=0; i<95; i++)
#endif
	{
		if(rez.a > 0.95 || t > 17.)break;

		vec3 pos = ro + t*rd;
        float dtp = dot(pos,pos);
        float dn = map(pos);
		float den = clamp(dn, 0.0, 1.3);

        float ext = linstep(60.,15.,dtp);
        float dt2 = linstep(0.,30.,dtp)+0.35;
        float crv = linstep(-3.5,-15., curv(pos*.75, dn, .65*dt2))*0.5+0.035;
        
        vec4 col = vec4((sin(vec3(3.7,1.7,.9) + dtp*0.03+4.9)*vec3(0.4,0.17,0.25)+vec3(0.2,0.01,0.1)) ,0.05)*den;
        col.xyzw *= vec4(0.25,0.37,.3,1.)*crv*ext;
        
        float dif =  clamp((map(pos * 1.09)- dn)*1.2, -.8, 6. );
        col.xyz *= vec3(1.) + vec3(.9,.8,.7)*dif;
        
        col.rgba += vec4(.75,.6,.5, 0.01)*clamp(0.07/dtp, 0., 5.)*(crv*0. + .2)*smoothstep(7.,-5.,dtp);
        rez = rez + col*(1. - rez.a);
        
        t += clamp(0.06 - den*.1, 0.065, 0.15);
        
	}
	return clamp(rez, 0.0, 1.0);
}

vec4 hash43x(vec3 p)
{
    uvec3 x = uvec3(ivec3(p));
    x = 1103515245U*((x.xyz >> 1U)^(x.yzx));
    uint h = 1103515245U*((x.x^x.z)^(x.y>>3U));
    uvec4 rz = uvec4(h, h*16807U, h*48271U, h*69621U); //see: http://random.mat.sbg.ac.at/results/karl/server/node4.html
    return vec4((rz >> 1) & uvec4(0x7fffffffU))/float(0x7fffffff);
}


/*  Very happy with this new star function, allows for constellations, low artifacts and "cheap" to evaluate
    Probably the nicest trick here is rendering on a "cube", and making sure the random displacement is bounded
    to the surface of the evaluated cube by selecting the correct displacement plane analytically
    Another great benefit is that the cube projection gives an non-uniform distribution for a single layer,
    by rotating the successive layers, the result ends up being more natural looking with visible large-scale structure.
*/
vec3 stars(vec3 p)
{
    vec3 col = vec3(0);
    float rad = .087*iResolution.y;
    float dens = 0.15;
    float id = 0.;
    float rz = 0.;
    float z = 1.;
    
    for (float i = 0.; i < 5.; i++)
    {
        p *= mat3(0.86564, -0.28535, 0.41140, 0.50033, 0.46255, -0.73193, 0.01856, 0.83942, 0.54317);
        vec3 q = abs(p);
        vec3 p2 = p/max(q.x, max(q.y,q.z));
        p2 *= rad;
        vec3 ip = floor(p2 + 1e-5);
        vec3 fp = fract(p2 + 1e-5);
        vec4 rand = hash43x(ip*283.1);
        vec3 q2 = abs(p2);
        vec3 pl = 1.0- step(max(q2.x, max(q2.y, q2.z)), q2);
        vec3 pp = fp - ((rand.xyz-0.5)*.6 + 0.5)*pl; //don't displace points away from the cube faces
        float pr = length(ip) - rad;   
        if (rand.w > (dens - dens*pr*0.035)) pp += 1e6;

        float d = dot(pp, pp);
        d /= pow(fract(rand.w*172.1), 32.) + .25;
        float bri = dot(rand.xyz*(1.-pl),vec3(1)); //since one random value is unused to displace, we can reuse
        id = fract(rand.w*101.);
        col += bri*z*.00009/pow(d + 0.025, 3.0)*(mix(vec3(1.0,0.45,0.1),vec3(0.75,0.85,1.), id)*0.6+0.4);
        
        rad = floor(rad*1.08);
        dens *= 1.45;
        z *= 0.6;
        p = p.yxz;
    }
    
    return col;
}

void main()
{
	vec2 fragCoord=xlv_TEXCOORD0.xy;

    vec2 q = fragCoord.xy / iResolution.xy;
    vec2 p = q - 0.5;
	p.x*=iResolution.x/iResolution.y;
	vec2 mo = iMouse.xy / iResolution.xy-.5;
    mo = (mo==vec2(-.5))?mo=vec2(-0.25,0.15):mo;
	mo.x *= iResolution.x/iResolution.y;
    mo*=3.14;
	mo.y = clamp(mo.y*0.6-.5,-4. ,.15 );
	
    vec3 ro = vec3(0.,-0.0,12.);
    vec3 rd = normalize(vec3(p,-1.4));
    
    mat3 cam = rot_x(-mo.y)*rot_y(-mo.x + sin(iTime*0.2)*0.15);
	rd *= cam;
    ro *= cam;
    
    vec4 scn = render(ro, rd);
    vec3 col = vec3(0.1, 0.1, 0.11)*smoothstep(-1.,1.,rd.y)*0.;
    col = stars(rd);
    
    col = col*(1.0-scn.w) + scn.xyz;

    col = pow(col, vec3(.5));
    col *= pow( 16.0*q.x*q.y*(1.0-q.x)*(1.0-q.y), 0.12); //Vign
	gl_FragColor = vec4( col, 1.0 );
}

