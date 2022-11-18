const int MAX_MARCHING_STEPS = 255;
const float MIN_DIST = 0.0;
const float MAX_DIST = 100.0;
const float PRECISION = 0.001;

float sdSphere(vec3 p, float r )
{
  return length(p) - r;
}

struct Material {
  vec3 ambientColor; // k_a * i_a
  vec3 diffuseColor; // k_d * i_d
  vec3 specularColor; // k_s * i_s
  float alpha; // shininess
};

struct Surface {
  int id; // id of object
  float sd; // signed distance
  Material mat;
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
  vec3 aCol = vec3(1. + 0.7*mod(floor(p.x) + floor(p.z), 2.0)) * 0.3;
  vec3 dCol = vec3(0.3);
  vec3 sCol = vec3(0);
  float a = 1.;

  return Material(aCol, dCol, sCol, a);
}

Surface opUnion(Surface obj1, Surface obj2) {
  if (obj2.sd < obj1.sd) return obj2;
  return obj1;
}

Surface scene(vec3 p) {
  Surface sFloor = Surface(1, p.y + 1., checkerboard(p));
  Surface sSphereGold = Surface(2, sdSphere(p - vec3(-2, 0, 0), 1.), gold());
  Surface sSphereSilver = Surface(3, sdSphere(p - vec3(2, 0, 0), 1.), silver());
  
  Surface co = opUnion(sFloor, sSphereGold); // closest object
  co = opUnion(co, sSphereSilver);
  return co;
}

Surface rayMarch(vec3 ro, vec3 rd) {
  float depth = MIN_DIST;
  Surface co;

  for (int i = 0; i < MAX_MARCHING_STEPS; i++) {
    vec3 p = ro + depth * rd;
    co = scene(p);
    depth += co.sd;
    if (co.sd < PRECISION || depth > MAX_DIST) break;
  }
  
  co.sd = depth;

  return co;
}

vec3 calcNormal(vec3 p) {
    vec2 e = vec2(1.0, -1.0) * 0.0005;
    return normalize(
      e.xyy * scene(p + e.xyy).sd +
      e.yyx * scene(p + e.yyx).sd +
      e.yxy * scene(p + e.yxy).sd +
      e.xxx * scene(p + e.xxx).sd);
}

mat3 camera(vec3 cameraPos, vec3 lookAtPoint) {
	vec3 cd = normalize(lookAtPoint - cameraPos); // camera direction
	vec3 cr = normalize(cross(vec3(0, 1, 0), cd)); // camera right
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

void main( )
{
  vec2 fragCoord=xlv_TEXCOORD0;
  vec2 uv = (fragCoord-.5*iResolution.xy)/iResolution.y;
  vec3 backgroundColor = mix(vec3(1, .341, .2), vec3(0, 1, 1), uv.y) * 1.6;
  vec3 col = vec3(0);

  vec3 lp = vec3(0); // lookat point (aka camera target)
  vec3 ro = vec3(0, 0, 5);

  vec3 rd = camera(ro, lp) * normalize(vec3(uv, -1)); // ray direction

  Surface co = rayMarch(ro, rd); // closest object
  
  if (co.sd > MAX_DIST) {
    col = backgroundColor;
  } else {
      vec3 p = ro + rd * co.sd; // point on surface found by ray marching
      vec3 normal = calcNormal(p); // surface normal

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
      col += lightIntensity2 * phong(lightDirection2, normal , rd, co.mat);
  }

  gl_FragColor = vec4(col, 1.0);
}

