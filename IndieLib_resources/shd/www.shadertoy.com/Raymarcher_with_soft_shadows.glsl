struct material
{
    vec3 baseColor;
};

float join(float a, float b)
{
    return min(a, b);
}

float sdPlane(vec3 p, float y)
{
    return p.y - y;
}

float sdSphere(vec3 p, vec3 pos, float r)
{
    return length(p - pos) - r;
}

float sdBox( vec3 p, vec3 b )
{
    vec3 q = abs(p) - b;
    return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

float sdBox( vec3 p, vec3 b, vec3 bPos )
{
    return sdBox(p - bPos, b);
}

float sceneDist(vec3 p)
{
    return join(join(join(sdSphere(p, vec3(0.3, 0.0, 2.0), 0.2), sdSphere(p, vec3(-0.3, 0.0, 2.0), 0.2)), sdPlane(p, -0.3)), sdBox(p, vec3(0.5, 0.1, 0.5), vec3(0.0,-0.3, 2.0)));
}

vec3 calcNormal( in vec3 p ) // for function f(p)
{
    const float h = 0.0001; // replace by an appropriate value
    const vec2 k = vec2(1,-1);
    return normalize( k.xyy*sceneDist( p + k.xyy*h ) + 
                      k.yyx*sceneDist( p + k.yyx*h ) + 
                      k.yxy*sceneDist( p + k.yxy*h ) + 
                      k.xxx*sceneDist( p + k.xxx*h ) );
}

void march(vec3 dir, inout vec3 p, int maxSteps, float minDist, out bool hit, out float depth, out vec3 normal)
{
    vec3 oldP = p;
    depth = 0.0;
    for(int i = 0; i < maxSteps; i++)
    {
        depth = sceneDist(p);
        p += depth*dir;
        hit = depth < minDist;
        if(hit)
        {
            normal = calcNormal(p);
            depth = distance(oldP, p);
            return;
        }
    }
    normal = calcNormal(p);
}

void marchShadow(vec3 dir, vec3 p, int maxSteps, float minDist, out float inShadow, float k)
{
    vec3 oldP = p;
    float dist = 0.002;
    float totalDist = dist;

    p += dist*dir;
    inShadow = 1.0;
    for(int i = 0; i < maxSteps; i++)
    {
        dist = sceneDist(p);
        totalDist += dist;

        if(dist < minDist)
        {
            inShadow = 1.0;
            return;
        }
        
        p += dist*dir;
        inShadow = min(inShadow, k*dist/totalDist);
    }
    inShadow = 1.0-inShadow;
}

void main( )
{
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = xlv_TEXCOORD0/iResolution.xy;
    uv -= 0.5;
    uv.y *= iResolution.y/iResolution.x;

    vec3 eyeDir = normalize(vec3(uv * vec2(1.0, 1.0), 1.0));
    vec3 eyePos = vec3(0.0, 0.0, 0.0);
    vec2 mouse = iMouse.z > 0.0?iMouse.xy/iResolution.xy - 0.5 : vec2(sin(iTime), cos(iTime) + 2.0);
    vec3 lightingDir = normalize(vec3((mouse) * vec2(1.0, iResolution.y/iResolution.x) * 10.0, -1.0));
    
    bool hit = false;
    float depth = 0.0;
    vec3 normal = vec3(0.0);
    march(eyeDir, eyePos, 512, 0.0001, hit, depth, normal);
    
    float inShadow = 0.0;
    marchShadow(lightingDir, eyePos, 64, 0.00001, inShadow, 4.0);
    
    //lightingDir = vec3(0.0, 1.0, 0.0);
    float lighting = max(0.0, dot(normal, lightingDir));
    lighting *= (1.0-inShadow) + 0.5;
    lighting /= 1.5;

    //gl_FragColor = vec4(vec3(depth/8.0),1.0);
    gl_FragColor = vec4(vec3(lighting),1.0);
    //gl_FragColor = vec4(eyeDir.xy,0.0, 1.0);
}
