/*
 * Original shader from: https://www.shadertoy.com/view/Ns23Rc
 */
#if 0
#ifdef GL_ES
precision highp float;
#endif

// glslsandbox uniforms
//uniform float time;
uniform vec2 resolution;

// shadertoy emulation
#define iTime time
#define iResolution resolution
#endif

// --------[ Original ShaderToy begins here ]---------- //
////////////////////////////////////////////////////////////////////////////////
//
// hommage to "lightcrypt" by gopher/Alcatraz - At Revision 2021 I was blown
// away seeing a 256-bytes DOS demo implementing 3D raymarching with lighting
// and shadows. I always had a huge facination for this extreme form of size-
// -coding, but "lightcrypt" is on another level in my view.
//
// Since this sparked even more interest for that particular demoscene-discipline
// and I really need to get back into shader-coding, I begin my ventures into
// size-coding by getting a grip on the elements gopher probably used... only
// less dense :)
//
// See http://www.pouet.net/prod.php?which=88539
//
// Author(s):
//   Mirco "MacSlow" Müller <macslow@gmail.com>
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License version 3, as published
// by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranties of
// MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
// PURPOSE.  See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program.  If not, see <http://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////

float opSmoothSubtract (float a, float b, float k)
{
    float h = clamp (.5 - .5*(a+b)/k, .0, 1.);
    return mix (b, -a, h) + k*h*(1. - h); 
}

float cube (vec3 p, float size)
{
    vec3 cube = abs (p) - size;
    return length (max (cube, .0)) + min (max (cube.x, max (cube.y, cube.z)), .0);
}

float scene (vec3 p)
{
    float cellSize = 1.; 
    float cellSizeHalf = cellSize*.5;
    float radius = .6; 

    float ground = p.y + .125;

    vec3 repreatedSpace = mod (p + cellSizeHalf, cellSize) - cellSizeHalf;
    p = repreatedSpace;

    float ball =  length (p) - radius;
    float box =  cube (p, cellSizeHalf);
    float cell = opSmoothSubtract (ball, box, .05);

    float d = min (cell, ground);

    return d;
}

vec3 camera (vec2 uv, vec3 ro, vec3 aim, float zoom)
{
    vec3 camForward = normalize (vec3 (aim - ro));
    vec3 worldUp = vec3 (.0, 1., .0);
    vec3 camRight = normalize (cross (worldUp, camForward));
    vec3 camUp = normalize (cross (camForward, camRight));
    vec3 camCenter = ro + camForward*zoom;

    return normalize (camCenter + uv.x*camRight + uv.y*camUp - ro);
}

float raymarch (vec3 ro, vec3 rd) 
{
    float t = .0; 
    float d = .0; 
    vec3 p = vec3 (.0);

    for (int iter = 0; iter < 128; ++iter) {
        p = ro + d * rd; 
        t = scene (p);
        if (abs (t) < .0002*(1. + .125*t)) break;
        d += t*.75;
    }   

    return d;
}

vec3 normal (vec3 p)
{
    vec2 e = vec2 (0., .001);

    return normalize (vec3 (scene (p + e.yxx),
                            scene (p + e.xyx),
                            scene (p + e.xxy)) - scene(p));
}

float shadow (vec3 p, vec3 n, vec3 lPos)
{
  float distanceToLight = distance (lPos, p); 
  vec3 lDir = normalize (lPos - p); 
  float distanceToObject = raymarch (p + .01 * n, lDir);
  bool isShadowed = distanceToObject < distanceToLight;

  return isShadowed ? .3 : 1.; 
}

float ao (vec3 p, vec3 n, float stepSize, int iterations, float intensity)
{
  float ao = .0; 
  float dist = .0; 

  for (int a = 1; a <= 12; ++a) {
    dist = float (a)*stepSize;
    ao += max (.0, (dist - scene (p + n*dist))/dist);
  }

  return 1. - ao*intensity;
}

vec3 shade (vec3 ro, vec3 rd, float d, vec3 n)
{
    vec3 p = ro + d*rd;
    vec3 amb = vec3 (.15);
    vec3 lPos = p + vec3 (1., .25, -3.*cos (.5*iTime));
    vec3 lPos2 = p + vec3 (-1., .25, -3.*sin (.25*iTime));
    vec3 lDir = normalize (lPos - p);
    vec3 lDir2 = normalize (lPos2 - p);
    //float lDist = distance (p, lPos);
    //float attn = 1. / (lDist*lDist);
    float diff = max (dot (n, lDir), .0);
    float diff2 = max (dot (n, lDir2), .0);
    vec3 diffMat = vec3 (.8, .7, .2);
    vec3 h = normalize (ro + lDir);
    vec3 h2 = normalize (ro + lDir2);
    float spec = pow (max (dot (h, n), .0), 40.);
    float spec2 = pow (max (dot (h2, n), .0), 40.);
    float sha = shadow (p, n, lPos);
    float sha2 = shadow (p, n, lPos2);
    float ao = ao (p, n, .01, 12, .1);

    return amb + sha*ao*(diff*diffMat + spec) +
                 sha2*ao*(diff2*diffMat + spec2);
}

mat2 r2d (float deg)
{
  float rad = radians (deg);
  float c = cos (rad);
  float s = sin (rad);

  return mat2 (vec2 (c, s), vec2 (-s, c));
}

void mainImage_( out vec4 fragColor, in vec2 fragCoord )
{
    // normalize and aspect-correct UVs
    vec2 uv = vec2(fragCoord.x / iResolution.x, fragCoord.y / iResolution.y);
    uv -= .5;
    uv /= vec2(iResolution.y / iResolution.x, 1.);

    // build view-ray and camera
    vec2 offset = vec2 (.125*cos (iTime), -.3*iTime);
    vec3 ro = vec3 (offset.x, -.025, offset.y + 2.);
    vec3 aim = vec3 (.0, .5, offset.y);
    float zoom = 1.25;
    vec3 rd = camera (uv, ro, aim, zoom);
    rd.xy *= r2d(4. + 2.*cos (1.25*iTime));

    // do raymarch, normal-calc and shading
    float d = raymarch (ro, rd);
    vec3 p = ro + d*rd;
    vec3 n = normal (p);
    vec3 color = shade (ro, rd, d, n);

    // fog
    float fog = 1. / (1. + d*d*1.);
    color *= fog;

    //colored haze
    color = mix (color, vec3 (.99, .95, .2), pow (1. - 1. / d, 4.));

    // vignette
    color *= 1. - .625*length (fragCoord.xy/iResolution.xy*2. - 1.);

    // Reinhart tone-mapping
    color = color / (1. - color);

    // Gamma-correction
    color = pow (color, vec3 (1./2.2));

    fragColor = vec4 (color, 1.);
}
// --------[ Original ShaderToy ends here ]---------- //

void main(void)
{
    mainImage_(gl_FragColor, xlv_TEXCOORD0.xy);
}
