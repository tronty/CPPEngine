#ifdef GL_ES
precision highp float;
#endif

// I should really add anti-aliasing or soft-shadows.  The former is probably low hanging fruit. 
// I can't think of an elegant solution to generating unique random numbers on every fragment 
// though to calculate all the offsets.

// broken in windows firefox
// Essentially, given some geometry and a ray, find the point at which the
// I should do a blog post about how I switched to Egyptian-brace style after picking up Rust ...
//
// do I need to explain what a directional light is?
// shader optimization, as with OpenCL, defer conditionals/returns as later as possible and
// when you have to branch, try to load both branches evenly.

// I start with a simple ray definition, consisting of an origin point and a
// direction vector. I also define a directional light to illuminate my scene.
struct Ray {
    vec3 origin;
    vec3 direction;
};

struct Light {
    vec3 color;
    vec3 direction;
};

// In real life, objects have many different material properties. Some objects
// respond very differently to light than others. For instance, a sheet of paper
// and a polished mirror. The former exhibits a strong *diffuse* response;
// incoming light is reflected at many angles. The latter is an example of a
// *specular* response, where incoming light is reflected in a single direction.
// To model this, I create a basic material definition. Objects in my scene
// share a single (RGB) color with diffuse and specular weights.
struct Material {
    vec3 color;
    float diffuse;
    float specular;
};

// To render the scene, I need to know where a ray intersects with an object.
// Since rays have infinite length from an origin, I can model the point of
// intersection by storing the distance along the ray. I also need to store the
// surface normal so I know which way to bounce! Once I create a ray, it loses
// the concept of scene geometry, so one more thing I do is forward the surface
// material properties.
struct Intersect {
    float len;
    vec3 normal;
    Material material;
};

// The last data structures I create are for objects used to fill my scene. The
// most basic object I can model is a sphere, which is defined as a radius at
// some center position, with some material properties. To draw the floor, I
// also define a simple horizontal plane centered at the origin, with a normal
// vector pointing upwards.
struct Sphere {
    float radius;
    vec3 position;
    Material material;
};

struct Plane {
    vec3 normal;
    Material material;
};

// Magic numbers ... everybody hates them! First up, I define an epsilon value.
// This is to help mitigate floating point errors.

// Due to floating point precision errors, when a ray intersects geometry at a
// surface, the point of intersection could possibly be just below the surface.
// The subsequent reflection ray would then bounce off the *inside* wall of the
// surface. This is known as self-intersection.
// ~~~~~~
const float epsilon = 1e-3;

// The classical ray tracing algorithm is recursive. However, GLSL does not
// support recursion, so I instead use an iterative approach to control the
// number of light bounces.
const int iterations = 10;

// Next, I define an exposure time and gamma value. These are used to control
// the image brightness. This is different from changing the intensity of
// ~~~~~
const float exposure = 1e-2;
const float gamma = 2.2;

// At this point, I create a basic directional light. I also define the ambient
// light color; the color here is mostly a matter of taste.
const float intensity = 50.0;
const vec3 ambient = vec3(0.6, 0.8, 1.0) * intensity / gamma;
// For a Static Light
// Light light = Light(vec3(1.0) * intensity, normalize(vec3(-1.0, 0.75, 1.0)));

// For a Rotating Light
 Light light = Light(vec3(1.0) * intensity, normalize(
                vec3(-1.0 + 4.0 * cos(time), 4.75,
                      1.0 + 4.0 * sin(time))));

// I strongly dislike this line. I needed to know when a ray hits or misses a
// surface. If it hits geometry, I returned the point at the surface. Otherwise,
// the ray misses all geometry and instead hits the skybox. In a language that
// supports dynamic return values, I could `return false`, but that is not an
// option in GLSL. In the interests of making progress, I created an intersect
// of distance zero to represent a miss and moved on.
const Intersect miss = Intersect(0.0, vec3(0.0), Material(vec3(0.0), 0.0, 0.0));

// As indicated earlier, I implement ray tracing for spheres. I need to compute
// the point at which a ray intersects with a sphere. [Line-Sphere](http://en.wikipedia.org/wiki/Line-sphere_intersection)
// intersection is relatively straightforward. For reflection purposes, a ray
// either hits or misses, so I need to check for no solutions, or two solutions.
// In the latter case, I need to determine which solution is "in front" so I can
// return an intersection of appropriate distance from the ray origin.
Intersect intersect(Ray ray, Sphere sphere) {
    // Check for a Negative Square Root
    vec3 oc = sphere.position - ray.origin;
    float l = dot(ray.direction, oc);
    float det = pow(l, 2.0) - dot(oc, oc) + pow(sphere.radius, 2.0);
    if (det <= 0.00001) return miss;

    // Find the Closer of Two Solutions
             float len = l - sqrt(det);
    if (len < 0.0) len = l + sqrt(det);
    if (len < 0.0) return miss;
    return Intersect(len, (ray.origin + len*ray.direction - sphere.position) / sphere.radius, sphere.material);
}

// Since I created a floor plane, I also have to handle reflections for planes
// by implementing [Line-Plane](http://en.wikipedia.org/wiki/Line-plane_intersection)
// intersection. I only care about the intersect for the purposes of reflection,
// so I only check if the quotient is non-zero.
Intersect intersect(Ray ray, Plane plane) {
    float len = -dot(ray.origin, plane.normal) / ( dot(ray.direction, plane.normal) + 0.0001 );
	if (len < 0.0) return miss;
	else return Intersect(len, plane.normal, plane.material); 
}

// In a *real* ray tracing renderer, geometry would be passed in from the host
// as a mesh containing vertices, normals, and texture coordinates, but for the
// sake of simplicity, I've hand-coded the scenegraph. In this function, I take
// an input ray and iterate through all geometry to determine intersections.
Intersect trace(Ray ray) {
    const int num_spheres = 3;
    Sphere spheres[num_spheres];

    // I initially started with the [smallpt](www.kevinbeason.com/smallpt/)
    // scene definition, but soon found performance was abysmal on very large
    // spheres. I kept the general format, modified to fit my data structures.

    spheres[0] = Sphere(2.0, vec3(-4.0, 3.0 + sin(time), 0), Material(vec3(1.0, 0.0, 0.0), 1.0, 0.001));
    spheres[1] = Sphere(3.0, vec3( 4.0 + cos(time), 3.0, 0), Material(vec3(0.0, 1.0, 0.0), 1.0, 0.001));
    spheres[2] = Sphere(1.0, vec3( 0.5, 1.0, 6.0),           Material(vec3(0.0, 0.0, 1.0), 0.5, 0.25));

    // Since my ray tracing approach involves drawing to a 2D quad, I can no
    // longer use the OpenGL Depth and Stencil buffers to control the draw
    // order. Drawing is therefore sensitive to z-indexing, so I first intersect
    // with the plane, then loop through all spheres back-to-front.

    Intersect intersection = miss;
    Intersect plane = intersect(ray, Plane(vec3(0, 1, 0), Material(vec3(1.0, 1.0, 1.0), 1.0, 0.0)));
    if (plane.material.diffuse > 0.0 || plane.material.specular > 0.0) { intersection = plane; }
    for (int i = 0; i < num_spheres; i++) {
        Intersect sphere = intersect(ray, spheres[i]);
        if (sphere.material.diffuse > 0.0 || sphere.material.specular > 0.0)
            intersection = sphere;
    }
    return intersection;
}

// This is the critical part of writing a ray tracer. I start with some empty
// scratch vectors for color data and the Fresnel factor. I trace the scene with
// using an input ray, and continue to fire new rays until the iteration depth
// is reached, at which point I return the total sum of the color values from
// computed at each bounce.
vec3 radiance(Ray ray) {
    vec3 color = vec3(0.0), fresnel = vec3(0.0);
    vec3 mask = vec3(1.0);
    for (int i = 0; i <= iterations; ++i) {
        Intersect hit = trace(ray);

        // This goes back to the dummy "miss" intersect. Basically, if the scene
        // trace returns an intersection with either a diffuse or specular
        // coefficient, then it has encountered a surface of a primitive.
        // Otherwise, the current ray has reached the ambient-colored skybox.

        if (hit.material.diffuse >= 0.01 || hit.material.specular > 0.001) {

            // Here I use the [Schlick Approximation](http://en.wikipedia.org/wiki/Schlick's_approximation)
            // to determine the Fresnel specular contribution factor, a measure
            // of how much incoming light is reflected or refracted. I compute
            // this value first and then subtract it out later when I do the
            // diffuse coloring to create a smooth gradient background effect.

            vec3 r0 = hit.material.color.rgb * hit.material.specular;
            float hv = clamp(dot(hit.normal, -ray.direction), 0.0, 1.0);
            fresnel = r0 + (1.0 - r0) * pow(1.0 - hv, 5.0); 
		mask *= fresnel;

            // Here is where I handle shadows and diffuse colors. I condensed
            // this part into one conditional evaluation for brevity. Remember
            // `epsilon` the magic ~~dragon~~ number? I use this to fire a new
            // ray slightly offset from the original point of intersection in
            // the *direction of the light source* to determine if the intersect
            // should be in a shadow. FIXME

            // ~~~~~~~~~~~~~~~~

 	    // the condition statement can not be compiled on my pc/win7/GeForceGT610 -> compiled with errors
	    if (trace(Ray(ray.origin + hit.len * ray.direction + epsilon * light.direction, light.direction)) == miss) 

            Intersect isec = trace(Ray(ray.origin + hit.len * ray.direction + epsilon * light.direction, light.direction));
//	    if (isec == miss)  // !!! this condition statement is the malefactor !!!
	    {
                color += clamp(dot(hit.normal, light.direction), 0.0, 1.0) * light.color
                       * hit.material.color.rgb * hit.material.diffuse  // Add Diffuse
                       * (1.0 - fresnel) * mask / fresnel;         // Subtract Specular
            }


            // After computing diffuse colors, I then generate a new reflection
            // ray and overwrite the original ray that was passed in as an
            // argument to the radiance(...) function. Then I repeat until I
            // reach the iteration depth.

            vec3 reflection = reflect(ray.direction, hit.normal);
            ray = Ray(ray.origin + hit.len * ray.direction + epsilon * reflection, reflection);

        } else {

            // This is the other half of the tracing branch. If the trace failed
            // to return an intersection with an attached material, then it is
            // safe to assume that the ray points at the sky, or out of bounds
            // of the scene. At this point I realized that real objects have a
            // small sheen to them, so I hard-coded a small spotlight pointing
            // in the same direction as the main light for pseudo-realism.

            vec3 spotlight = vec3(1e8
				 ) * pow(abs(dot(ray.direction, light.direction)), 250.0);
            color += mask * (ambient + spotlight); break;
        }
    }
    return color;
}

// The main function primarily deals with organizing data from OpenGL into a
// format that the ray tracer can use. For ray tracing, I need to fire a ray for
// each pixel, or more precisely, a ray for every fragment. However, pixels to
// fragment coordinates do not map exactly, so I need to divide the fragment
// coordinates by the viewport resolution. I then offset that by a fixed value
// to re-center the coordinate system.

// void mainImage(out vec4 fragColor, in vec2 fragCoord) {
//     vec2 uv    = fragCoord.xy / iResolution.xy - vec2(0.5);
//          uv.x *= iResolution.x / iResolution.y;

//     // For each fragment, create a ray at a fixed point of origin directed at
//     // the coordinates of each fragment. The last thing before writing the color
//     // to the fragment is to post-process the pixel values using tone-mapping.
//     // In this case, I adjust for exposure and perform linear gamma correction.

//     Ray ray = Ray(vec3(0.0, 2.5, 12.0), normalize(vec3(uv.x, uv.y, -1.0)));
//     fragColor = vec4(pow(radiance(ray) * exposure, vec3(1.0 / gamma)), 1.0);
// }


void main() {
//    vec2 uv = 2. * gl_FragCoord.xy / resolution.xy - 1.;
  // vec2 uvs = uv * resolution.xy / max(resolution.x, resolution.y);
    vec2 uv    = gl_FragCoord.xy / resolution.xy - vec2(0.5);
         uv.x *= resolution.x / resolution.y;

    // For each fragment, create a ray at a fixed point of origin directed at
    // the coordinates of each fragment. The last thing before writing the color
    // to the fragment is to post-process the pixel values using tone-mapping.
    // In this case, I adjust for exposure and perform linear gamma correction.

    Ray ray = Ray(vec3(0.0, 2.5, 12.0), normalize(vec3(uv.x, uv.y, -1.0)));
    gl_FragColor = vec4(pow(radiance(ray) * exposure, vec3(1.0 / gamma)), 1.0);
}

