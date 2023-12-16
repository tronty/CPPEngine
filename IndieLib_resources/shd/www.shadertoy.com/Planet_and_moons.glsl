const float M_PI = 3.14159265358979323846264338327950288;

// Field of View:
const float FOV = 60.0 * M_PI/180.0;

const int MAX_NUM_SPHERES = 2;
vec3 spherePos[MAX_NUM_SPHERES];
float sphereRadius[MAX_NUM_SPHERES];

//----------------------------------------------------------------------------//

bool TestRayAgainstSphere(in vec3 rayStart, in vec3 rayDir,
                          in vec3 spherePos, in float sphereRadius,
                          inout vec3 hitPos, inout vec3 normal)
{
    vec3 camPosPrime = rayStart - spherePos;
    float d = dot(rayDir, camPosPrime);
    float disc = d*d - dot(camPosPrime, camPosPrime) + sphereRadius*sphereRadius;
    if(disc > 0.0)
    {
        // Ray intersects sphere, get t-value of the intersection.
        float sigma = -d - sqrt(disc);
        
        // Use to get the intersection point in world space and obtain the
        // surface normal.
        hitPos = rayStart + sigma*rayDir;
        normal = normalize(hitPos - spherePos);

        return true;
    }
    else
    {
        return false;
    }
}

//----------------------------------------------------------------------------//

void UpdatePlanets(in float iTime)
{
    spherePos[0] = vec3(0.0, 0.0, 5.0);
    sphereRadius[0] = 1.0;
    
    // Moons:
    float fTime = iTime/1.0;
    float orbitalRadius = 2.0;
    spherePos[1] = vec3(orbitalRadius*sin(-fTime) + spherePos[0].x,
                        0.0,
                        orbitalRadius*cos(-fTime) + spherePos[0].z);
    sphereRadius[1] = 0.2;
}

//----------------------------------------------------------------------------//

void main()
{
    vec4 outputCol = vec4(0.0, 0.0, 0.0, 1.0);
    float fragDepth = 1e10;
    
    // Position planetary system.
    UpdatePlanets(iTime);
    
    // Update position of Sun to simulate orbiting seen from perspective of
    // parent planet.
    float planetOrbitRadius = 100.0;
    float orbitalSpeed = -0.1;
    vec3 lightPos = vec3(planetOrbitRadius*sin(orbitalSpeed*iTime) - spherePos[0].z,
                         0.0,
                         planetOrbitRadius*cos(orbitalSpeed*iTime) - spherePos[0].x);
	lightPos = vec3(1,0,0);

    // Camera position and position of centre of near clip plane.
    vec3 camPos = vec3(0.0, 0.0, 0.0);
    vec3 camForward = vec3(0.0, 0.0, 1.0);
    vec3 camRight = vec3(1.0, 0.0, 0.0);
    vec3 camUp = vec3(0.0, -1.0, 0.0);

    // Assume distance of 1 unit from camera to screen in order to compute
    // screen coordinates in world space.
    float screen_hw = tan(0.5*FOV);
    float screen_hh = screen_hw * float(iResolution.y)/float(iResolution.x);

    // World space coordinates of top-left pixel of screen.
    vec3 screenPos_0 = camPos + camForward - screen_hw*camRight + screen_hh*camUp;

    // And now for this particular pixel.
    vec3 screenPos = screenPos_0 + xlv_TEXCOORD0.x*2.0*screen_hw/float(iResolution.x)*camRight
                        - xlv_TEXCOORD0.y*2.0*screen_hh/float(iResolution.y)*camUp;

    // Define rays through each pixel in the near clip plane.
    vec3 rayDir = normalize(screenPos - camPos);

    // Test ray for intersection with sphere.
    vec3 hitPos;
    vec3 normal;
    for(int i = 0; i < MAX_NUM_SPHERES; i++)
    {
        if(TestRayAgainstSphere(camPos, rayDir, spherePos[i], sphereRadius[i], hitPos, normal))
        {
            // Lighting.
            vec3 hitPosToLight = lightPos - hitPos;
            vec3 reflectedLightDir = normalize(reflect(hitPosToLight, normal));

            float diffuse = 0.8*max(dot(normalize(hitPosToLight), normal), 0.0);
            float specular = 0.15*pow(max(dot(reflectedLightDir, rayDir), 0.0), 32.0);

            float ambient = 0.1;

            // Depth test.
            if(length(hitPos - camPos) < fragDepth)
            {
                fragDepth = length(hitPos - camPos);
                outputCol.rgb = vec3(diffuse + specular + ambient);
            }
        }
    }

    // Output to screen
    gl_FragColor = outputCol;
}
