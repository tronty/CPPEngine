uniform sampler2D iChannel1;
uniform sampler2D iChannel0;
 //trying to account for resolution dependant MipBias
#define MIPs (8.5+iResolution.y/512.0)

float Sphere (in vec2 Coord, in vec2 Position, in float Size)
{
    return 1.0-clamp(dot(Coord/Size-Position,Coord/Size-Position),0.0,1.0);
}

float SelectMip (in float Roughness)
{
    return MIPs-1.0-(3.0-1.15*log2(Roughness));
}

vec2 Reflection (in vec2 Coord, in vec2 Position, in float Size, in float NormalZ)
{
    return (1.0-Size*(Coord/Size-Position)/NormalZ)/2.0;
}

vec4 BlurTexture (in vec2 Coord, in sampler2D Tex, in float MipBias)
{
	vec2 TexelSize = MipBias/iChannelResolution[0].xy;
    
    vec4  Color = texture2D(Tex, Coord, MipBias);
    Color += texture2D(Tex, Coord + vec2(TexelSize.x,-1.0), MipBias);    	
    Color += texture2D(Tex, Coord + vec2(-TexelSize.x,-1.0), MipBias);    	
    Color += texture2D(Tex, Coord + vec2(0.0,TexelSize.y), MipBias);    	
    Color += texture2D(Tex, Coord + vec2(0.0,-TexelSize.y), MipBias);    	
    Color += texture2D(Tex, Coord + vec2(TexelSize.x,TexelSize.y), MipBias);    	
    Color += texture2D(Tex, Coord + vec2(-TexelSize.x,TexelSize.y), MipBias);    	
    Color += texture2D(Tex, Coord + vec2(TexelSize.x,-TexelSize.y), MipBias);    	
    Color += texture2D(Tex, Coord + vec2(-TexelSize.x,-TexelSize.y), MipBias);    

    return Color/9.0;
}

void main( )
{
    vec2 uv = xlv_TEXCOORD0.xy/iResolution.yy;    
    
    //Objects    
    float s1 = Sphere(uv, vec2(1.5,1.25),0.25);	float s2 = Sphere(uv, vec2(4.5,1.25),0.25);
    float s3 = Sphere(uv, vec2(3.0,2.75),0.25);	float s4 = Sphere(uv, vec2(6.0,2.75),0.25);    
    float sph = s1*0.25+s2*0.25+s3*0.25+s4*0.25;    
    float spm = clamp(sph*64.0,0.0,1.0);   
    
    //Normals    
    float dx = dFdx(sph)*iResolution.x/4.0;    
    float dy = dFdy(sph)*iResolution.x/4.0;     
    vec3 vNormal = normalize(vec3(dx,dy,sqrt(clamp(1.0-dx*dx-dy*dy,0.0,1.0)))); 
    
    //Shading    
	uv = (2.0*xlv_TEXCOORD0.xy-iMouse.xy*0.1)/(iResolution.yy*2.0);
    
    vec2 uvr = ceil(s1)*Reflection(uv, vec2(1.5,1.25),0.25,vNormal.z) + ceil(s2)*Reflection(uv, vec2(4.5,1.25),0.25,vNormal.z)
        	 + ceil(s3)*Reflection(uv, vec2(3.0,2.75),0.25,vNormal.z) + ceil(s4)*Reflection(uv, vec2(6.0,2.75),0.25,vNormal.z);    
    
    vec3 BaseColor = ceil(s1)*vec3(1.0,0.76,0.33) + ceil(s2)*vec3(0.66,0.61,0.53)
                   + ceil(s3)*vec3(0.95,0.64,0.54)+ ceil(s4)*vec3(0.56,0.57,0.58);
    
    float Roughness = ceil(s1)*0.20+ceil(s2)*0.35+ceil(s3)*0.27+ceil(s4)*0.5;
    	  Roughness = mix(Roughness*1.5, Roughness*0.67, texture2D(iChannel1,8.0*uvr).x);
       
    vec3 vLight = normalize(vec3(xlv_TEXCOORD0.xy,128.0)-vec3(iMouse.xy,0.0));
    
    vec3 vEye = vec3(0.0,0.0,-1.0);
    
    float Fresnel = 1.0-pow(clamp(dot(vNormal,-vEye),0.0,1.0),1.0);
    
    vec3 Environment = (1.0+Fresnel) * clamp(1.0*BlurTexture(uvr,iChannel0,SelectMip(Roughness)).xyz +
                       3.0*clamp(texture2D(iChannel0,uvr,SelectMip(Roughness)+1.0).xxx-0.67,0.0,1.0),0.0,2.0);
    
    float sh = 0.75+0.25*clamp(dot(vNormal,vLight),0.0,1.0);
    
    vec3 sp = vec3(1.0-Roughness)*pow(clamp(dot(vNormal,normalize((vLight-vEye)*0.5)),0.0,1.0),1.0/(pow(Roughness+0.1,4.0)));
    
    vec3 fcol01 = mix(sh*BaseColor*Environment,mix(normalize(BaseColor)*2.5,vec3(1.5),0.4),sp);
    
    vec3 fcol02 = pow(1.375*texture2D(iChannel0,(0.2+uv)*vec2(0.4,-0.8),SelectMip(1.0)).xyz,vec3(1.5));

    gl_FragColor = vec4(mix(fcol02,fcol01,spm),1.0);
}

