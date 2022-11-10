const float EARTH_RADIUS = 1.0;
const float MAX = 10000.0;
#define PI 3.14159265358979323844
float iteration = 10.;
float timeScale = 3.;
vec2 zoom = vec2(25.,5.5);
vec2 offset = vec2(0,2.);

#if 0
struct PSIn
{
    float4 position;
    float2 texCoord;
    float3 viewVectorTangent;
    float3 lightVectorTangent;
    float3 tangent;
    float3 binormal;
    float3 viewVector;    
    float3 lightVector;
};

struct PSInQuad
{
    float4 position;
    float3 texCoord;
};

void CreateIndexBuffer()
{
    int IndexNumber = SPHERE_MERIDIAN_SLICES_NUM * SPHERE_PARALLEL_SLICES_NUM * 6;
    unsigned *pIndices = new unsigned[IndexNumber];
    unsigned indexCount = 0;
    for( int i = 0; i < SPHERE_PARALLEL_SLICES_NUM; i++ )
    {
        for( int j = 0; j < SPHERE_MERIDIAN_SLICES_NUM; j++ )
        {
            pIndices[indexCount] = i * (SPHERE_MERIDIAN_SLICES_NUM + 1) + j;
            indexCount++;
            pIndices[indexCount] = (i + 1) * (SPHERE_MERIDIAN_SLICES_NUM + 1) + j + 1;
            indexCount++;
            pIndices[indexCount] = (i + 1) * (SPHERE_MERIDIAN_SLICES_NUM + 1) + j;
            indexCount++;
            pIndices[indexCount] = i * (SPHERE_MERIDIAN_SLICES_NUM + 1) + j;
            indexCount++;
            pIndices[indexCount] = i * (SPHERE_MERIDIAN_SLICES_NUM + 1) + j + 1;
            indexCount++;
            pIndices[indexCount] = (i + 1) * (SPHERE_MERIDIAN_SLICES_NUM + 1) + j + 1;
            indexCount++;
        }
    }
    return;
}
//--------------------------------------------------------------------------------------
// Calculate vertex positions for procedural sphere mesh based on an input index buffer
//--------------------------------------------------------------------------------------
float4 VSMain(VSIn input)
{
    PSIn output;
    
    float meridianPart = ( input.index % ( g_SphereMeridianSlices + 1 ) ) / float( g_SphereMeridianSlices );
    float parallelPart = ( input.index / ( g_SphereMeridianSlices + 1 ) ) / float( g_SphereParallelSlices );
    
    float angle1 = meridianPart * 3.14159265 * 2.0;
    float angle2 = ( parallelPart - 0.5 ) * 3.14159265;
    
    float cos_angle1 = cos( angle1 );
    float sin_angle1 = sin( angle1 );
    float cos_angle2 = cos( angle2 );
    float sin_angle2 = sin( angle2 );
        
    float3 VertexPosition;
    VertexPosition.z = cos_angle1 * cos_angle2;
    VertexPosition.x = sin_angle1 * cos_angle2;
    VertexPosition.y = sin_angle2;
    
    output.position = mul( float4( VertexPosition, 1.0 ), g_ModelViewProj );
    output.texCoord = float2( 1.0 - meridianPart, 1.0 - parallelPart );
    
    float3 tangent = float3( cos_angle1, 0.0, -sin_angle1 );
    float3 binormal = float3( -sin_angle1 * sin_angle2, cos_angle2, -cos_angle1 * sin_angle2 );
        
    float3 viewVector = normalize(g_EyePosition - VertexPosition);
    
    output.viewVectorTangent.x = dot( viewVector, tangent );
    output.viewVectorTangent.y = dot( viewVector, binormal);
    output.viewVectorTangent.z = dot( viewVector, VertexPosition );
    
    float3 lightVector = normalize( g_LightPosition );
    
    output.lightVectorTangent.x = dot( lightVector, tangent );
    output.lightVectorTangent.y = dot( lightVector, binormal);
    output.lightVectorTangent.z = dot( lightVector, VertexPosition );
    
    return output;
}

float4 VSMainQuad(VSIn input)
{
    float4 output;
    
    // We don't need to do any calculations here because everything
    // is done in the geometry shader.
    output.position = 0;
    output.texCoord = 0;
        
    return output;
}
#endif

//////////////////////////////////////////////////////////////////////


float sdSphere( vec3 p, float s )
{
    return length(p)-s;
}

vec2 scene( in vec3 pos )//reception d'une sphere
{
    vec3 dim = vec3(1, 1, 1);
    
    pos += vec3(0, 0., 0);
    
    float resSphere = sdSphere(pos, 1.3);
    
   
    vec2 res = vec2(resSphere, 2.);
    
    
    return res;
}

vec3 calcNormal( in vec3 pos )
{
    vec3 eps = vec3( 0.001, 0.0, 0.0 );
    vec3 nor = vec3(
                    scene(pos+eps.xyy).x - scene(pos-eps.xyy).x,
                    scene(pos+eps.yxy).x - scene(pos-eps.yxy).x,
                    scene(pos+eps.yyx).x - scene(pos-eps.yyx).x );
    return normalize(nor);
}

vec2 getUV(vec3 pos)
{
    vec3 nor = calcNormal(pos);
    float lon = atan(nor.x,nor.z)/3.14;
    float lat = acos(nor.y)/3.14;
    vec2 r = vec2(lat, lon);
    
    return r;
}


// Created by Alex Kluchikov
vec2 rot(vec2 p,float a)
{
    float c=cos(a*15.83+time*timeScale);
    float s=sin(a*15.83+time*timeScale);
    return p*mat2(s,c,c,-s);
}

vec3 makeSun(vec2 uv_)
{
    vec2 uv=uv_/iResolution.xx;
    uv=vec2(.125,.75)+(uv-vec2(.125,.75))*.015;
    float T=iTime*.5;

    vec3 c = normalize(.75-.25*vec3(
        sin(length(uv-vec2(.1,0))*132.+T*3.3),
        sin(length(uv-vec2(.9,0))*136.-T*2.5),
        sin(length(uv-vec2(.5,1))*129.+T*4.1)
        ));

    vec3 c0=vec3(0);
    float w0=0.;
#if 1
    float N=80.;
    float wb=.012;
    for(float i=0.;i<N;i++)
    {
        float wt=(i*i/N/N-.2)*.3;
        float wp=0.5+(i+1.)*(i+1.5)*0.001;
    	c.zx=rot(c.zx,1.6+T*0.65*wt+(uv.x+.7)*23.*wp);
    	c.xy=rot(c.xy,c.z*c.x*wb+1.7+T*wt+(uv.y+1.1)*15.*wp);
    	c.yz=rot(c.yz,c.x*c.y*wb+2.4-T*0.79*wt+(uv.x+uv.y*(fract(i/2.)-0.25)*4.)*17.*wp);
    	c.zx=rot(c.zx,c.y*c.z*wb+1.6-T*0.65*wt+(uv.x+.7)*23.*wp);
    	c.xy=rot(c.xy,c.z*c.x*wb+1.7-T*wt+(uv.y+1.1)*15.*wp);
        float w=(1.5-i/N);
        c0+=c*sqrt(w);
        w0+=sqrt(w);
    }
    c0=c0/w0*3.+.5;
#else
	c0=c;
#endif
    
	return vec3(sqrt(c0.r)*1.2,c0.r*c0.r*.9,c0.r*c0.r*c0.r*.4);
}

#if 0
void main()
{
	vec4 fragColor=vec4( 0.0, 0.0, 0.0, 1.0 );
	vec2 fragCoord=xlv_TEXCOORD0;

    float scale = 0.85;
    vec2 res = iResolution.xy;
    vec2 xy = (2. * fragCoord.xy - res ) / res.y / scale;
    
    float r = sqrt(dot(xy,xy));
    if( r > 1.0 )
    {
    	//fragColor = vec4( 0.0, 0.0, 0.0, 1.0 );
        return;
    }
    
    vec3 p = vec3( xy, sqrt( 1.0 - r*r ));
    float angle = iTime * 0.5;
    vec3 axis = vec3(0.2, 1, 0.6);
    
    // rotational matrix
    float ca = cos(angle);
    float sa = sin(angle);
    axis = normalize(axis);
    
    mat3 rot = mat3(
    ca + axis.x*axis.x*(1.-ca), axis.x*axis.y*(1.-ca) - axis.z*sa, axis.x*axis.z*(1.-ca) + axis.y*sa,
    axis.y*axis.x*(1.-ca) + axis.z*sa, ca + axis.y*axis.y*(1.-ca), axis.y*axis.z*(1.-ca) - axis.x*sa,
    axis.z*axis.x*(1.-ca) - axis.y*sa, axis.y*axis.z*(1.-ca) + axis.x*sa, ca + axis.z*axis.z*(1.-ca));
	
    vec3 rp = rot * p;

#if 0
    float u = 0.5 + atan(rp.z,rp.x) / PI;
    float v = 0.5 - asin(rp.y) / PI;
    fragColor.rgb = texture( iChannel0, vec2( u, v ));
#elif 1
    float u = 0.5 + atan(rp.z,rp.x) / PI;
    float v = 0.5 - asin(rp.y) / PI;
    fragColor.rgb = vec3(makeSun(vec2( u, v )));
#else
    vec3 hitPos=rp;
    float2 chekerUv = getUV(hitPos);
    fragColor.rgb = vec3(makeSun(chekerUv));
#endif
#if 0
    // directional light
    vec3 lightDir = normalize( vec3( -1.1, -0.3, 1.0 )); 
    float mflight = max(dot( p, lightDir), 0.0);
    fragColor.rgb *= mflight;
    

    // point light
    vec3 light1pos = vec3( 0.6, 0.5, 0.6 ); 
    vec3 light1col = vec3( 1.0, 2.0, 3.0 ); 
    float dis = 1.0 - length( p - light1pos ) / 2.0;
    fragColor.rgb += light1col * pow( dis, 8.0);
#endif
	gl_FragColor=fragColor;
}
#elif 0
vec3 nSphere( in vec3 pos, in vec4 sph ) {
    return (pos-sph.xyz)/sph.w;
}

float iSphere( in vec3 ro, in vec3 rd, in vec4 sph ) {
	vec3 oc = ro - sph.xyz;
	float b = dot( oc, rd );
	float c = dot( oc, oc ) - sph.w*sph.w;
	float h = b*b - c;
	if( h<0.0 ) return -1.0;
	return -b - sqrt( h );
}
vec4 renderPlanet( const in vec3 ro, const in vec3 rd, const in vec3 up, inout float maxd ) {
    float d = iSphere( ro, rd, vec4( 0., 0., 0., EARTH_RADIUS ) );

    vec3 intersection = ro + rd*d;
    vec3 n = nSphere( intersection, vec4( 0., 0., 0., EARTH_RADIUS ) );
    vec4 res=vec4(n, 1.0);
    return res;
}

void main()
{
	vec2 uv = xlv_TEXCOORD0.xy / resolution.xy;
    
    vec2 p = -1.0 + 2.0 * (xlv_TEXCOORD0.xy) / resolution.xy;
    p.x *= resolution.x/resolution.y;
    
    vec3 col;

    {
        // camera
        vec3 ro, ta, up;
        //cameraPath( time*.7, ro, ta, up );
    ro = vec3(1. ,7000. ,1500. );
    ta = vec3(    0. ,    0. ,   0. );
    up = vec3(    0. ,    0.4,   1. );

        vec3 ww = normalize( ta - ro );
        vec3 uu = normalize( cross(ww,up) );
        vec3 vv = normalize( cross(uu,ww));
        vec3 rd = normalize( -p.x*uu + p.y*vv + 2.2*ww );

        float maxd = MAX;  

        vec4 planet = renderPlanet( ro, rd, up, maxd );       
        if( planet.w > 0. ) col.xyz = planet.xyz;
   }
   gl_FragColor=vec4(col, 1.0);
}
#elif 1
void main()
{
    vec3 col = vec3(1,0,0);
#if 1
    vec2 p = -1.0 + 2.0 * (xlv_TEXCOORD0.xy) / resolution.xy;
    p.x *= resolution.x/resolution.y;
	vec2 texCoord=xlv_TEXCOORD0.xy / resolution.xy;
    // make us round
    float d = texCoord.x * texCoord.x + texCoord.y * texCoord.y;

    if (d > 1.0)
        discard;

    // we're in, compute the exact Z
    float z = sqrt(1.0 - d);

    // get light intensity
    vec4 point = vec4(texCoord.xy, z, 1.0);

    float l = clamp(dot(point, lightPos), minLight, 1.0);

    // rotate
    point *= rotation;

    // get texture coordinates (I believe this could be replaced with a
    // precomputed texture lookup, if you need more performance)
    float x = (atan(point.x, point.z) + PI) / TWOPI,
          y = (asin(point.y) + PIP2) / PI;
	vec2 uv=vec2(x,y);
	vec3 rp=point.xyz;
	vec3 hitPos=rp;
	float2 chekerUv = getUV(hitPos);
	col = vec3(makeSun(chekerUv));
#endif
	gl_FragColor=vec4(col, 1.0);
}
#endif

