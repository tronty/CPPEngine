#if 0
#extension GL_OES_standard_derivatives : enable

precision mediump float;

uniform vec2 resolution;
//uniform float time;
#endif

float gridScale = 13.;

vec3 ramp(float x) {
  if (x < 1.) {
    return vec3(1.,1.,0.);
  } else if (x < 2.) {
    return vec3(0.,1.,1.);
  } else if (x < 3.) {
    return vec3(0.,.75,0.);
  } else if (x < 4.) {
    return vec3(1.,0.,1.);
  } else if (x < 5.) {
    return vec3(1.,0.,0.);
  } else if (x < 6.) {
    return vec3(0.,0.,1.);
  }
  return vec3(0.,0.,0.);
}

vec2 char_size = vec2(6, 8);

vec2 c_s = vec2(0x72281c, 0x0a2700);
vec2 c_v = vec2(0x8a2894, 0x508200);
vec2 c_e = vec2(0xfa083c, 0x820f80);
vec2 c_r = vec2(0xf228bc, 0x8a2880);
vec2 c_i = vec2(0x708208, 0x208700);
vec2 c_g = vec2(0x72282c, 0x8a2700);
vec2 c_t = vec2(0xf88208, 0x208200);
vec2 c_2 = vec2(0x72208c, 0x420f80);

float ch(vec2 ch, vec2 uv) {
	uv = floor(uv);
	vec2 b = vec2((char_size.x - uv.x - 1.0) + uv.y * char_size.x) - vec2(24,0);
	vec2 p = mod(floor(ch * exp2(-clamp(b,-1.0, 25.0))), 2.0);
	float o = dot(p,vec2(1)) * float(all(bvec4(greaterThanEqual(uv,vec2(0)), lessThan(uv,char_size))));
	return o;
}

float text(vec2 uv) {
  vec2 c = mod(floor(uv/char_size), 8.);
  vec2 l = mod(uv,char_size);
  float pix = 0.;
  if (c.x == 0.) pix = ch(c_s, l);
  if (c.x == 1.) pix = ch(c_v, l);
  if (c.x == 2.) pix = ch(c_e, l);
  if (c.x == 3.) pix = ch(c_r, l);
  if (c.x == 4.) pix = ch(c_i, l);
  if (c.x == 5.) pix = ch(c_g, l);
  if (c.x == 6.) pix = ch(c_e, l);
  return pix;
}

float text2(vec2 uv) {
  vec2 c = mod(floor(uv/char_size), 8.);
  vec2 l = mod(uv,char_size);
  float pix = 0.;
  if (c.x == 0.) pix = ch(c_t, l);
  if (c.x == 1.) pix = ch(c_v, l);
  if (c.x == 2.) pix = ch(c_2, l);
  return pix;
}

vec3 tv(vec2 uv) {
  vec2 cell = (uv*gridScale) + vec2(.5);
  vec2 grid = 1. - smoothstep(abs(fract(cell)-.5), vec2(.5), vec2(.48));
  vec2 c = floor(cell);


  float circle = step(length(uv * gridScale), 6.);
  vec3 cg = vec3(max(max(grid.x, grid.y), .5));

  vec3 inside = vec3(1.);
  if (abs(c.x) > 8. || abs(c.y) > 6.) {
     cg = vec3(mod(floor(c.y+c.x+1.), 2.));
  } else if (c.y == 5. && (cell.x >= -1.5 && cell.x <= 2.5)) {
     inside = vec3(text2((cell - vec2(-.5, 5.)) * 8.));
  }else if (c.y == 4. && (cell.x < -2.5 || cell.x > 3.5)) {
     inside = vec3(0.);
  }else if (c.y == 3.) {
     inside = vec3(mod(floor(cell.x*1.4+1.2), 2.) * .8);
  }else if (c.y == 0. || (c.x == 0. && abs(c.y) <= 1.)) {
    vec2 g = smoothstep(fract(cell+vec2(.5,.5)), vec2(.05), vec2(1.));
    g.y *= step(abs(c.y), .5);
    inside = vec3(max(g.x, g.y));
  } else if ((c.y == 2. || c.y == 1.) && (c.x >= -6. && c.x <= 6.)) {
     inside = ramp(floor((cell.x-.5)/2.)+3.);
  } else if (c.y == -3.) {
     inside = vec3((c.x+4.)/8.);
  } else if (c.x == 0. && (c.y == -5. || c.y == -6.)) {
     inside = vec3(1.,0.,0.);
  } else if (c.y >= -2. && c.y < 0.) {
    float o = -6.;
    if (cell.x > -4. && cell.x < 6.) {
      float f = 6.28*exp2(floor((c.x-o)*.5));
      inside = vec3(sin((cell.x - o)*f)*.5 + .5);
    } else {
      inside = vec3(0.);
    }
  } else if (c.y == -4. && (c.x >= -2. && c.x <= 2.)) {
     inside = vec3(text((cell - vec2(-2., -4.)) * 8.));
  } else if (c.y < -4.) {
     inside = vec3(1.,1.,0.);
  }

  if (abs(c.x) == 7. && abs(c.y) <= 5.) {
    if (c.x < 0.) {
      cg = mix(vec3(0.3,0.6,.5), vec3(.6,.3,.4), step(c.y, 0.));
    } else {
      cg = mix(vec3(0.5,0.6,.3), vec3(.45,.4,.7), step(c.y, 0.));
    }
  } else if (abs(c.x) == 6. && abs(c.y) <= 5. && abs(c.y) >= 4.) {
     cg = mix(vec3(0.4,0.5,.9), vec3(.6,.5,.25), step(c.y, 0.));
  }

  return mix(cg, inside, circle);
}


float stripe(float y) {
  return (sin(y)*.125 +.875);
}

void main() {
  vec2 pixel = vec2(1.) / max(resolution.x, resolution.y);
  vec2 uv = (xlv_TEXCOORD0.xy * 2. - resolution.xy) * pixel;

  vec3 col = tv(uv);

  col *= stripe(xlv_TEXCOORD0.y*3.1415*.5);
  col *= (sin(xlv_TEXCOORD0.y*.0095+time*17.)*.05+.95);

  gl_FragColor = vec4(col, 1.0);
}

