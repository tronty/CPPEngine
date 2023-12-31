#if 0
#if GL_ES
  precision mediump float;
#endif

uniform vec2 resolution;
//uniform float time;
#endif

float sdRect(vec2 uv, vec4 rect, float r) {
  return length(max(abs(uv - rect.xy) - rect.zw, 0.0)) - r;
}

float segm(vec2 uv, int n1, int n2, int n3, int n4, int n5, int n6, int n7) {
  float res = 1.0;

  if (n1 == 1) res = min(res, sdRect(uv, vec4(0.0, 0.4, 0.125, 0.025), 0.01));
  if (n2 == 1) res = min(res, sdRect(uv, vec4(-0.2, 0.2, 0.025, 0.125), 0.01));
  if (n3 == 1) res = min(res, sdRect(uv, vec4(0.2, 0.2, 0.025, 0.125), 0.01));
  if (n4 == 1) res = min(res, sdRect(uv, vec4(0.0, 0.0, 0.125, 0.025), 0.01));
  if (n5 == 1) res = min(res, sdRect(uv, vec4(-0.2, -0.2, 0.025, 0.125), 0.01));
  if (n6 == 1) res = min(res, sdRect(uv, vec4(0.2, -0.2, 0.025, 0.125), 0.01));
  if (n7 == 1) res = min(res, sdRect(uv, vec4(0.0, -0.4, 0.125, 0.025), 0.01));

  return res;
}

float dots(vec2 uv) {
  float res = 1.0;

  res = min(res, sdRect(uv, vec4(0., 0.2, 0.025, 0.025), 0.01));
  res = min(res, sdRect(uv, vec4(0., -0.2, 0.025, 0.025), 0.01));

  return res;
}

float digit(vec2 uv, int d) {
  float res = 1.0;

  if (d == 0) res = min(res, segm(uv, 1, 1, 1, 0, 1, 1, 1));
  if (d == 1) res = min(res, segm(uv, 0, 0, 1, 0, 0, 1, 0));
  if (d == 2) res = min(res, segm(uv, 1, 0, 1, 1, 1, 0, 1));
  if (d == 3) res = min(res, segm(uv, 1, 0, 1, 1, 0, 1, 1));
  if (d == 4) res = min(res, segm(uv, 0, 1, 1, 1, 0, 1, 0));
  if (d == 5) res = min(res, segm(uv, 1, 1, 0, 1, 0, 1, 1));
  if (d == 6) res = min(res, segm(uv, 1, 1, 0, 1, 1, 1, 1));
  if (d == 7) res = min(res, segm(uv, 1, 0, 1, 0, 0, 1, 0));
  if (d == 8) res = min(res, segm(uv, 1, 1, 1, 1, 1, 1, 1));
  if (d == 9) res = min(res, segm(uv, 1, 1, 1, 1, 0, 1, 1));

  return res;
}

void main() {
  vec2 uv = (2.0 * xlv_TEXCOORD0.xy - resolution.xy) / resolution.y;
  //vec2 uv = ( xlv_TEXCOORD0.xy /resolution.xy ) * vec2(2,2);
  vec3 col = vec3(0.0);

  vec3 lowColor = vec3(0.004, 0.04, 0.02);
  vec3 highColor = vec3(0.1, 1.0, 0.5);
  vec3 dotsColor = int(mod(time, 2.)) == 0 ? highColor : lowColor;

  float size = 1.5;
  float step = 3.0 / resolution.y;

  float t = time * 1.6666667;

  float seconds = mod(t, 100.);
  float minutes = mod(t, 10000.) / 100.;
  float hours = mod(t, 10000. * 1.6666667) / 10000.;
  float days = t / (10000. * 1.6666667);

  int sUni = int(mod(seconds, 10.));
  int sTen = int(seconds / 10.);
  int mUni = int(mod(minutes, 10.));
  int mTen = int(minutes / 10.);
  int hUni = int(mod(hours, 10.));
  int hTen = int(hours / 10.);
  int dUni = int(mod(days, 10.));
  int dTen = int(days / 10.);

  // seconds
  col = mix(lowColor, col,
            smoothstep(0.0, step, digit(size * uv - vec2(2.45, 0), 8)));
  col = mix(highColor, col,
            smoothstep(0.0, step, digit(size * uv - vec2(2.45, 0), sUni)));
  col = mix(
      lowColor, col,
      smoothstep(0.0, 3. / resolution.y, digit(size * uv - vec2(1.75, 0), 8)));
  col = mix(highColor, col,
            smoothstep(0.0, step, digit(size * uv - vec2(1.75, 0), sTen)));
  // dots
  col = mix(dotsColor, col,
            smoothstep(0.0, step, dots(size * uv - vec2(1.4, 0))));
  // minutes
  col = mix(lowColor, col,
            smoothstep(0.0, step, digit(size * uv - vec2(1.05, 0), 8)));
  col = mix(highColor, col,
            smoothstep(0.0, step, digit(size * uv - vec2(1.05, 0), mUni)));
  col = mix(lowColor, col,
            smoothstep(0.0, step, digit(size * uv - vec2(.35, 0), 8)));
  col = mix(highColor, col,
            smoothstep(0.0, step, digit(size * uv - vec2(.35, 0), mTen)));
  // dots
  col = mix(dotsColor, col,
            smoothstep(0.0, step, dots(size * uv - vec2(0.0, 0))));
  // hours
  col = mix(lowColor, col,
            smoothstep(0.0, step, digit(size * uv - vec2(-0.35, 0), 8)));
  col = mix(highColor, col,
            smoothstep(0.0, step, digit(size * uv - vec2(-0.35, 0), hUni)));
  col = mix(lowColor, col,
            smoothstep(0.0, step, digit(size * uv - vec2(-1.05, 0), 8)));
  col = mix(highColor, col,
            smoothstep(0.0, step, digit(size * uv - vec2(-1.05, 0), hTen)));
  // dots
  col = mix(dotsColor, col,
            smoothstep(0.0, step, dots(size * uv - vec2(-1.4, 0))));
  // days
  col = mix(lowColor, col,
            smoothstep(0.0, step, digit(size * uv - vec2(-1.75, 0), 8)));
  col = mix(highColor, col,
            smoothstep(0.0, step, digit(size * uv - vec2(-1.75, 0), dUni)));
  col = mix(lowColor, col,
            smoothstep(0.0, step, digit(size * uv - vec2(-2.45, 0), 8)));
  col = mix(highColor, col,
            smoothstep(0.0, step, digit(size * uv - vec2(-2.45, 0), dTen)));

  gl_FragColor = vec4(col, 1.0);
}

