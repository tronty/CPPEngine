// Author: HongZhang
// Date: 08/26/2022

#define PI 3.141592653
#define FOV 45.
#define rad(x) x * PI / 180.
#define MaxV 1e10
#define Gramma(x) pow(x, vec3(1./2.2))

// Camera
const vec3 cam = vec3(0., 0., 3.5);

// define the SDF for sphere
float sphereSDF(vec3 p, float radius){
    return length(p) - radius;
}

vec3 sphereNormal(vec3 p){
    return normalize(p);
}

// compute the view direction
vec3 computeView(vec2 uv){
    uv = (uv - 0.5) * 2.;
    float h = tan(rad(FOV)/2.);
    float w = h * (iResolution.x / iResolution.y);
    return normalize(vec3(w * uv.x, h * uv.y, -1.));
}

// compute the sphere-line intersection 
vec2 computeIntersection(vec3 origin, vec3 ray, float radius){
    float factor = pow(dot(origin, ray), 2.) - (dot(origin, origin) - radius * radius);
    if(factor >= 0.){
        float t1 = -dot(origin, ray) - sqrt(factor);
        float t2 = -dot(origin, ray) + sqrt(factor);
        return vec2(t1, t2);
    }
    return vec2(MaxV, -MaxV);
}

// get the light 
void getLight(out vec3 pos, out vec3 col){
   pos = 5. * vec3(cos(iTime), 0.5, abs(sin(iTime)));
   col = vec3(1.);
}


// Attributes For Materials 
struct MaterialAttr{
    vec3 col_a; // ambient
    vec3 col_d; // diffuse
    vec3 col_s; // spec
    float roughness;
};

// Fresnel Term
// Schlick Approximation
// norm could be the normal of the surface or the half-way normal
vec3 Fschlick(vec3 col0, vec3 l_dir, vec3 norm){
    return col0 + (1. - col0) * pow(1. - dot(l_dir, norm), 5.);
}

// Phong Shading (PS)
vec4 PS(MaterialAttr mat, vec3 p, vec3 norm, vec3 view, vec3 l_pos, vec3 l_col){
    vec3 l_dir = normalize(l_pos - p);
    vec3 ambient = mat.col_a;
    vec3 diffuse = mat.col_d * max(0., dot(norm, l_dir)) ;
    
    vec3 v_ref = normalize(view + 2. * norm); // reflected view
    vec3 spec = mat.col_s * pow(max(0., dot(v_ref, l_dir)), mat.roughness);
    
    vec4 res = vec4((ambient + diffuse + spec) * l_col, 1.);
    return res;
}

// Blinn Phong Shading  (BPS)
vec4 BPS(MaterialAttr mat, vec3 p, vec3 norm, vec3 view, vec3 l_pos, vec3 l_col){
    vec3 l_dir = normalize(l_pos - p);
    vec3 ambient = mat.col_a;
    vec3 diffuse = mat.col_d * max(0., dot(norm, l_dir)) ;
    
    vec3 half_norm = normalize(l_dir - view); // half-way normal
    vec3 spec = mat.col_s * pow(max(0., dot(half_norm, norm)), mat.roughness);
    
    vec4 res = vec4((ambient + diffuse + spec) * l_col, 1.);
    return res; 
}

// Normalized Blinn Phong Shading (NBPS)
vec4 NBPS(MaterialAttr mat, vec3 p, vec3 norm, vec3 view, vec3 l_pos, vec3 l_col){
    vec3 l_dir = normalize(l_pos - p);
    vec3 ambient = mat.col_a;
    vec3 diffuse = mat.col_d * max(0., dot(norm, l_dir)) ;
    
    vec3 half_norm = normalize(l_dir - view); // half-way normal
    float normalized_factor = (mat.roughness + 2.) / 8.;
    vec3 spec = mat.col_s * normalized_factor *  pow(max(0., dot(half_norm, norm)), mat.roughness);
    
    vec4 res = vec4((ambient + diffuse + spec) * l_col, 1.);
    return res; 
}

// Normalized Blinn Phong Shading With Fresnel Term (NBPSF)
vec4 NBPSF(MaterialAttr mat, vec3 p, vec3 norm, vec3 view, vec3 l_pos, vec3 l_col){
    vec3 l_dir = normalize(l_pos - p);
    vec3 ambient = mat.col_a;
    vec3 diffuse = mat.col_d * max(0., dot(norm, l_dir)) ;
    
    vec3 half_norm = normalize(l_dir - view); // half-way normal
    float normalized_factor = (mat.roughness + 2.) / 8.;
    vec3 fresnel_col_spec = Fschlick(mat.col_s, l_dir, half_norm);
    vec3 spec = fresnel_col_spec * normalized_factor *  pow(max(0., dot(half_norm, norm)), mat.roughness);
    spec *= max(0., dot(l_dir, norm));
    vec4 res = vec4((ambient + diffuse + spec) * l_col, 1.);
    return res; 
}

void main( )
{
    vec2 uv = xlv_TEXCOORD0/iResolution.xy;
    
    // divide the screen into 4 parts 
    float n = 2.;
    vec2 uv_i = floor(uv * n);  // integer
    vec2 uv_f = fract(uv * n);  // fractions
    
    if(abs(uv.x - 1./n) - 0.002 < 0. || abs(uv.y - 1./n) - 0.002 < 0.){
        gl_FragColor = vec4(1.);
        return;
    }
    
    // 1. get the light info
    vec3 l_pos, l_col;
    getLight(l_pos, l_col);
    
    // 2. compute the view dir
    vec3 view = computeView(uv_f);
    
    float radius = 1.;
    // 3. compute the sphere-view intersections
    vec2 t = computeIntersection(cam, view, radius);
    if(t == vec2(MaxV, -MaxV)){
        gl_FragColor = vec4(0.);
        return;
    }
    
    // 4. the intersection
    vec3 point = cam + min(t.x, t.y) * view;
    vec3 norm = sphereNormal(point);
    
    // 5. set the material
    MaterialAttr obj = MaterialAttr(vec3(0.05),
                                         vec3(0.5),
                                         vec3(0.17),
                                         64. + 48. * sin(iTime*2.));
    
    // 6.shading 
    // 6.1. Phong Shading 
    if(uv_i.x == 0. && uv_i.y == 0.){
        vec4 color = PS(obj, point, norm, view, l_pos, l_col);
        gl_FragColor = vec4(Gramma(color.xyz), 1.);
        return;
    }
    
    // 6.2. Blinn Phong Shading 
    if(uv_i.x == 0. && uv_i.y == 1.){
        vec4 color = BPS(obj, point, norm, view, l_pos, l_col);
        gl_FragColor = vec4(Gramma(color.xyz), 1.);
        return;
    }
    
    // 6.3. Normalized Blinn Phong Shading 
    if(uv_i.x == 1. && uv_i.y == 1.){
        vec4 color = NBPS(obj, point, norm, view, l_pos, l_col);
        gl_FragColor = vec4(Gramma(color.xyz), 1.);
        return;
    }
    
    // 6.4. Normalized Blinn Phong Shading With Fresnel Term for specular color
    if(uv_i.x == 1. && uv_i.y == 0.){
        vec4 color = NBPSF(obj, point, norm, view, l_pos, l_col);
        gl_FragColor = vec4(Gramma(color.xyz), 1.);
        return;
    }
    
}
