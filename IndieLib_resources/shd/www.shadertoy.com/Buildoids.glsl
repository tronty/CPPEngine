// Shader made for the Cookie Collective live stream (2020/03/19)
// https://www.twitch.tv/videos/570598632
// Added dithering and mouse control for the Shadertoy version.

// Leon Denise aka ponk from Cookie Collective
// Made in 2020/03/19 with the work of the Shadertoy community

const float iterations = 7.;

//vec2 mouse;
float random (in vec2 st) { return fract(sin(dot(st.xy,vec2(12.9898,78.233)))*43758.5453123); }
mat2 rot (float a) { float c=cos(a),s=sin(a); return mat2(c,-s,s,c); }
vec3 lookat (vec3 eye, vec3 at, vec2 uv, float fov) {
  vec3 forward = normalize(at-eye);
  vec3 right = normalize(cross(forward, vec3(0,1,0)));
  vec3 up = normalize(cross(right, forward));
  return normalize(forward * fov + right * uv.x + up * uv.y);
}

float map (vec3 pos) {
  float scene = 1.0;
  float range = 2.0;
  float a = 1.0;
  float falloff = 1.2 + mouse.x * 0.6;
  for (float index = iterations; index > 0.; --index) {
    pos.xz *= rot(iTime*.05);
    pos = abs(pos)-range*a;
    scene = min(scene, max(pos.x,max(pos.y,pos.z)));
    a /= falloff;
  }
  scene = max(-scene,0.);
  return scene;
}

vec3 getNormal (vec3 p) {
  vec2 e = vec2(0.0001,0);
  return normalize(vec3(map(p+e.xyy)-map(p-e.xyy), map(p+e.yxy)-map(p-e.yxy), map(p+e.yyx)-map(p-e.yyx)));
}

void main( )
{
    vec2 uv = (xlv_TEXCOORD0-.5*iResolution.xy)/iResolution.y;
    vec3 eye = vec3(0,0.,-2);
    vec3 ray = lookat(eye, vec3(0.,sin(iTime*.2)*.5,0), uv, .5);
    //mouse = (iMouse.xy / iResolution.xy);
    float total = 0.0;
    float shade = 0.0;
    float dither = random(uv);
    const float count = 100.;
    for (float index = count; index > 0.; --index) {
        float dist = map(eye+ray*total);
        if (dist < 0.001) {
            shade = index/count;
            break;
        }
        dist *= .9 + 0.1 * dither;
        total += dist;
    }
    vec3 color = vec3(shade);
    vec3 normal = getNormal(eye+ray*total);
    color = vec3(.3)*clamp(dot(normal, normalize(vec3(0,1,-1))),0.,1.);
    color += vec3(.9)*pow(clamp(dot(normal, -ray),0.,1.), 8.);
    color *= shade;
    gl_FragColor = vec4(color,1);
}

