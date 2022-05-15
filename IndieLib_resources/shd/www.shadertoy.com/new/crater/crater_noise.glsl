#define MAX_DIST 20.
#define MIN_DIST .001
#define MAX_ITER 100

const vec3 BOX = vec3(0.5);
const float eps = 0.0001;
float hash(
	in float n
){
	return fract(sin(n)*753.5453123);
}

vec3 getColor(vec3 pos, vec3 normal){
	float slope = abs(normal.y);
	vec3 mainColor = vec3( 0.75 *vec3(0.9647, 0.3843, 0.23549));
	vec3 orangeRust = vec3(195.0, 88.0, 23.0)/255.0;
	vec3 mid  = vec3(0.75686276,0.6039216,0.41960785) * 0.8;
	vec3 col = mix(mid * 0.75, orangeRust * 0.65, slope * .75 );
	return col * 0.5;
}

float noise(in vec3 x){
	vec3 p = floor(x);
	vec3 f = fract(x);
	f = f*f*(3.0 - 2.0*f);
    float n = p.x + p.y*157.0 + 113.0*p.z;
    return mix(mix(mix( hash(n+  0.0), hash(n+  1.0),f.x),
                   mix( hash(n+157.0), hash(n+158.0),f.x),f.y),
               mix(mix( hash(n+113.0), hash(n+114.0),f.x),
                   mix( hash(n+270.0), hash(n+271.0),f.x),f.y),f.z);
}

const mat3 m = mat3( 0.00,  0.80,  0.60,
                    -0.80,  0.36, -0.48,
                    -0.60, -0.48,  0.64 );
float fbm (in vec3 p, in int octaves)
{
    float f = 0.0;
    float freq = 1.0;
    for (int i = 0; i < octaves; i++)
    {
        float n = noise((p * m) * freq) / freq;
        f += n;
        freq *= 2.0;
    }
    return f;
}



vec4 gpuGetCell3D(const in int x, const in int y, const in int z)
{
	float u = float(x + y * 31) / 256.0;
	float v = float(z - x * 3) / 256.0;
	return(texture(iChannel1, vec2(u, v)));
}

vec2 gpuCellNoise3D(const in vec3 xyz)
{
	int xi = int(floor(xyz.x));
	int yi = int(floor(xyz.y));
	int zi = int(floor(xyz.z));

	float xf = xyz.x - float(xi);
	float yf = xyz.y - float(yi);
	float zf = xyz.z - float(zi);

	float dist1 = 9999999.0;
	float dist2 = 9999999.0;
	vec3 cell;

	for (int z = -1; z <= 1; z++)
	{
		for (int y = -1; y <= 1; y++)
		{
			for (int x = -1; x <= 1; x++)
			{
				cell = gpuGetCell3D(xi + x, yi + y, zi + z).xyz;
				cell.x += (float(x) - xf);
				cell.y += (float(y) - yf);
				cell.z += (float(z) - zf);
				float dist = dot(cell, cell);
				if (dist < dist1)
				{
					dist2 = dist1;
					dist1 = dist;
				}
				else if (dist < dist2)
				{
					dist2 = dist;
				}
			}
		}
	}
	return vec2(sqrt(dist1), sqrt(dist2));
}

float f2mf1(vec3 p)
{
	vec2 c = gpuCellNoise3D(p);

	return c.y - c.x;
}


float craterNoise3D(in vec3 p){
	
    
    const float radius = 0.25;
    const float slope = .095;
    const float frequency = 2.35;
    const float depth = -0.982;
    const float rimWidth = 0.125;
    
	float fractal = fbm(p * frequency * 2.0, 4) * 0.07;
	float cell = gpuCellNoise3D((p * frequency) + fractal ).x;
	float r = radius + fractal;
	float crater = smoothstep(slope, r, cell);
	  	  crater = mix(depth, crater, crater);
	float rim = 1.0 - smoothstep(r, r + rimWidth, cell);
	      crater = rim - (1.0 - crater);
	return crater * 0.08;
}

  
float sdSphere(vec3 p, vec3 c, float r) {
    
    return length(p - c) - (r + (fbm(p * 2.15, 6) * 0.25) + (craterNoise3D(p / 2.0)));
}

float map(in vec3 p){
    return sdSphere(p, vec3(0.0), 4.0);
}
vec3 sNormal(in vec3 p){
    float e = 0.001;
    float d = map(p);
    float xd = map(p+vec3(e,0,0));
    float yd = map(p+vec3(0,e,0));
    float zd = map(p+vec3(0,0,e));
    vec3 n = (vec3(xd,yd,zd)-d)/e;
    return n;
}
float castRay(vec3 ro, vec3 rd){
    float t = 0.0; 
    for(int i=0; i<MAX_ITER; i++){
        vec3 p = ro + t*rd;
        float h = map(p);
        if(abs(h)<MIN_DIST) break;
        t+=h*0.75;
        if(t>MAX_DIST) break;
    }
    if(t>MAX_DIST) t = -1.0;
    return t;
}
vec3 camera(in vec2 uv, in vec3 ro, vec3 ta, float fd){
    vec3 up = vec3(0,1,0); // global up vector
    vec3 ww = normalize(ta-ro); // direction to the target
    vec3 uu = normalize(cross(ww, up)); // camera right direction
    vec3 vv = normalize(cross(uu, ww)); // camera up direction
    
    vec3 rd = normalize(uv.x*uu + uv.y*vv + fd*ww); // camera direction to uv
    return rd;
}
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord/iResolution.xy;
    uv -= vec2(0.5);
    uv.x *= iResolution.x/iResolution.y;
    vec3 li = normalize(vec3(0.5, .8, 3.0));
    float a = 10.0 * iMouse.x/iResolution.x;
    float taDist = 2.0;
    vec3 ro = vec3( 8.0 * sin(a + float(iTime * 0.25)), 7.0, 8.0* cos(a + float(iTime * 0.25)));
    vec3 ta = vec3(0,0,0);
    
    vec3 rd = camera(uv, ro, ta, 1.0);
    float t = castRay(ro, rd);
    vec3 col = vec3(0.0);
    if (t > 0.0) {
        vec3 pos = ro + t*rd;
        vec3 normal = sNormal(pos);
        //col =mix(vec3(0.6, 0.4, 0.3), vec3(0.2), smoothstep(4.5, 5.5, length(pos))) + 0.25*pow(max(dot(li, normal)*1.2, 0.05), .75);
        col = (dot(li, normal) + 1.15) * getColor(pos, normal);
    }

    fragColor = vec4(col, 1.0);
}

