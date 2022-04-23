#define MAX_STEPS 100
#define MAX_DIST 100. 
#define EPS .001
vec3 rotate(vec3 v, float a, float b, float c)
{
    //v = normalize(v);
    vec3 new_v = vec3(
        cos(b)*cos(c)*v.x 
            + (sin(a)*sin(b)*cos(c) - cos(a)*sin(c))*v.y 
            + (cos(a)*sin(b)*cos(c) + sin(a)*sin(c))*v.z,
            
        cos(b)*sin(c)*v.x 
            + (sin(a)*sin(b)*sin(c) + cos(a)*cos(c))*v.y 
            + (cos(a)*sin(b)*sin(c) - sin(a)*cos(c))*v.z,
            
        -sin(b)*v.x + sin(a)*cos(b)*v.y + cos(a)*cos(b)*v.z
    );
    return new_v; 
}

float smoothMin(float dstA, float dstB, float k)
{
    float h = max(k - abs(dstA - dstB), 0.) / k; 
    return min(dstA, dstB) - h*h*h*k * 1. / 6.0; 
}

float sphereDist(vec3 point, vec4 sphere)
{
    float sphere_dist = length(point - sphere.xyz)-sphere.w;
    return sphere_dist;
}

float cubeDist(vec3 eye, vec3 centre, vec3 size)
{
    eye = rotate(eye, 0., 0.3*sin(iTime), 0.5*cos(iTime));
    vec3 o = abs(eye-centre) -size;
    float ud = length(max(o,0.));
    float n = max(max(min(o.x,0.),min(o.y,0.)), min(o.z,0.));
    return ud+n;
}

float torusDist(vec3 eye, vec3 centre, float r1, float r2)
{   
    eye = rotate(eye, 0.2*sin(iTime), 0.02, 0.3*cos(iTime));
    vec2 q = vec2( length( (eye-centre).xz )-r1, eye.y-centre.y);
    return length(q)-r2;
}



float GetSceneDistance(vec3 point, out int obj)
{
    vec4 sphere = vec4(0.+sin(iTime), 1.+sin(0.5*iTime), 6.+3.*cos(iTime), 0.4+0.2*clamp(cos(0.2*iTime), 0., 1.)); // (xyz, radius)
    vec4 sphere2 = vec4(0.+2.5*cos(iTime), 1.+0.5*sin(0.5*iTime), 6.+2.*sin(iTime), 0.5+0.2*clamp(sin(0.2*iTime), 0., 1.)); // (xyz, radius)
    vec3 cube_centre = vec3(0.+2.*sin(iTime),1.+sin(iTime),10.+ 2. * sin(iTime));
    vec3 cube_size = vec3(1, 1,1);    
    vec3 torus_centre = vec3(1.+0.2*cos(iTime), 0.7+ 0.2*sin(iTime), 7.+sin(iTime)); 
    float torus_r1 = 0.5;
    float torus_r2 = 0.2; 

    float sphere_dist = sphereDist(point, sphere);
    float sphere2_dist = sphereDist(point, sphere2);
    float cube_dist = cubeDist(point, cube_centre, cube_size);
    float torus_dist = torusDist(point, torus_centre, torus_r1, torus_r2);
    //float torus2_dist = torusDist(point, torus2_centre, 0.5, 0.2, 3.);

    //float cube2_dist = cubeDist(point, cube_centre+ 5.* cos(0.2 * iTime)*sin(0.1*iTime), 0.5 * cube_size);
    
    float plane_dist = abs(point.y + 1.); 
    //float plane2_dist = abs(point.x -50.); 
    //float plane3_dist = abs(point.x + 50.); 
    
    float k = 2.; 
    float d = 
        //smoothMin(
        smoothMin(
        smoothMin(
        smoothMin(
           smoothMin(sphere_dist, plane_dist,k), 
           sphere2_dist, k),
            cube_dist, k),
           torus_dist,k);
           //torus2_dist,k);
    
    float eps = 0.5;
    if( abs(sphere_dist - d) < eps)
        obj = 1;
    else if ( abs(sphere2_dist - d) < eps)
        obj = 2;
    else if ( abs(cube_dist - d) < eps)
        obj = 3;
    else if(abs(torus_dist - d) < eps)
        obj = 4;
    else 
        obj = 0;
    return d; 
}

float RayMarch(vec3 ray_origin, vec3 ray_dir, out int obj)
{
    float d = 0.; 
    for(int i = 0; i < MAX_STEPS; i++)
    {
        vec3 p = ray_origin + ray_dir * d;
        float ds = GetSceneDistance(p, obj); 
        d += ds; 
        if(d > MAX_DIST || ds < EPS) 
            break;  // hit object or out of scene
    }
    return d; 
}

vec3 GetNormal(vec3 point)
 {
    int obj;
    float d = GetSceneDistance(point, obj); 
    vec2 e = vec2(0.001, 0); 
    vec3 n = d - vec3(
        GetSceneDistance(point - e.xyy, obj),
        GetSceneDistance(point - e.yxy, obj),
        GetSceneDistance(point - e.yyx, obj)
    );
    
    return normalize(n); 
}

float GetLight(vec3 point)
{    
    vec3 light_pos = vec3(2, 6, 5); 
    light_pos += vec3(sin(iTime), 0, cos(iTime));
    vec3 to_light = normalize(light_pos - point); 
    vec3 normal = GetNormal(point); 
    
    float intensity = 0.6;
    float light = intensity * clamp(dot(to_light, normal), 0., 1.); 
    
    int obj;
    float d = RayMarch(point+normal*2.*EPS, to_light, obj);
    
    if (d < length(light_pos - point))
        light *= 0.3;
    
    return light;
}

vec3 getColor(vec2 uv, vec3 ray_origin)
{
    int obj;
    vec3 ray_dir = normalize(vec3(uv.x, uv.y, 1.));
    float d = RayMarch(ray_origin, ray_dir, obj);
    
    vec3 point = ray_origin + d * ray_dir;
       
    float diffuse_light = GetLight(point); 
    
    vec3 col = vec3(diffuse_light);
    
    if(obj == 0)
        col += 0.2 + 0.3*cos(iTime+uv.xyx+vec3(0,2,4));
       // col += 0.7* vec3(1.+0.1*sin(iTime), 0.6+ 0.05*cos(iTime), 1.+0.01*cos(iTime));
    else if(obj == 1) // sphere
        col += vec3(0.1, 0.4, 0);
    else if(obj == 2) // sphere 2
        col += vec3(0, 0.3, 0.5);
    else if(obj == 3) // cube 
        col += vec3(0.8, 0.4, 0);
    //else if(obj == 4) // torus
    else
        col += vec3(0.7, 0, 0.3);
    
    return col;
}


void main( )
{
    // Normalized pixel coordinates (from -0.5 to 0.5)
    vec2 uv = (xlv_TEXCOORD0 - .5*iResolution.xy)/iResolution.x;

    // Time varying pixel color
    vec3 ray_origin = vec3(0, 1, -2);
   
    vec3 col = getColor(uv, ray_origin);
    //vec3 col2 = getColor(vec2(uv.x,uv.y+1.), ray_origin);
    //vec3 col3 = getColor(vec2(uv.x+1.,uv.y), ray_origin);
    //vec3 col4 = getColor(vec2(uv.x-0.5,uv.y), ray_origin);
    //vec3 col5 = getColor(vec2(uv.x,uv.y-0.5), ray_origin);
   
    //vec3 col = (col1 + col2 + col3 + col4 + col5) / 5.0;
    // Output to screen
    gl_FragColor = vec4(col, 1.0);
}
