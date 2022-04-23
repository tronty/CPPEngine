uniform sampler2D iChannel0;
vec3 lightSource = vec3(10.0,10.0,-10.5);
const vec3 v = vec3(0.0,0.0,-10.0);
#if 1
const vec4 ambient = vec4(0.1,0.1,0.1,1.0);
const vec4 diffuse = vec4(0.6,0.6,0.6,1.0);
const vec4 specular = vec4(0.9,0.9,0.9,1.0);
#else
const vec4 ambient = 0.5 * vec4(0.7, 0.5, 0, 1.0);
const vec4 diffuse = 0.6 * vec4(0.7, 0.7, 0, 1.0);
const vec4 specular = 0.6 * vec4(1, 1, 1, 1.0);
#endif
float shininess = 180.0;


vec4 phong(vec3 normal, vec4 color) {
   // see https://www.opengl.org/sdk/docs/tutorials/ClockworkCoders/lighting.php 
   vec3 L = normalize(lightSource - v);   
   vec3 E = normalize(-v); 
   vec3 R = normalize(-reflect(L,normal));  
 
   //calculate Ambient Term:  
   vec4 Iamb = ambient;    

   //calculate Diffuse Term:  
   vec4 Idiff = diffuse * max(dot(normal,L), 0.0);
   Idiff = clamp(Idiff, 0.0, 1.0);     
   
   // calculate Specular Term:
   vec4 Ispec = specular 
                * pow(max(dot(R,E),0.0),0.3*shininess);
   Ispec = clamp(Ispec, 0.0, 1.0); 
   // write Total Color:  
    vec4 p =Iamb + Idiff + Ispec;
    
    vec4 r = color;
    r.x *= p.x;
    r.y *= p.y;
    r.z *= p.z;
    
    return r;
    
}

void main( )
{
    
  //  lightSource = vec3(cos(iTime)*10.0, 10.5, sin(iTime)*10.5);
    
	vec2 uv = xlv_TEXCOORD0.xy / iResolution.xy;
    
    float bias = iResolution.x/iResolution.y;
    
   uv.x*=iResolution.x/iResolution.y;
    
    vec2 middle= vec2(0.5*iResolution.x/iResolution.y,0.5);
 
    
    float len = length(middle - uv);
    
    if(length(middle - uv) < 0.3) {
	   float z = sqrt(abs((len)*(len) - 0.09));
        vec3 norm = normalize(vec3(uv.xy, z)-vec3(middle.xy,0.0));
        
        float su =  atan(norm.z, norm.x) / 3.1415*2.0 + iTime / 5.0;
		float sv = asin(norm.y)/3.14+0.5;
        
        vec4 color = texture2D(iChannel0, vec2(su,sv));
        
        // try random bumping
        norm.x += sin((su)*40.0)/4.;
        norm.y += sin(sv  *40.0 )/4.;
        
        gl_FragColor = phong(normalize(norm), color);
        
     
        
    } else {
        gl_FragColor = vec4(0.0,0.0,0.0,1.0);
    }
  
}

