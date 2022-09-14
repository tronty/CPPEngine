// KIFS City by leon denise 2021/11/22
// example of how to go to fractal city
// which is a cubic kaleidoscopic shape carving volume
// using code from Inigo Quilez, LJ, Fabrice Neyret and many others
// licensed under hippie love conspiracy

mat2 rot (float a) { return mat2(cos(a),-sin(a),sin(a),cos(a)); }

float map (vec3 p)
{
  float scene = 100.;
  float t = floor(iTime/5.);
  float falloff = 1.0;
  for (float index = 0.; index < 8.; ++index)
  {
    p.xz *= rot(t/falloff);
    p = abs(p)-0.5*falloff;
    scene = min(scene, max(p.x, max(p.y, p.z)));
    falloff /= 1.8;
  }
  return -scene;
}

void main( )
{
  vec2 uv = 2.*(xlv_TEXCOORD0.xy-0.5*iResolution.xy)/iResolution.y;
  vec3 pos = vec3(0);
  vec3 ray = normalize(vec3(uv, 1));
  float shade = 0.;
  const float count = 30.;
  for (float index = count; index > 0.; --index)
  {
    float dist = map(pos);
    if (dist < 0.001)
    {
      shade = index/count;
      break;
    }
    pos += ray * dist;
  }
  gl_FragColor = vec4(shade);
}

