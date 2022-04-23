// Inspired and referenced from
// https://wallisc.github.io/rendering/2020/05/02/Volumetric-Rendering-Part-1.html

const int MAX_MARCHING_STEPS = 255;
const float MIN_DIST = 0.0;
const float MAX_DIST = 100.0;
const float PRECISION = 0.001;
const float PI = 3.1415926535897932384626433832795;
const mat3 m3 = mat3(0.00, 0.80, 0.60, -0.80, 0.36, -0.48, -0.60, -0.48, 0.64);

struct Material {
  vec3 ambientColor; // k_a * i_a
  vec3 diffuseColor; // k_d * i_d
  vec3 specularColor; // k_s * i_s
  float alpha; // shininess
};

struct Surface {
  int id; // id of object
  float sd; // signed distance value from SDF
  Material mat; // material of object
};

Material gold() {
  vec3 aCol = 0.5 * vec3(0.7, 0.5, 0);
  vec3 dCol = 0.6 * vec3(0.7, 0.7, 0);
  vec3 sCol = 0.6 * vec3(1, 1, 1);
  float a = 5.;

  return Material(aCol, dCol, sCol, a);
}

Material silver() {
  vec3 aCol = 0.4 * vec3(0.8);
  vec3 dCol = 0.5 * vec3(0.7);
  vec3 sCol = 0.6 * vec3(1, 1, 1);
  float a = 5.;

  return Material(aCol, dCol, sCol, a);
}

Material checkerboard(vec3 p) {
  vec3 aCol = vec3(1. + 0.7 * mod(floor(p.x) + floor(p.z), 2.0)) * 0.3;
  vec3 dCol = vec3(0.3);
  vec3 sCol = vec3(0);
  float a = 1.;

  return Material(aCol, dCol, sCol, a);
}

mat3 identity() {
  return mat3(vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1));
}

mat2 rotate2d(float theta) {
  float s = sin(theta), c = cos(theta);
  return mat2(c, -s, s, c);
}

Surface sdFloor(vec3 p, float offset, Material mat) {
  float d = p.y - offset;
  return Surface(1, d, mat);
}

Surface sdSphere(vec3 p, float r, vec3 offset, Material mat, mat3 transform, vec3 transformOrigin) {
  p = (p - offset) * transform - transformOrigin;
  return Surface(3, length(p) - r, mat);
}

Surface opUnion(Surface obj1, Surface obj2) {
  if(obj2.sd < obj1.sd)
    return obj2; // The sd component of the struct holds the "signed distance" value
  return obj1;
}

Surface sdSmoothUnion(Surface obj1, Surface obj2, float k) {
  float d1 = obj1.sd;
  float d2 = obj2.sd;

  Material mat = obj1.mat;

  float h = clamp(0.5 + 0.5 * (d2 - d1) / k, 0.0, 1.0);

  return Surface(4, mix(d2, d1, h) - k * h * (1.0 - h), mat);
}

float hash1(float n) {
  return fract(n * 17.0 * fract(n * 0.3183099));
}

float noise(in vec3 x) {
  vec3 p = floor(x);
  vec3 w = fract(x);

  vec3 u = w * w * w * (w * (w * 6.0 - 15.0) + 10.0);

  float n = p.x + 317.0 * p.y + 157.0 * p.z;

  float a = hash1(n + 0.0);
  float b = hash1(n + 1.0);
  float c = hash1(n + 317.0);
  float d = hash1(n + 318.0);
  float e = hash1(n + 157.0);
  float f = hash1(n + 158.0);
  float g = hash1(n + 474.0);
  float h = hash1(n + 475.0);

  float k0 = a;
  float k1 = b - a;
  float k2 = c - a;
  float k3 = e - a;
  float k4 = a - b - c + d;
  float k5 = a - c - e + g;
  float k6 = a - b - e + f;
  float k7 = -a + b + c - d + e - f - g + h;

  return -1.0 + 2.0 * (k0 + k1 * u.x + k2 * u.y + k3 * u.z + k4 * u.x * u.y + k5 * u.y * u.z + k6 * u.z * u.x + k7 * u.x * u.y * u.z);
}

float fbm_4(in vec3 x) {
  float f = 2.0;
  float s = 0.5;
  float a = 0.0;
  float b = 0.5;
  for(int i = min(0, iFrame); i < 4; i++) {
    float n = noise(x);
    a += b * n;
    b *= s;
    x = f * m3 * x;
  }
  return a;
}

Surface sdScene(vec3 p) {
  vec3 fbmCoord = (p + 2.0 * vec3(iTime, 0.0, iTime)) / 1.5f;

  Surface floorObj = sdFloor(p, -1., checkerboard(p));
  Surface leftSphereObj = sdSphere(p, 5.6, vec3(-8.0, 2.0 + 20.0 * sin(iTime), -1), gold(), identity(), vec3(0)); // identity
  Surface midSphereObj = sdSphere(p, 5.6, vec3(8.0, 8.0 + 12.0 * cos(iTime), 3), gold(), identity(), vec3(0));
  Surface rightSphereObj = sdSphere(p, 8., vec3(5.0 * sin(iTime), 3.0, 0), gold(), identity(), vec3(0));

  Surface co = sdSmoothUnion(leftSphereObj, midSphereObj, 3.0);
  co = sdSmoothUnion(co, rightSphereObj, 3.0);
  co.sd += 7.0 * fbm_4(fbmCoord / 3.2);

  co = opUnion(co, floorObj);

  return co;
}

Surface rayMarch(vec3 ro, vec3 rd, float start, float end) {
  float depth = start;
  Surface co; // closest object

  for(int i = 0; i < MAX_MARCHING_STEPS; i++) {
    vec3 p = ro + depth * rd;
    co = sdScene(p);
    depth += co.sd;
    if(co.sd < PRECISION || depth > end)
      break;
  }

  co.sd = depth;

  return co;
}

vec3 calcNormal(in vec3 p) {
  vec2 e = vec2(1., -1.) * .0005; // epsilon
  return normalize(e.xyy * sdScene(p + e.xyy).sd +
    e.yyx * sdScene(p + e.yyx).sd +
    e.yxy * sdScene(p + e.yxy).sd +
    e.xxx * sdScene(p + e.xxx).sd);
}

mat3 camera(vec3 cameraPos, vec3 lookAtPoint) {
  vec3 up = vec3(0, 1, 0);
  vec3 cd = normalize(lookAtPoint - cameraPos); // camera direction
  vec3 cr = normalize(cross(up, cd)); // camera right
  vec3 cu = normalize(cross(cd, cr)); // camera up

  return mat3(-cr, cu, -cd); 
}

vec3 phong(vec3 lightDir, vec3 normal, vec3 rd, Material mat) {
  // ambient
  vec3 ambient = mat.ambientColor;

  // diffuse
  float dotLN = clamp(dot(lightDir, normal), 0., 1.);
  vec3 diffuse = mat.diffuseColor * dotLN;

  // specular
  float dotRV = clamp(dot(reflect(lightDir, normal), -rd), 0., 1.);
  vec3 specular = mat.specularColor * pow(dotRV, mat.alpha);

  return ambient + diffuse + specular;
}

void main( ) {
  vec2 uv = (xlv_TEXCOORD0 - .5 * iResolution.xy) / iResolution.y;

  vec2 mouseUV = iMouse.xy / iResolution.xy; // Range: <0, 1>

  vec3 backgroundColor = vec3(.835, 1, 1);

  vec3 col = vec3(0);

  vec3 lp = vec3(0, .5, -4); // lookat point - middle box

  vec3 ro = vec3(0, 5, 0);

  float cameraRadius = 3.;
  ro.yz = ro.yz * cameraRadius * rotate2d(mix(PI / 2., 0., mouseUV.y)); // remap mouseUV.y to <pi/2, 0> range
  ro.xz = ro.xz * rotate2d(mix(-PI, PI, mouseUV.x)) + vec2(lp.x, lp.z); // remap mouseUV.x to <-pi, pi> range

  vec3 rd = camera(ro, lp) * normalize(vec3(uv, -1)); // ray direction

  Surface co = rayMarch(ro, rd, MIN_DIST, MAX_DIST); // closest object

  if(co.sd > MAX_DIST) {
    col = backgroundColor; // ray didn't hit anything
  } else {
    vec3 p = ro + rd * co.sd; // point on sphere or floor we discovered from ray marching
    vec3 normal = calcNormal(p);

      // light #1
    vec3 lightPosition1 = vec3(-8, -6, -5);
    vec3 lightDirection1 = normalize(lightPosition1 - p);
    float lightIntensity1 = 0.9;

      // light #2
    vec3 lightPosition2 = vec3(1, 1, 1);
    vec3 lightDirection2 = normalize(lightPosition2 - p);
    float lightIntensity2 = 0.5;

      // final color of object
    col = lightIntensity1 * phong(lightDirection1, normal, rd, co.mat);
    col += lightIntensity2 * phong(lightDirection2, normal, rd, co.mat);
  }

  // Output to screen
  gl_FragColor = vec4(col, 1.);
}

