// https://www.clicktorelease.com/blog/vertex-displacement-noise-3d-webgl-glsl-three-js/
varying vec2 vUv;

void main() {

  vUv = uv;
  gl_Position = projectionMatrix * modelViewMatrix * vec4( position, 1.0 );

}

varying vec2 vUv;

void main() {

  // colour is RGBA: u, v, 0, 1
  gl_FragColor = vec4( vec3( vUv, 0. ), 1. );

}

varying vec2 vUv;
varying float noise;

float turbulence( vec3 p ) {

  float w = 100.0;
  float t = -.5;

  for (float f = 1.0 ; f <= 10.0 ; f++ ){
    float power = pow( 2.0, f );
    t += abs( pnoise( vec3( power * p ), vec3( 10.0, 10.0, 10.0 ) ) / power );
  }

  return t;

}

void main() {

  vUv = uv;

  // get a turbulent 3d noise using the normal, normal to high freq
  noise = 10.0 *  -.10 * turbulence( .5 * normal );
  // get a 3d noise using the position, low frequency
  float b = 5.0 * pnoise( 0.05 * position, vec3( 100.0 ) );
  // compose both noises
  float displacement = - 10. * noise + b;

  // move the position along the normal and transform it
  vec3 newPosition = position + normal * displacement;
  gl_Position = projectionMatrix * modelViewMatrix * vec4( newPosition, 1.0 );

}

varying vec2 vUv;
varying float noise;

void main() {

  // compose the colour using the UV coordinate
  // and modulate it with the noise like ambient occlusion
  vec3 color = vec3( vUv * ( 1. - 2. * noise ), 0.0 );
  gl_FragColor = vec4( color.rgb, 1.0 );

}

varying vec2 vUv;
varying float noise;
uniform float time;

float turbulence( vec3 p ) {

  float w = 100.0;
  float t = -.5;

  for (float f = 1.0 ; f <= 10.0 ; f++ ){
    float power = pow( 2.0, f );
    t += abs( pnoise( vec3( power * p ), vec3( 10.0, 10.0, 10.0 ) ) / power );
  }

  return t;

}

void main() {

vUv = uv;

  // add time to the noise parameters so it's animated
  noise = 10.0 *  -.10 * turbulence( .5 * normal + time );
  float b = 5.0 * pnoise( 0.05 * position + vec3( 2.0 * time ), vec3( 100.0 ) );
  float displacement = - noise + b;

  vec3 newPosition = position + normal * displacement;
  gl_Position = projectionMatrix * modelViewMatrix * vec4( newPosition, 1.0 );

}

varying vec2 vUv;
varying float noise;
uniform sampler2D tExplosion;

float random( vec3 scale, float seed ){
  return fract( sin( dot( gl_FragCoord.xyz + seed, scale ) ) * 43758.5453 + seed ) ;
}

void main() {

  // get a random offset
  float r = .01 * random( vec3( 12.9898, 78.233, 151.7182 ), 0.0 );
  // lookup vertically in the texture, using noise and offset
  // to get the right RGB colour
  vec2 tPos = vec2( 0, 1.3 * noise + r );
  vec4 color = texture2D( tExplosion, tPos );

  gl_FragColor = vec4( color.rgb, 1.0 );

}

