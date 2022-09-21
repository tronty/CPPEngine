// https://www.shadertoy.com/view/Nl3BDj truchet labyrinth 3d by jt
// Extruded https://www.shadertoy.com/view/stdfDB truchet labyrinth sdf


#define EPSILON 0.001
#define DIST_MAX 50.0

// https://www.shadertoy.com/view/WttXWX "Best" Integer Hash by FabriceNeyret2
// implementing Chris Wellons https://nullprogram.com/blog/2018/07/31/
uint triple32(uint x)
{
    x ^= x >> 17;
    x *= 0xed5ad4bbU;
    x ^= x >> 11;
    x *= 0xac4c1b51U;
    x ^= x >> 15;
    x *= 0x31848babU;
    x ^= x >> 14;
    return x;
}

float hash(uint x)
{
    return float(triple32(x)) / float(0xffffffffU);
}

uint hashi(uvec2 v)
{
    return triple32(v.x + triple32(v.y));
}
#if 0
float sdBox( vec3 p, vec3 b )
{
    vec3 d = abs(p) - b;
    return min(max(d.x,max(d.y,d.z)),0.0) + length(max(d,0.0));
}
#else
// https://iquilezles.org/articles/distfunctions
float sdBox( in vec2 p, in vec2 b )
{
    vec2 d = abs(p)-b;
    return length(max(d,0.0)) + min(max(d.x,d.y),0.0);
    //return min(max(d.x,d.y),0.0) + length(max(d,0.0));
}
#endif
// https://iquilezles.org/articles/distfunctions
float sdOrientedBox( in vec2 p, in vec2 a, in vec2 b, float th )
{
    float l = length(b-a);
    vec2  d = (b-a)/l;
    vec2  q = (p-(a+b)*0.5);
          q = mat2(d.x,-d.y,d.y,d.x)*q;
          q = abs(q)-vec2(l,th)*0.5;
    return length(max(q,0.0)) + min(max(q.x,q.y),0.0);    
}

// Create infinite copies of an object -  https://iquilezles.org/articles/distfunctions
vec2 opRep(in vec2 p)
{
    return p - 2.0*round(p/2.0);
    //return fract(p) - 1.0;
}

ivec2 opRepIdx(in vec2 p)
{
    return ivec2(floor((p + 1.0) / 2.0));
}

uint opRepIdxRnd(in vec2 p)
{
    return hashi(uvec2(opRepIdx(p)));
    //return hashi(triple32(uint(iFrame/100)) + uvec2(opRepIdx(p)));
}

bool opRepIdxRndBool(in vec2 p)
{
    return (opRepIdxRnd(p) & 1u) != 0u;
}

float labyrinth(in vec2 p)
{
    vec2 r = opRep(p); // coordinates inside tile
    //float d = 1.0 / 0.0;
    float d = DIST_MAX; // maximal distance

    float th = 0.1; // wall thickness
    float l = 1.0 + th / 4.0; // wall length
    mat2 m = mat2(vec2(1,1),vec2(-1,1)); // 45 degree rotation
    for(int v = -1; v <= +1; v++)
    {
        for(int u = -1; u <= +1; u++)
        {
            vec2 o = 2.0 * vec2(u, v); // offset
            if(opRepIdxRndBool(p - o))
                //d = min(d, sdOrientedBox(r + o, vec2(-l,+l), vec2(+l,-l), th));
                d = min(d, sdBox((r + o)*m, vec2(th/sqrt(2.0),2.0+th/sqrt(2.0)))/sqrt(2.0));
            else
                //d = min(d, sdOrientedBox(r + o, vec2(-l,-l), vec2(+l,+l), th));
                d = min(d, sdBox(m*(r + o), vec2(th/sqrt(2.0),2.0+th/sqrt(2.0)))/sqrt(2.0));
                
        }
    }

    return d;
}

float extrude_labyrinth(vec3 p)
{
    float t = 0.01;
    float dp = p.z; // distance to plane
    float df = labyrinth(p.xy); // distance to 2d SDF
    return max(df,dp) - t; // 3dify
}

float halfspace(vec3 p, float d)
{
    return p.z - d;
}

float map(vec3 p)
{
    float d = mix(0.01, 0.1, 0.5 + 0.5 * cos(iTime));
    return
        min
        (
            extrude_labyrinth(p),
            halfspace(p, -1.2)
        );
}

// https://iquilezles.org/articles/normalsSDF tetrahedron normals
vec3 normal(vec3 p)
{
    const float h = EPSILON;
    const vec2 k = vec2(1,-1);
    return
        normalize
        (
            k.xyy * map(p + k.xyy*h)
            +
            k.yyx * map(p + k.yyx*h)
            +
            k.yxy * map(p + k.yxy*h)
            +
            k.xxx * map(p + k.xxx*h)
        );
}

float trace(vec3 ro, vec3 rd)
{
    for(float t = 0.0; t < DIST_MAX;)
    {
        float h = map(ro + rd * t);
        if(h < EPSILON)
            return t;
        t += h;
    }
    return DIST_MAX;
}

// https://iquilezles.org/articles/rmshadows
float shadow( in vec3 ro, in vec3 rd, float mint, float maxt )
{
    for( float t=mint; t<maxt; )
    {
        float h = map(ro + rd*t);
        if( h<EPSILON )
            return 0.0;
        t += h;
    }
    return 1.0;
}

// https://www.shadertoy.com/view/Xds3zN raymarching primitives
float calcAO( in vec3 pos, in vec3 nor )
{
    float occ = 0.0;
    float sca = 1.0;
    for( int i=0; i<5; i++ )
    {
        float h = 0.01 + 0.12*float(i)/4.0;
        float d = map( pos + h*nor );
        occ += (h-d)*sca;
        sca *= 0.95;
        if( occ>0.35 ) break;
    }
    return clamp( 1.0 - 3.0*occ, 0.0, 1.0 ) ;
}

#define pi 3.1415926

void main( )
{
    vec2 uv = xlv_TEXCOORD0.xy / iResolution.xy;
    vec2 ndc = 2.0 * uv - 1.0;
    ndc.x *= float(iResolution.x) / float(iResolution.y);
    float mx = 2.0 * pi * float(iMouse.x) / float(iResolution.x);
    float my = pi / 2.0 + pi / 2.0 * float(iMouse.y) / float(iResolution.y);
    mx = (iMouse.x != 0.0) ? mx : 2.0 * pi * fract(iTime * 0.1);
    my = (iMouse.y != 0.0) ? my : pi * 2.5 / 4.0;;

    mat2 R = mat2(vec2(cos(mx), sin(mx)), vec2(-sin(mx), cos(mx)));
    vec3 ro = vec3(0.0, 0.0, -5.0 );//vec3(0.0, -10.0 * my, 0.0);
    //mat2 S = mat2(vec2(0.0, 1.0), vec2(-1.0, 0.0));
    mat2 S = mat2(vec2(cos(my), sin(my)), vec2(-sin(my), cos(my)));
    ro.yz=S*ro.yz;
    ro.xy = R * ro.xy;

    vec3 rd = normalize(vec3(0.5 * ndc.xy, 1.0)); // NOTE: omitting normalization results in clipped edges artifact
    rd.yz=S*rd.yz;
    rd.xy = R * rd.xy;

    float dist = trace(ro, rd);
    vec3 dst = ro + rd * dist;
    vec3 n = normal(dst);

    vec3 lightdir = normalize(vec3(1.0, 1.0, 1.0));
    vec3 ambient = vec3(0.4);
    float brightness = max(dot(lightdir, n), 0.0);
    brightness *= shadow(ro+rd*dist,lightdir, 0.01, DIST_MAX); // XXX artifacts on cylinder XXX
    vec3 color = vec3(1.0);
    color *= (n * 0.5 + 0.5);
    color = (ambient * calcAO(dst, n) + brightness) * color;

    vec4 fragColor = mix(vec4(color, 1.0), vec4(0.0), step(DIST_MAX, dist));
    gl_FragColor = sqrt(fragColor); // approximate gamma
}
