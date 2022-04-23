vec3 camera = vec3(0,0.2,2);
float viewportDistance = 2.0;
float height = -0.5;
float PI = 3.1415926535;
float cameraDistance = 3.0;
float fogDistance = 5.0;
vec3 light = normalize(vec3(1.0,1.0,1.0));



struct Hit
{
    float dist;
    float distToCamera;
    int ball;
    vec3 normal;
    vec3 position;
    vec3 color;
    float reflectionIndex;
    bool hitRefreaction;
};

Hit hits[4];

struct Sphere
{
    vec3 position;
    float radius;
    vec3 color;
    float reflectionIndex;
    bool toRefract;
};



vec3 Checkers (in vec2 xlv_TEXCOORD0)
{
    float gridSize = 1.0;
   
    float g = mod(floor((xlv_TEXCOORD0.x) / gridSize) + floor((xlv_TEXCOORD0.y) / gridSize),2.0);

    return mix(vec3(1.0),vec3(0.0),g);
    
    
}

vec3 Ray(in vec2 position)
{
    vec2 uv = (position/iResolution.xy*2.0)-vec2(1,1);
    uv.x *= iResolution.x/iResolution.y;
    vec3 worldPos = vec3(uv,0.0);
    
    
    vec3 viewportDirection = normalize(camera * -1.0);
    
    vec3 x = cross(viewportDirection, vec3(0,1,0));
    vec3 y = cross(x , viewportDirection);
    worldPos = worldPos.x * x + worldPos.y * y;
    
  
    vec3 viewportPosition = viewportDirection * viewportDistance + camera;
    
    
    
    
    vec3 ray = worldPos - camera + viewportPosition;
    
    return normalize(ray);
}


Hit PlaneIntersection(in vec3 ray, in vec3 position)
{
    Hit hit;
    
    hit.dist = (height - position.y)/ ray.y;
    if(hit.dist > 0.0)
    {
        vec3 pos = (ray * hit.dist) + position;
        hit.color = Checkers(pos.xz);
        
        hit.position = camera + ray*hit.dist;
        hit.distToCamera = distance(hit.position, camera);
    
        hit.normal = vec3(0,1,0); 
        
        hit.position += hit.normal *0.0001;
        
        hit.reflectionIndex = 0.2;
    }
    
    return hit;
    

}

Hit SphereIntersection(in vec3 ray,in Sphere ball,in vec3 origin)
{
    Hit hit;
    float t1 = dot(ray, ball.position - origin);
    vec3 k = origin + t1 * ray;
    float l1 = length(ball.position - k);
    float l2 = sqrt(ball.radius * ball.radius - l1 * l1);
    hit.dist = t1 -l2;
        
    if(hit.dist > 0.0)
    {        
        hit.position = origin + ray*hit.dist;
        hit.distToCamera = distance(hit.position, camera);
        hit.normal = normalize(hit.position - ball.position);
        hit.position += hit.normal *0.0001;
        hit.color = ball.color;
        hit.reflectionIndex = ball.reflectionIndex;
    }

    return hit;
}

vec3 SkyColor( in vec3 ray)
{
    float f = clamp(ray.y, 0.0, 1.0);
    return mix(vec3(0.85, 0.9, 1.0), vec3(0.2,0.4,1.0), f);
}



void RotateCamera()
{
    float alpha = 2.0 * PI * mod(iTime * 0.05, 2.0);
    camera = cameraDistance * vec3(cos(alpha), camera.y, sin(alpha));
}


Hit checkHit(in vec3 ray, in vec3 origin, in int toIgnore)
{
    Hit returnHit;
    returnHit.color = vec3(0,1,1);
    
    //FIRST BALL
    Sphere sphere;
    sphere.position = vec3(-0.7,0.45 + sin(iTime)/6.0,-0.5);
    sphere.radius = 0.5;
    sphere.color = vec3(0.0,0.4,0.0);
    sphere.reflectionIndex = 0.1;

    
    //SECOND BALL
    Sphere sphere2;
    sphere2.position = vec3(0.7,0.3 + cos(iTime)/6.0,-0.7);
    sphere2.radius = 0.5;
    sphere2.color = vec3(0.0,0.0,0.7);
    sphere2.reflectionIndex = 0.2;
    
    //THIRD BALL
    Sphere sphere3;
    sphere3.position = vec3(0.3,0.5 + cos(iTime + 1.0)/6.0,0.5);
    sphere3.radius = 0.5;
    sphere3.color = vec3(0.7,0.0,0.0);
    sphere3.reflectionIndex = 0.1;

    
    hits[0] = PlaneIntersection(ray, origin);
    hits[1] = SphereIntersection(ray,sphere,origin);
    hits[2] = SphereIntersection(ray,sphere2,origin);
    hits[3] = SphereIntersection(ray,sphere3,origin);
    
    float maxDist = 9999.0;

    for(int i = 0; i < 4; i++)
    {
        if(i != toIgnore)
        if(hits[i].dist > 0.0 && hits[i].dist < maxDist)
        {
            maxDist = hits[i].dist;
            returnHit = hits[i];
            returnHit.ball = i;
            
            //This is used to make the first ball the one with refraction
            if(i == 1)
            {
               returnHit.hitRefreaction = true;
            }
            else
            {
                returnHit.hitRefreaction = false;
            }
            ////////////////////////////////////////////////////////////
            
        }
    }
   
    return returnHit;
}

vec3 Illumination(in Hit hit, in vec3 ray)
{
    vec3 initialColor = hit.color;
    hit.color = mix(vec3(0),initialColor, dot(hit.normal,light))* 0.7;
    hit.color += initialColor * 0.2;
    
    vec3 specVec = reflect(light, hit.normal);
    
    float spec = dot(normalize(specVec), ray);
    spec = clamp(spec, 0.0, 1.0);
    spec = pow(spec, 50.0);
      
    
    hit.color += vec3(1) * spec;
    
    Hit shadoHit = checkHit(light,hit.position, hit.ball);
    
    if(shadoHit.dist > 0.0)
    {
        hit.color = vec3(0) + initialColor * 0.2 + mix(vec3(0),initialColor, dot(hit.normal,light))* 0.7*0.2;
    }
    
    return hit.color;
}

vec3 Reflection(in Hit hit, vec3 ray)
{
    vec3 reflectionVector = reflect(ray, hit.normal);
    
    Hit reflectionHit = checkHit(reflectionVector, hit.position, -1);
    
    if(reflectionHit.dist > 0.0)
    {
        reflectionHit.color = Illumination(reflectionHit, reflectionVector);
    }
    else
    {
        reflectionHit.color = SkyColor(reflectionVector);
    }
    
    return reflectionHit.color;
}

vec3 Refraction(in Hit hit, vec3 ray)
{
    vec3 refractionVector = refract(ray, hit.normal,0.3);
    
    Hit refractionHit = checkHit(refractionVector, hit.position, 1);
    
    if(refractionHit.dist > 0.0)
    {
        refractionHit.color = Illumination(refractionHit, refractionVector);
    }
    else
    {
        refractionHit.color = SkyColor(refractionVector);
    }
    
    return refractionHit.color;
}


vec3 PixelColor(in vec2 coords)
{
    vec3 ray = Ray(coords);
    Hit hit = checkHit(ray,camera, -1);
   
    if(hit.dist > 0.0)
    {
    hit.color = Illumination(hit,ray);
    
    if(hit.hitRefreaction != true)
    {
        vec3 reflectColor = Reflection(hit, ray);
        float fresnel = hit.reflectionIndex + (1.0 - hit.reflectionIndex ) * pow(1.0 - dot(hit.normal, -ray), 5.0);
        hit.color = mix(hit.color, reflectColor,fresnel);
    }
    else
    {        
        hit.color = Refraction(hit, ray);
        
        vec3 reflectColor = Reflection(hit, ray);
        float fresnel = hit.reflectionIndex + (1.0 - hit.reflectionIndex ) * pow(1.0 - dot(hit.normal, -ray), 5.0);
        
        hit.color = mix(hit.color, reflectColor,fresnel);
    }
    }
    else
    {
    hit.color = SkyColor(ray);
    }
       
       
    if(hit.dist > fogDistance)
    {
        float range = smoothstep(fogDistance, 20.0, hit.dist);
        hit.color = mix(hit.color,SkyColor(ray),range);
    }  
    
    return hit.color;

}

void main( )
{    
   
    RotateCamera();
    
    vec3 color = vec3(0);

    vec2 positions[4];
    
    positions[0] = vec2(xlv_TEXCOORD0.x - 0.5,xlv_TEXCOORD0.y - 0.5);
    positions[1] = vec2(xlv_TEXCOORD0.x - 0.5,xlv_TEXCOORD0.y + 0.5);
    positions[2] = vec2(xlv_TEXCOORD0.x + 0.5,xlv_TEXCOORD0.y + 0.5);
    positions[3] = vec2(xlv_TEXCOORD0.x + 0.5,xlv_TEXCOORD0.y - 0.5);
                
    color += PixelColor(positions[0]);
    color += PixelColor(positions[1]);
    color += PixelColor(positions[2]);
    color += PixelColor(positions[3]);

    color = color/4.0;

    // Output to screen
    gl_FragColor = vec4(color,1.0);
}

