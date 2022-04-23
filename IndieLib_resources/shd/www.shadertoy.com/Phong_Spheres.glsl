
// Constants #######################################################################

const vec3 cameraPos = vec3(0.0, 1.3, 0.0);	// Position of the camera in world space
const vec3 viewPlane = cameraPos + vec3(0.0, 0.0, 1.0);	// Position of the view-plane in world space (point that camera is looking at)
#define noMat Material(0.0, 0.0, 0.0, 0.0, vec3(0.0, 0.0, 0.0))



// Structs #########################################################################

struct Light {
    vec3 pos;
    vec3 color;
    vec3 att; //constant, linear, quadratic attenuation factors
};

struct Material {
    float ambient_str;
    float diffuse_str;
    float specular_str;
    float shininess;
    vec3 color;
};

struct Plane {
    vec3 pos;
    vec3 nrm;
    Material mat;
};
    
struct Sphere {
    vec3 pos;
    float r;
    Material mat;
};
    
struct Ray {
    vec3 origin;
    vec3 dir;
};
    
struct Hit {
    bool hit;
    vec3 pos;
    vec3 nrm;
    Material mat;
};
    
    
    
// Scene Objects ####################################################################
    
//Lights
Light lights[] = Light[](
    //    position             //color              //attenuation
    Light(vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0), vec3(1.0, 0.5, 0.5)),	// Moving white Light
    Light(vec3(-1.5, 0.0, -1.5), vec3(1.0, 0.0, 0.0), vec3(0.0, 0.2, 0.2)),	// Red light bottom left
    Light(vec3(1.5, 0.0, -1.5), vec3(0.0, 0.8, 0.0), vec3(0.0, 0.2, 0.2)),	// Green light bottom right
    Light(vec3(0.0, 2.5, 0.0), vec3(0.0, 0.0, 1.0), vec3(0.0, 1.0, 0.5))	// Blue light top blinking
);
    
// Planes
Plane planes[] = Plane[](
    //    position             // surface-normal    //mat    //a  //d  //s  //sh  //color
    Plane(vec3(0.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), Material(0.1, 1.0, 1.0, 32.0, vec3(1.0, 1.0, 1.0)) ),
    Plane(vec3(-2.0, 0.0, 0.0), vec3(1.0, 0.0, 0.0), Material(0.1, 1.0, 1.0, 32.0, vec3(1.0, 1.0, 1.0)) ),
    Plane(vec3(2.0, 0.0, 0.0), vec3(-1.0, 0.0, 0.0), Material(0.1, 1.0, 1.0, 32.0, vec3(1.0, 1.0, 1.0)) )
);

Sphere spheres[] = Sphere[](
    //     position                //r   //mat    //a  //d  //s  //sh  //color
    Sphere(vec3(-1.50, 0.3, -3.0), 0.3f, Material(0.08, 0.0, 3.0, 512.0, vec3(1.0, 1.0, 1.0)) ),
    Sphere(vec3(-1.50, 1.0, -3.0), 0.3f, Material(0.08, 1.0, 0.5, 016.0, vec3(1.0, 0.3, 3.3)) ),
    Sphere(vec3(-1.50, 1.7, -3.0), 0.3f, Material(0.08, 1.0, 5.0, 016.0, vec3(1.0, 1.0, 1.0)) ),
    Sphere(vec3(-1.50, 2.4, -3.0), 0.3f, Material(0.08, 1.0, 0.0, 002.0, vec3(1.0, 1.0, 1.0)) ),
    Sphere(vec3(-0.75, 0.3, -3.0), 0.3f, Material(0.08, 1.0, 0.2, 064.0, vec3(0.0, 1.0, 0.0)) ),
    Sphere(vec3(-0.75, 1.0, -3.0), 0.3f, Material(0.08, 1.0, 1.0, 032.0, vec3(0.5, 0.5, 0.5)) ),
    Sphere(vec3(-0.75, 1.7, -3.0), 0.3f, Material(0.50, 1.0, 5.0, 128.0, vec3(1.0, 1.0, 1.0)) ),
    Sphere(vec3(-0.75, 2.4, -3.0), 0.3f, Material(0.08, 0.1, 8.0, 001.0, vec3(0.0, 1.0, 1.0)) ),
    Sphere(vec3(00.00, 0.3, -3.0), 0.3f, Material(0.50, 0.1, 0.5, 032.0, vec3(1.0, 1.0, 0.0)) ),
    Sphere(vec3(00.00, 1.0, -3.0), 0.3f, Material(0.08, 1.0, 1.0, 032.0, vec3(1.0, 1.0, 1.0)) ),
    Sphere(vec3(00.00, 1.7, -3.0), 0.3f, Material(0.08, 1.0, 0.0, 016.0, vec3(0.2, 0.2, 0.2)) ),
    Sphere(vec3(00.00, 2.4, -3.0), 0.3f, Material(0.08, 2.5, 0.1, 001.0, vec3(1.0, 1.0, 1.0)) ),
    Sphere(vec3(00.75, 0.3, -3.0), 0.3f, Material(0.08, 0.6, 0.1, 128.0, vec3(0.9, 0.2, 0.2)) ),
    Sphere(vec3(00.75, 1.0, -3.0), 0.3f, Material(0.25, 1.0, 0.1, 001.0, vec3(0.24, 0.14, 0.0)) ),
    Sphere(vec3(00.75, 1.7, -3.0), 0.3f, Material(0.08, 1.0, 1.0, 016.0, vec3(1.0, 0.0, 0.0)) ),
    Sphere(vec3(00.75, 2.4, -3.0), 0.3f, Material(0.15, 1.5, 1.0, 128.0, vec3(0.0, 1.0, 0.35)) ),
    Sphere(vec3(01.50, 0.3, -3.0), 0.3f, Material(0.08, 1.0, 9.0, 001.0, vec3(0.01, 0.01, 0.01)) ),
    Sphere(vec3(01.50, 1.0, -3.0), 0.3f, Material(0.00, 0.7, 0.0, 032.0, vec3(1.0, 1.0, 1.0)) ),
    Sphere(vec3(01.50, 1.7, -3.0), 0.3f, Material(0.20, 0.8, 1.0, 128.0, vec3(1.0, 0.7, 0.2)) ),
    Sphere(vec3(01.50, 2.4, -3.0), 0.3f, Material(0.08, 1.0, 1.0, 002.0, vec3(0.0, 0.0, 1.0)) ) 
);



// Math ######################################################################

// Returns point where a ray intersects a given plane p
Hit ray_plane(Ray r, Plane p) {    
    // Check if ray is parallel to plane
    if(dot(p.nrm, r.dir)==0.0) return Hit(false, vec3(0.0), vec3(0.0), noMat);
    
    // See: https://en.wikipedia.org/wiki/Line%E2%80%93plane_intersection
    float d = dot((p.pos-r.origin), p.nrm) / dot(r.dir, p.nrm);
    if(d<=0.0) return Hit(false, vec3(0.0), vec3(0.0), noMat); //collision *behind* origin -> no collision
    return Hit(true, r.origin + d*r.dir, p.nrm, p.mat);
}

// Returns points where a ray intersects a given sphere sph
Hit ray_sphere(Ray r, Sphere sph) {
    // A sphere is characterized by dot(P-s_origin, P-s_origin) - r^2 = 0
    // A ray is characterized by p(t) = r_origin + t*dir
    // Inserting p(t) into P of the sphere equation yields: dot(p(t)-s_origin, p(t)-s_origin) - r^2 = 0
    // When expanded and solved for t we get following: http://viclw17.github.io/2018/07/16/raytracing-ray-sphere-intersection/
    float a = dot(r.dir, r.dir);
    vec3 so_ro = r.origin - sph.pos;
    float b = 2.0 * dot(r.dir, so_ro);
    float c = dot(so_ro, so_ro) - (sph.r * sph.r);
    
    float disr = b*b - 4.0*a*c; //discriminant
    
    // Doesn't hit
    if (disr < 0.0) {
        return Hit(false, vec3(0.0), vec3(0.0), noMat);
    }
    // hits
    vec3 hitpos = r.origin + r.dir*(-b - sqrt(disr))/(2.0*a);
    vec3 normal = normalize(hitpos-sph.pos);
    return Hit(true, hitpos, normal, sph.mat);
}



// Shading ######################################################################

vec3 phong(vec3 pos, vec3 nrm, Material mat, Light light, vec3 viewPos) {
    // Ambient
    vec3 ambient = mat.ambient_str * light.color;			// ambient color
  	
    // Diffuse
    vec3 lightDir = normalize(light.pos - pos);				// Direction of lightsource
    float diff = max(dot(nrm, lightDir), 0.0);				// diffuse factor
    vec3 diffuse = mat.diffuse_str * diff * light.color;	// diffuse color

    // Glossy
    vec3 viewDir = normalize(viewPos - pos);				// View direction
    vec3 reflectDir = reflect(-lightDir, nrm); 				// Perfect reflectance direction 
    float spec = pow(max(dot(reflectDir, viewDir), 0.0), 8.0); // specular factor
    vec3 specular = mat.specular_str * spec * light.color;  // Specular color

    // Attenuation (Const. Lin. Quadratic)
    float dist = length(light.pos - pos);
    float attenuation = 1.0f / (light.att.x + light.att.y * dist + light.att.z * (dist * dist));
    
    // Final result
    return (ambient + diffuse + specular) * mat.color * attenuation;
}

// Returns a ray through any given fragment coordinate of a virtual camera
Ray cameraRay(vec2 xlv_TEXCOORD0) {
    // Viewplane x-Axis is cross product of global up-Vector and camera forward vector
    vec3 xvec = normalize(cross(vec3(0.0, 1.0, 0.0), cameraPos-viewPlane));
    // Viewplane y-Axis is cross product of viewplane x-Axis and camera forward vector
    vec3 yvec = normalize(cross(xvec, cameraPos-viewPlane));
    
    vec2 uv = xlv_TEXCOORD0/iResolution.xy; 	// Normalized screen coordiantes [0, 1]
    uv -= 0.5;								// Screen coordiantes centered [0,1] -> [-0.5, 0.5]	
    uv.x *= iResolution.x / iResolution.y; 	// Apply aspect ration of resolution
    
    // Get virtual pixel position on virtual screen at viewPlane in world space
    vec3 vpp = viewPlane + xvec*uv.x + yvec*uv.y;
    // This gives us the desired ray direction
    vec3 rayDir = cameraPos - vpp;
    
    return Ray(cameraPos, rayDir);
}

// Returns closest collision point of any given ray on all objects
Hit rayTrace(Ray ray) {
    // Set some non-collision point very far away as the closest
    Hit closest = Hit(false, vec3(9999.0, 9999.0, 9999.0), vec3(0), noMat);
    
    // Trace to each sphere, if collision -> keep closest point  to ray-origin
    for(int i=0; i<spheres.length(); i++) {
        Hit h = ray_sphere(ray, spheres[i]);
        if(h.hit && length(h.pos - ray.origin) < length(closest.pos - ray.origin)) {
            closest = h;
        }
    }
    
    // Trace to each plane, if collision -> keep closest point  to ray-origin
    for(int i=0; i<planes.length(); i++) {
        Hit h = ray_plane(ray, planes[i]);
        if(h.hit && length(h.pos - ray.origin) < length(closest.pos - ray.origin)) {
            closest = h;
        }
    }
    
    return closest;
}

// Move lights over time
void updateLights() {
    lights[0].pos = vec3(1.9*sin(iTime), 1.5, -3.0+1.3*cos(iTime));
    lights[3].color.b = sin(iTime*0.8f);    
}


// Main shader
void main( )
{
    updateLights();
    
    // Trace a ray through the respective pixel
    Hit h = rayTrace(cameraRay(xlv_TEXCOORD0));
    
    // If we hit something, calculate respective phong color
    //if(h.hit) gl_FragColor = vec4(phong(h.pos, h.nrm, vec3(0.0, 1.0, 0.0)) ,1.0);    
    //else gl_FragColor = vec4(0.0);
    
    vec3 col = vec3(0.0);
    if(h.hit) {
    	for(int i=0; i<lights.length(); i++) {
        	col += phong(h.pos, h.nrm, h.mat, lights[i], cameraPos);
    	}
    }
    
    gl_FragColor = vec4(col, 1.0);
}

