/*originals https://glslsandbox.com/e#73363.0 and other*/
#ifdef GL_ES
precision highp float;
#endif
 
#if 0 
uniform float time;
uniform vec2 resolution;
#endif

#define iterations 12
#define formuparam2 0.79
 
#define volsteps 5
#define stepsize 0.390
 
#define zoom 0.900
#define tile   0.850
#define speed2  500.0 
#define brightness 0.003
#define darkmatter 0.400
#define distfading 0.560
#define saturation 0.800


#define transverseSpeed zoom*4.0
#define cloud 0.21 

 
float triangle(float x, float a) { 
	float output2 = 2.0*abs(  2.0*  ( (x/a) - floor( (x/a) + 0.5) ) ) - 1.0;
	return output2;
}
 
float field(in vec3 p) {	
	float strength = 10. + .03 * log(1.e-6 + fract(sin(time) * 43273.11));
	float accum = 0.1;
	float prev = 0.2;
	float tw = 0.2;	

	for (int i = 0; i < 12; ++i) {
		float mag = dot(p, p);
		p = abs(p) / mag + vec3(-.5, -.8 + 0.5*sin(time*0.7 + 2.0), -1.1+0.3*cos(time*0.3));
       
		float w = exp(-float(i) / 7.);
		accum += w * exp(-strength * pow(abs(mag - prev), 2.3));
		tw += w;
		prev = mag;
	}
	return max(0.2, 5. * accum / tw - 0.9);
}

float random (float st) {
    return .1 + fract(sin(dot(st, 12.9898)) * 43758.5453123);
}
float happy_healthy_star(vec2 uv, float anim)
{
    uv = abs(uv);
    vec2 pos = min(uv.xy/uv.yx, anim);
    float p = (2.0 - pos.x - pos.y);
    return (2.0+p*(p*p-1.5)) / (uv.x+uv.y);      
}
 
void main(){

float time=iTime;
     	vec2 uv2 = 2. * xlv_TEXCOORD0.xy / vec2(512) - 1.;
	vec2 uvs = uv2 * vec2(512)  / 512.;
	
 
	    vec2 uv3=xlv_TEXCOORD0.xy/iResolution.xy-0.5;

	float time2 = time;               
        float speed = speed2;
        speed = .01 * cos(time2*0.02 + 3.1415926/4.0);          
	#define num 50.
   float t = iTime * .1 + ((.25 + .05 * sin(iTime * .1))/(length(uv2.xy) + 1.07)) * 2.2;
	float si = sin(t);
	float co = cos(t);
	mat2 ma = mat2(co, si, -si, co);
  uv2 *= 2.0 * ( cos(iTime * 2.0) -2.5); // scale
    float anim = sin(iTime * 12.0) * 0.1 + 1.0;  // anim between 0.9 - 1.1 

  
    float level = .5;
    
	
	
    	float formuparam = formuparam2;
     
   
	vec2 uv = uvs;		       
	 uv.xy*=ma; 
     
	float a_xz = 0.9;
	float a_yz = -.6;
	float a_xy = 0.9 + time*0.08;	
	
	mat2 rot_xz = mat2(cos(a_xz),sin(a_xz),-sin(a_xz),cos(a_xz));	
	mat2 rot_yz = mat2(cos(a_yz),sin(a_yz),-sin(a_yz),cos(a_yz));		
	mat2 rot_xy = mat2(cos(a_xy),sin(a_xy),-sin(a_xy),cos(a_xy));
	

	float v2 =1.0;	
	vec3 dir=vec3(uv*zoom,1.); 
	vec3 from=vec3(0.0, 0.0,0.0);                               
        from.x -= 5.0*(vec2(sin(time)/48., cos(time)/48.).x-0.5);
        from.y -= 5.0*(vec2(sin(time)/48., cos(time)/48.).y-0.5);
            
	vec3 forward = vec3(0.,0.,1.);   
	from.x += transverseSpeed*(1.0)*cos(0.01*time) + 0.001*time;
	from.y += transverseSpeed*(1.0)*sin(0.01*time) +0.001*time;
	from.z += 0.003*time;	
	
	dir.xy*=rot_xy;
	forward.xy *= rot_xy;
	dir.xz*=rot_xz;
	forward.xz *= rot_xz;	
	dir.yz*= rot_yz;
	forward.yz *= rot_yz;
	
	from.xy*=-rot_xy;
	from.xz*=rot_xz;
	from.yz*= rot_yz;
	    vec2 p2;
    for (float i = 0.; i < num; i += 1.)
    {
        float r1 = random(i);
        float r2 = random(i + .3);
        float r3 = random(i + .5);
        float r4 = random(i + .7);
        
        vec2 p = vec2(
            sin(iTime * r1) / 3. + .5, 
            sin(iTime * r2) / 3. + .5);
           
        float dist = distance(p-0.5, dir.xy*7.);
      
        level += 2. / num * sin(-iTime * 10. * r3 + 2.0 * r4 * float(i) * dist) * exp(-5.0 * dist);
    }
	

	float zooom = (time2-3311.)*speed;
	from += forward* zooom;
	float sampleShift = mod( zooom, stepsize );
	 
	float zoffset = -sampleShift;
	sampleShift /= stepsize;
	     vec4 fragColor2 = vec4(happy_healthy_star(dir.xy, anim) * vec3(0.55,0.5,0.55)*0.05, 1.0);		             
	 
	
	float s=0.24;
	float s3 = s + stepsize/2.0;
	vec3 v=vec3(0.);
	float t3 = 0.0;	
	
	vec3 backCol2 = vec3(0.);
	for (int r=0; r<volsteps; r++) {
		vec3 p2=from+(s+zoffset)*dir+level;
		vec3 p3=from+(s3+zoffset)*dir*level;
		
		p2 = abs(vec3(tile)-mod(p2,vec3(tile*2.)));
		p3 = abs(vec3(tile)-mod(p3,vec3(tile*2.)));		
		#ifdef cloud
		t3 = field(p3);
		#endif
		
		float pa,a=pa=0.;
		for (int i=0; i<iterations; i++) {
			p2=abs(p2)/dot(p2,p2)-formuparam;
			p2.xy*=mat2(cos(iTime*0.025),sin(iTime*0.025),-sin(iTime*0.025),cos(iTime*0.025));
			float D = abs(length(p2)-pa);
			a += i > 7 ? min( 12., D) : D;
			pa=length(p2);
		}
		
		
		
		a*=a*a;
		
		float s1 = s+zoffset;
		
		float fade = pow(distfading,max(0.,float(r)-sampleShift));		
			
		v+=fade;
	       	

		
		if( r == 0 )
			fade *= (1. - (sampleShift));
		
		if( r == volsteps-1 )
			fade *= sampleShift;
		v+=vec3(s1,s1*s1,s1*s1*s1*s1)*a*brightness*fade;
		
		backCol2 += mix(2.3, 1.2, v2) * vec3(1.0 * t3 * t3 * t3, 1.0 * t3 * t3, t3) * fade;

		
		s+=stepsize;
		s3 += stepsize;		
	}
		       
	v=mix(vec3(length(v)),v,saturation);	

	vec4 forCol2 = vec4(v*.01,1.);	
	#ifdef cloud
	backCol2 *= cloud;
	#endif	
	backCol2.b *= 1.8;
	backCol2.r *= 0.05;	
	
	backCol2.b = 0.5*mix(backCol2.g, backCol2.b, 0.8);
	backCol2.g = 0.0;
	backCol2.bg = mix(backCol2.gb, backCol2.bg, 0.5*(cos(time*0.01) + 1.0));	
	gl_FragColor = forCol2 + vec4(backCol2+fragColor2.xyz, 1.0);
}

