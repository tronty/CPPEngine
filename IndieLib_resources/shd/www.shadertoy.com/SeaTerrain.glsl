// https://www.shadertoy.com/view/flfczs
// Help from https://www.shadertoy.com/view/MtsSRf

const int MAX_MARCHING_STEPS = 255;
const float MIN_DIST = 0.0;
const float MAX_DIST = 100.0;
const float PRECISION = 0.001;
vec3 lightDir = normalize(vec3(-.8, .15, -.3));

#define SC (250.0)

float random(in vec2 uv)
{
    return fract(sin(dot(uv.xy, 
                         vec2(12.9898, 78.233))) * 
                 43758.5453123);
}

float noise(in vec2 uv)
{
    vec2 i = floor(uv);
    vec2 f = fract(uv);
    
    float a = random(i + vec2(0., 0.));
    float b = random(i + vec2(1., 0.));
    float c = random(i + vec2(0., 1.));
    float d = random(i + vec2(1., 1.));
    
    vec2 u = smoothstep(0., 1., f);
    
    return mix(mix(a, b, u.x), 
               mix(c, d, u.x), u.y);
}

#define OCTAVES 8
float fbm(in vec2 uv)
{
    float value = 0.;
    float amplitude = 1.;
    float freq = 0.8;
    
    for (int i = 0; i < OCTAVES; i++)
    {
        // value += noise(uv * freq) * amplitude;
        
        // From Dave_Hoskins https://www.shadertoy.com/user/Dave_Hoskins
        value += (.25-abs(noise(uv * freq)-.3) * amplitude);
        
        amplitude *= .37;
        
        freq *= 2.;
    }
    
    return value;
}

struct Surface {
  float sd; // signed distance
};

float cosNoise( in vec2 p )
{
    return 0.4*( sin(p.x) + sin(p.y) );
}

const mat2 m2 = mat2(1.6,-1.2,
                     1.2, 1.6);
float map( in vec3 pos )
{
    float h = 0.0;
    vec2 q = pos.xz*0.5;
    
    float s = 0.5;
    for( int i=0; i<6; i++ )
    {
        h += s*cosNoise( q ); 
        q = m2*q*0.9+iTime; 
        q += vec2(2.,5.);
        s *= 0.5 + 0.2*h;
    }
    h *= 2.0;
    return pos.y - h;
}

Surface rayMarch(vec3 ro, vec3 rd) {
  float depth = MIN_DIST;
  Surface co;

  for (int i = 0; i < MAX_MARCHING_STEPS; i++) {
    vec3 p = ro + depth * rd;
    co.sd = map(p);
    depth += co.sd;
    if (co.sd < PRECISION || depth > MAX_DIST) break;
  }
  
  co.sd = depth;

  return co;
}

vec3 calcNormal( in vec3 pos )
{
    vec2 e = vec2(1.0,-1.0) * 0.001;
    
    return normalize( e.xyy*map( pos + e.xyy ) + 
					  e.yyx*map( pos + e.yyx ) + 
					  e.yxy*map( pos + e.yxy ) + 
					  e.xxx*map( pos + e.xxx ) );
}

mat3 camera(vec3 cameraPos, vec3 lookAtPoint) {
	vec3 cd = normalize(lookAtPoint - cameraPos); // camera direction
	vec3 cr = normalize(cross(vec3(0, 1, 0), cd)); // camera right
	vec3 cu = normalize(cross(cd, cr)); // camera up
	
	return mat3(-cr, cu, -cd);
}
vec3 phong(vec3 lightDir, vec3 normal, vec3 rd) {
  // ambient
  float k_a = 0.6;
  vec3 i_a = vec3(0.2, 0.5, 0.8);
  vec3 ambient = k_a * i_a;

  // diffuse
  float k_d = 0.7;
  float dotLN = clamp(dot(lightDir, normal), 0., 1.);
  vec3 i_d = vec3(0., 0.3, 0.7);
  vec3 diffuse = k_d * dotLN * i_d;

  // specular
  float k_s = 0.6;
  float dotRV = clamp(dot(reflect(lightDir, normal), -rd), 0., 1.);
  vec3 i_s = vec3(0.5, 1, 1);
  float alpha = 9.;
  vec3 specular = k_s * pow(dotRV, alpha) * i_s;

  return ambient + diffuse + specular;
}

void main( )
{
	vec2 p = xlv_TEXCOORD0.xy / iResolution.xy;
    vec2 q = (-iResolution.xy + 2.1* xlv_TEXCOORD0.xy) / iResolution.y;
     
    // ray
    vec3 ro = vec3( .0, 2.2, -2. );
    vec3 lp = vec3(0., 2, 4); // lookat point (aka camera target

  vec3 rd = camera(ro, lp) * normalize( vec3(q-vec2(-0.2,0.6),-2.0));
    
    vec3 col = vec3( 0.6, 0.8, 1.0 );

    Surface co = rayMarch(ro, rd);
    if( co.sd < MAX_DIST )
    {
        vec3 pos = ro + co.sd*rd;
        vec3 nor = calcNormal( pos );
      
        vec3 mate = vec3(0.0,0.5,0.8);
        
        col = mate;
	
	vec3 lightPosition1 = vec3(8, 2, -20);
      vec3 lightDirection1 = normalize(lightPosition1 - co.sd);
      float lightIntensity1 = 0.85;
      
      vec3 lightPosition2 = vec3(1, 1, 10);
      vec3 lightDirection2 = normalize(lightPosition2 - co.sd);
      float lightIntensity2 = 0.8;
      
      
      col = lightIntensity1 * phong(lightDirection1, nor, rd);
      col += lightIntensity2 * phong(lightDirection2, nor , rd);
       }
    else
    {
#if 0
        // from iq's shader, https://www.shadertoy.com/view/MdX3Rr
        float sundot = clamp(dot(rd, lightDir), 0.0, 1.0);
        col = vec3(0.3,0.5,0.85) - rd.y*rd.y*0.5;
        col = mix( col, 0.85*vec3(0.7,0.75,0.85), pow( 1.0-max(rd.y,0.0), 4.0 ) );
        // sun
		col += 0.25*vec3(1.0,0.7,0.4)*pow( sundot,5.0 );
		col += 0.25*vec3(1.0,0.8,0.6)*pow( sundot,64.0 );
		col += 0.2*vec3(1.0,0.8,0.6)*pow( sundot,512.0 );
        // clouds
		vec2 sc = ro.xz + rd.xz*(SC*1000.0-ro.y)/rd.y;
		col = mix( col, vec3(1.0,0.95,1.0), 0.5*smoothstep(0.5,0.8,fbm(0.0005*sc/SC)) );
        // horizon
        col = mix( col, 0.68*vec3(0.4,0.65,1.0), pow( 1.0-max(rd.y,0.0), 16.0 ) );
#else
	float sundot = clamp(dot(rd,lightDir),0.0,1.0);
        // sky		
		col = vec3(0.3,.55,0.8)*(1.0-0.8*rd.y)*0.9;
        // sun
		col += 0.25*vec3(1.0,0.7,0.4)*pow( sundot,5.0 );
		col += 0.25*vec3(1.0,0.8,0.6)*pow( sundot,64.0 );
		col += 0.2*vec3(1.0,0.8,0.6)*pow( sundot,512.0 );
        // clouds
		vec2 sc = ro.xz + rd.xz*(1000.0-ro.y)/rd.y;
		col = mix( col, vec3(1.0,0.95,1.0), 0.5*smoothstep(0.5,0.8,fbm(0.0005*sc)) );
        // horizon
        col = mix( col, vec3(0.7,0.75,0.8), pow( 1.0-max(rd.y,0.0), 8.0 ) );
#endif
	}
	gl_FragColor = vec4( col, 1.0 );
}

