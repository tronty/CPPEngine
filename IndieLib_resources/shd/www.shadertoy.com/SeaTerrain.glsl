// https://www.shadertoy.com/view/flfczs
// Help from https://www.shadertoy.com/view/MtsSRf

const int MAX_MARCHING_STEPS = 255;
const float MIN_DIST = 0.0;
const float MAX_DIST = 100.0;
const float PRECISION = 0.001;

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
    
	gl_FragColor = vec4( col, 1.0 );
}

