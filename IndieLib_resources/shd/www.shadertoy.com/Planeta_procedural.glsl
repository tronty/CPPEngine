float generate_map = 150.0;
const int ssaa=4;

mat2 rotation(float x)
{
    return mat2(cos(x),-sin(x),sin(x),cos(x));
}

float noise (vec3 n) 
{ 
	return fract(sin(dot(n, vec3(95.43583, 93.323197, 94.993431))) * 65536.32);
}

float perlin_a (vec3 n)
{
    vec3 x = floor(n * 64.0) * 0.015625;
    vec3 k = vec3(0.015625, 0.0, 0.0);
    float a = noise(x);
    float b = noise(x + k.xyy);
    float c = noise(x + k.yxy);
    float d = noise(x + k.xxy);
    vec3 p = (n - x) * 64.0;
    float u = mix(a, b, p.x);
    float v = mix(c, d, p.x);
    return mix(u,v,p.y);
}

float perlin_b (vec3 n)
{
    vec3 base = vec3(n.x, n.y, floor(n.z * 64.0) * 0.015625);
    vec3 dd = vec3(0.015625, 0.0, 0.0);
    vec3 p = (n - base) *  64.0;
    float front = perlin_a(base + dd.yyy);
    float back = perlin_a(base + dd.yyx);
    return mix(front, back, p.z);
}

float fbm(vec3 n)
{
    float t = 0.0;
    float a = 1.0;
    float b = 0.1;
    for (int i = 0; i < 5; i++)
    {
        t += perlin_b(n * a) * b;
        a *= 0.5;
        b *= 2.0;
    }
    return t;
}

vec3 heightmap (vec3 n)
{
	return vec3(fbm((5.0 * n) + fbm((5.0 * n) * 3.0 - 1000.0) * 0.05),0,0);
}

float sphere(vec3 ro, vec3 rd)
{
    float b = dot(ro,rd);
    float c = dot(ro,ro)-1.0;
    float h = b*b-c;
    return (h<0.0)?-1.0:-b-sqrt(h);
}

vec3 atlas(vec2 uv)
{    
 	float color = clamp(vec4(vec3((heightmap(vec3(uv*5.0,generate_map)*0.02)-1.0)),1.0).r,0.0,1.0);
    return 
    color<0.10 ? vec3(0.73,0.85,0.98):
    color<0.20 ? vec3(0.77,0.90,0.98):
    color<0.30 ? vec3(0.82,0.92,0.99):
    color<0.55 ? vec3(0.91,0.97,0.99):
    color<0.65 ? vec3(0.62,0.75,0.59):
    color<0.75 ? vec3(0.70,0.80,0.68):
    color<0.85 ? vec3(0.77,0.86,0.73):
    color<0.95 ? vec3(0.82,0.88,0.75):     
    color<0.99 ? vec3(0.87,0.92,0.77):              
    vec3(0.93,0.99,0.81); 
}

vec3 raycast (vec3 ro, vec3 rd, vec2 p)
{
    float t = sphere(ro,rd);
    if (t > 0.0)
    {
    	vec3 d = ro+rd*t;
        p=vec2(acos(d.y/length(d)), atan(d.z,d.x));  
        return (atlas(p).xyz);      
    } 
    return 1.7-vec3(length(p));
}

void main( )
{	
	vec3 c = vec3(0.0);
	for (int m=0;m<ssaa;m++)
	{
		for (int n=0;n<ssaa;n++)
		{
			vec2 offset = vec2(float(m),float(n))/float(ssaa)-0.5;
			vec2 p = (xlv_TEXCOORD0.xy+offset.xy)/iResolution.xy-0.5;	
            p.x*=iResolution.x/iResolution.y;
            vec2 m = iMouse.xy / iResolution.xy-0.5;
            m.x *= iResolution.x/iResolution.y;
            p*= 1.5;
            vec3 ro = vec3(0.0,0.0,2.4);
            vec3 rd = normalize(vec3(p,-1.5));
            mat2 rx = rotation(iTime*0.4+m.x*5.0);
            ro.xz *= rx; rd.xz *= rx;  
			c+=raycast(ro,rd,p);
		}
	}
	c=c/float(ssaa*ssaa);    
	gl_FragColor = vec4(c, 1.0);
}
