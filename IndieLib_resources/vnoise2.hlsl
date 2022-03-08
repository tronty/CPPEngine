/*
  Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
 //
  // GLSL textureless classic 3D noise "cnoise",
  // with an RSL-style periodic variant "pnoise".
  // Author:  Stefan Gustavson (stefan.gustavson@liu.se)
  // Version: 2011-10-11
  //
  // Many thanks to Ian McEwan of Ashima Arts for the
  // ideas for permutation and gradient selection.
  //
  // Copyright (c) 2011 Stefan Gustavson. All rights reserved.
  // Distributed under the MIT license. See LICENSE file.
  // https://github.com/ashima/webgl-noise
  //

  float3 mod289(float3 x)
  {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
  }

  float4 mod289(float4 x)
  {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
  }

  float4 permute(float4 x)
  {
    return mod289(((x*34.0)+1.0)*x);
  }

  float4 taylorInvSqrt(float4 r)
  {
    return 1.79284291400159 - 0.85373472095314 * r;
  }

  float3 fade(float3 t) {
    return t*t*t*(t*(t*6.0-15.0)+10.0);
  }

  // Classic Perlin noise
  float cnoise(float3 P)
  {
    float3 Pi0 = floor(P); // Integer part for indexing
    float3 Pi1 = Pi0 + float3(1.0); // Integer part + 1
    Pi0 = mod289(Pi0);
    Pi1 = mod289(Pi1);
    float3 Pf0 = fract(P); // Fractional part for interpolation
    float3 Pf1 = Pf0 - float3(1.0); // Fractional part - 1.0
    float4 ix = float4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
    float4 iy = float4(Pi0.yy, Pi1.yy);
    float4 iz0 = Pi0.zzzz;
    float4 iz1 = Pi1.zzzz;

    float4 ixy = permute(permute(ix) + iy);
    float4 ixy0 = permute(ixy + iz0);
    float4 ixy1 = permute(ixy + iz1);

    float4 gx0 = ixy0 * (1.0 / 7.0);
    float4 gy0 = fract(floor(gx0) * (1.0 / 7.0)) - 0.5;
    gx0 = fract(gx0);
    float4 gz0 = float4(0.5) - abs(gx0) - abs(gy0);
    float4 sz0 = step(gz0, float4(0.0));
    gx0 -= sz0 * (step(0.0, gx0) - 0.5);
    gy0 -= sz0 * (step(0.0, gy0) - 0.5);

    float4 gx1 = ixy1 * (1.0 / 7.0);
    float4 gy1 = fract(floor(gx1) * (1.0 / 7.0)) - 0.5;
    gx1 = fract(gx1);
    float4 gz1 = float4(0.5) - abs(gx1) - abs(gy1);
    float4 sz1 = step(gz1, float4(0.0));
    gx1 -= sz1 * (step(0.0, gx1) - 0.5);
    gy1 -= sz1 * (step(0.0, gy1) - 0.5);

    float3 g000 = float3(gx0.x,gy0.x,gz0.x);
    float3 g100 = float3(gx0.y,gy0.y,gz0.y);
    float3 g010 = float3(gx0.z,gy0.z,gz0.z);
    float3 g110 = float3(gx0.w,gy0.w,gz0.w);
    float3 g001 = float3(gx1.x,gy1.x,gz1.x);
    float3 g101 = float3(gx1.y,gy1.y,gz1.y);
    float3 g011 = float3(gx1.z,gy1.z,gz1.z);
    float3 g111 = float3(gx1.w,gy1.w,gz1.w);

    float4 norm0 = taylorInvSqrt(float4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
    g000 *= norm0.x;
    g010 *= norm0.y;
    g100 *= norm0.z;
    g110 *= norm0.w;
    float4 norm1 = taylorInvSqrt(float4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
    g001 *= norm1.x;
    g011 *= norm1.y;
    g101 *= norm1.z;
    g111 *= norm1.w;

    float n000 = dot(g000, Pf0);
    float n100 = dot(g100, float3(Pf1.x, Pf0.yz));
    float n010 = dot(g010, float3(Pf0.x, Pf1.y, Pf0.z));
    float n110 = dot(g110, float3(Pf1.xy, Pf0.z));
    float n001 = dot(g001, float3(Pf0.xy, Pf1.z));
    float n101 = dot(g101, float3(Pf1.x, Pf0.y, Pf1.z));
    float n011 = dot(g011, float3(Pf0.x, Pf1.yz));
    float n111 = dot(g111, Pf1);

    float3 fade_xyz = fade(Pf0);
    float4 n_z = mix(float4(n000, n100, n010, n110), float4(n001, n101, n011, n111), fade_xyz.z);
    float2 n_yz = mix(n_z.xy, n_z.zw, fade_xyz.y);
    float n_xyz = mix(n_yz.x, n_yz.y, fade_xyz.x);
    return 2.2 * n_xyz;
  }

  // Classic Perlin noise, periodic variant
  float pnoise_(float3 P, float3 rep)
  {
    float3 Pi0 = mod289(floor(P));//, rep); // Integer part, modulo period
    float3 Pi1 = mod289(Pi0 + float3(1.0));//, rep); // Integer part + 1, mod period
    Pi0 = mod289(Pi0);
    Pi1 = mod289(Pi1);
    float3 Pf0 = fract(P); // Fractional part for interpolation
    float3 Pf1 = Pf0 - float3(1.0); // Fractional part - 1.0
    float4 ix = float4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
    float4 iy = float4(Pi0.yy, Pi1.yy);
    float4 iz0 = Pi0.zzzz;
    float4 iz1 = Pi1.zzzz;

    float4 ixy = permute(permute(ix) + iy);
    float4 ixy0 = permute(ixy + iz0);
    float4 ixy1 = permute(ixy + iz1);

    float4 gx0 = ixy0 * (1.0 / 7.0);
    float4 gy0 = fract(floor(gx0) * (1.0 / 7.0)) - 0.5;
    gx0 = fract(gx0);
    float4 gz0 = float4(0.5) - abs(gx0) - abs(gy0);
    float4 sz0 = step(gz0, float4(0.0));
    gx0 -= sz0 * (step(0.0, gx0) - 0.5);
    gy0 -= sz0 * (step(0.0, gy0) - 0.5);

    float4 gx1 = ixy1 * (1.0 / 7.0);
    float4 gy1 = fract(floor(gx1) * (1.0 / 7.0)) - 0.5;
    gx1 = fract(gx1);
    float4 gz1 = float4(0.5) - abs(gx1) - abs(gy1);
    float4 sz1 = step(gz1, float4(0.0));
    gx1 -= sz1 * (step(0.0, gx1) - 0.5);
    gy1 -= sz1 * (step(0.0, gy1) - 0.5);

    float3 g000 = float3(gx0.x,gy0.x,gz0.x);
    float3 g100 = float3(gx0.y,gy0.y,gz0.y);
    float3 g010 = float3(gx0.z,gy0.z,gz0.z);
    float3 g110 = float3(gx0.w,gy0.w,gz0.w);
    float3 g001 = float3(gx1.x,gy1.x,gz1.x);
    float3 g101 = float3(gx1.y,gy1.y,gz1.y);
    float3 g011 = float3(gx1.z,gy1.z,gz1.z);
    float3 g111 = float3(gx1.w,gy1.w,gz1.w);

    float4 norm0 = taylorInvSqrt(float4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
    g000 *= norm0.x;
    g010 *= norm0.y;
    g100 *= norm0.z;
    g110 *= norm0.w;
    float4 norm1 = taylorInvSqrt(float4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
    g001 *= norm1.x;
    g011 *= norm1.y;
    g101 *= norm1.z;
    g111 *= norm1.w;

    float n000 = dot(g000, Pf0);
    float n100 = dot(g100, float3(Pf1.x, Pf0.yz));
    float n010 = dot(g010, float3(Pf0.x, Pf1.y, Pf0.z));
    float n110 = dot(g110, float3(Pf1.xy, Pf0.z));
    float n001 = dot(g001, float3(Pf0.xy, Pf1.z));
    float n101 = dot(g101, float3(Pf1.x, Pf0.y, Pf1.z));
    float n011 = dot(g011, float3(Pf0.x, Pf1.yz));
    float n111 = dot(g111, Pf1);

    float3 fade_xyz = fade(Pf0);
    float4 n_z = mix(float4(n000, n100, n010, n110), float4(n001, n101, n011, n111), fade_xyz.z);
    float2 n_yz = mix(n_z.xy, n_z.zw, fade_xyz.y);
    float n_xyz = mix(n_yz.x, n_yz.y, fade_xyz.x);
    return 1.5 * n_xyz;
  }
struct VsIn {
	float3 Vertex: POSITION;
	float3 Normal: NORMAL;
	float3 BiNormal: BINORMAL;
        float3 Tangent: TANGENT;
        float3 Color: TEXCOORD0;
        float2 Tex: TEXCOORD1;
};
struct VsOut {
	float4 Position: POSITION;
	float2 Tex: TEXCOORD0;
	float noise_: TEXCOORD0;
};
[Vertex shader]
float turbulence( float3 p ) {

  float w = 100.0;
  float t = -.5;

  for (float f = 1.0 ; f <= 10.0 ; f++ ){
    float power = pow( 2.0, f );
    t += abs( pnoise_( float3( power * p ), float3( 10.0, 10.0, 10.0 ) ) / power );
  }

  return t;

}

ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;
VsOut main(VsIn In)
{
	VsOut Out=(VsOut)0;
  Out.Tex = In.Tex;

  // get a turbulent 3d noise using the normal, normal to high freq
  float noise_ = 10.0 *  -.10 * turbulence( .5 * In.Normal );
  // get a 3d noise using the position, low frequency
  float b = 5.0 * pnoise_( 0.05 * In.Vertex, float3( 100.0 ) );
  // compose both noises
  float displacement = - 10. * noise_ + b;

  // move the position along the normal and transform it
  float3 newPosition = In.Vertex + In.Normal * displacement;

    	Out.Position = mul(worldViewProj, float4(newPosition, 1.0));
	return Out;
}

[Fragment shader]
sampler2D noise_;
sampler2D flame;
sampler1D fire_gradient3;
sampler1D FireGrade;
sampler1D FirePalette;
float4 main(VsOut IN): COLOR {
	//return tex2D(noise_, IN.Tex)+tex1D(FireGrade, 0.5);
	//return tex2D(noise_, IN.Tex)+tex1D(fire_gradient3, 0.5);
	return tex2D(noise_, IN.Tex)+tex2D(noise_, IN.Tex+float2(0.5, 0.5))+tex1D(FirePalette, 0.5);
}
#if 0
varying float2 vUv;

void main() {

  vUv = uv;
  gl_Position = projectionMatrix * modelViewMatrix * float4( position, 1.0 );

}

varying float2 vUv;

void main() {

  // colour is RGBA: u, v, 0, 1
  gl_FragColor = float4( float3( vUv, 0. ), 1. );

}

varying float2 vUv;
varying float noise;

float turbulence( float3 p ) {

  float w = 100.0;
  float t = -.5;

  for (float f = 1.0 ; f <= 10.0 ; f++ ){
    float power = pow( 2.0, f );
    t += abs( pnoise( float3( power * p ), float3( 10.0, 10.0, 10.0 ) ) / power );
  }

  return t;

}

void main() {

  vUv = uv;

  // get a turbulent 3d noise using the normal, normal to high freq
  noise = 10.0 *  -.10 * turbulence( .5 * normal );
  // get a 3d noise using the position, low frequency
  float b = 5.0 * pnoise( 0.05 * position, float3( 100.0 ) );
  // compose both noises
  float displacement = - 10. * noise + b;

  // move the position along the normal and transform it
  float3 newPosition = position + normal * displacement;
  gl_Position = projectionMatrix * modelViewMatrix * float4( newPosition, 1.0 );

}

varying float2 vUv;
varying float noise;

void main() {

  // compose the colour using the UV coordinate
  // and modulate it with the noise like ambient occlusion
  float3 color = float3( vUv * ( 1. - 2. * noise ), 0.0 );
  gl_FragColor = float4( color.rgb, 1.0 );

}

varying float2 vUv;
varying float noise;
uniform float time;

float turbulence( float3 p ) {

  float w = 100.0;
  float t = -.5;

  for (float f = 1.0 ; f <= 10.0 ; f++ ){
    float power = pow( 2.0, f );
    t += abs( pnoise( float3( power * p ), float3( 10.0, 10.0, 10.0 ) ) / power );
  }

  return t;

}

void main() {

vUv = uv;

  // add time to the noise parameters so it's animated
  noise = 10.0 *  -.10 * turbulence( .5 * normal + time );
  float b = 5.0 * pnoise( 0.05 * position + float3( 2.0 * time ), float3( 100.0 ) );
  float displacement = - noise + b;

  float3 newPosition = position + normal * displacement;
  gl_Position = projectionMatrix * modelViewMatrix * float4( newPosition, 1.0 );

}

varying float2 vUv;
varying float noise;
uniform sampler2D tExplosion;

float random( float3 scale, float seed ){
  return fract( sin( dot( gl_FragCoord.xyz + seed, scale ) ) * 43758.5453 + seed ) ;
}

void main() {

  // get a random offset
  float r = .01 * random( float3( 12.9898, 78.233, 151.7182 ), 0.0 );
  // lookup vertically in the texture, using noise and offset
  // to get the right RGB colour
  float2 tPos = float2( 0, 1.3 * noise + r );
  float4 color = texture2D( tExplosion, tPos );

  gl_FragColor = float4( color.rgb, 1.0 );

}
#endif

