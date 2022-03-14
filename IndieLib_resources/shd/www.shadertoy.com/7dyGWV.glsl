// https://www.shadertoy.com/view/7dyGWV
// Insane Bumpy Terrain
////////////////////////////////
// Terrain generation section //
////////////////////////////////

float snoise(vec2 p) {
	vec2 f = fract(p);
	p = floor(p);
	float v = p.x+p.y*1000.0;
	vec4 r = vec4(v, v+1.0, v+1000.0, v+1001.0);
	r = fract(100000.0*sin(r*.001));
	f = f*f*(3.0-2.0*f);
	return 2.0*(mix(mix(r.x, r.y, f.x), mix(r.z, r.w, f.x), f.y))-1.0;
}

float noise(in vec2 uv)
{
    return sin(uv.x)+cos(uv.y);
}

float terrain(in vec2 uv,int octaves)
{
    //float scale_factor = 1.;
    //uv /= scale_factor;
    //this function generates the terrain height
    float value = 0.;
    float amplitude = 1.;
    float freq = .25;
    float n1 = 0.;
    
    //float f2 = .1; //this constant changes the variation in mountain height
    
    //float f1 = f2;
    //float f3 = 1.-f1;
    //vec2 uv1 = uv;
    for (int i = 0; i < octaves; i++)
    {
        //uv += vec2(amplitude,freq);
        float n2 = noise((uv) * freq);
        n1 = abs(n2+n1+freq);
        // From Dave_Hoskins https://www.shadertoy.com/user/Dave_Hoskins
        
        
        value = (value-n1 * amplitude);
        freq *= 1.5+1./(1.+amplitude);
        amplitude *= (1./(1.+freq));
        

        uv = uv.yx+n1/(1.+amplitude);

        //value *= .9-.1*noise(uv/freq); //dunes


}
    return value;
}


vec2 map(vec3 p, int octaves) {
	float dMin = 28.0;
	float d;
	float mID = -1.0;
	
	// Mountains
	float h = terrain(p.xz, octaves);
	//h += smoothstep(0.0, 1.1, h);
    //h += smoothstep(-0.1, 1.0, p.y)*0.6;
	d = p.y - h;
	if (d<dMin) { 
		dMin = d;
		mID = 0.0;
	}

	return vec2(dMin, mID);
}

////////////////////
// Render section //
////////////////////

vec2 castRay(vec3 ro, vec3 rd, int octaves) {
	const float p = 0.001;
	float t = 0.0;
	float h = p * 2.0;
	float m = -1.0;
	for (int i=0; i<36; i++) {
		if (abs(h)<=p || t>=28.0) break;
        t += h;
        vec2 res = map(ro + rd*t, octaves);
        h = res.x;
        m = res.y;
	}
	if (t>28.0) m = -1.0;
	return vec2(t, m);
}

vec3 calcNormal(vec3 p, int octaves) {
	const vec3 eps = vec3(0.002, 0.0, 0.0);
	return normalize( vec3(map(p+eps.xyy, octaves).x - map(p-eps.xyy, octaves).x,
			       map(p+eps.yxy, octaves).x - map(p-eps.yxy, octaves).x,
			       map(p+eps.yyx, octaves).x - map(p-eps.yyx, octaves).x) );
}

float shadows(vec3 ro, vec3 rd, float tMax, float k, int octaves) {
    float res = 1.0;
	float t = 0.1;
	for(int i=0; i<22; i++) {
        if (t>=tMax) break;
        float h = map(ro + rd*t, octaves).x;
        res = min( res, k*h/t );
        t += h;
    }
    return clamp(res, 0.2, 1.0);
}

vec3 render(vec3 ro, vec3 rd) {
	const int geoLOD = 4;
	
	vec2 res = castRay(ro, rd, geoLOD);
	
	vec3 lPos = normalize(vec3(1.0, 0.5, 0.0));
	vec3 lCol = vec3(1.0, 0.9, 0.8);
	
	vec3 pos = ro + rd*res.x;
	
	// mat -1 = Background / sky
    vec3 color = vec3(0.45,0.5,0.6);
    float sun = clamp(dot(rd,lPos),0.0,1.0);
    color += 0.6 * lCol * sun*sun;
    if (res.y < -0.5) {
		return color;
	}
    
    vec3 skyColor = color;
	
	int norLOD = int(max(2.0, 12.0-11.0*res.x/28.0));
	vec3 nor = calcNormal(pos, norLOD);
	
	// mat 0 = Rock / mountain
	if (res.y>-0.5 && res.y<0.5) {
		// Base rock
		color = mix( vec3(0.4, 0.1, 0.0), vec3(0.7, 0.6, 0.3), step(0.9, nor.y) );
		
		// Layer noise
		float n = 0.5*(snoise(pos.xy*vec2(2.0, 15.0))+1.0);
		color = mix( vec3(0.6, 0.5, 0.4), color, n*smoothstep(0.0, 0.7, 1.0-nor.y) );
		
        // Sand on top
        color = mix(color, vec3(0.7, 0.6, 0.3), smoothstep(0.0, 0.2, nor.y-0.8));
	}
	// mat 1 = Sand
	if (res.y>0.5) {
		// Base sand and rock color
		color = mix( vec3(0.3, 0.2, 0.0), vec3(0.7, 0.6, 0.3), nor.y );
	}
	
    // Lighting and shadows
    float lAmb = clamp( 0.5 + 0.5 * nor.y, 0.0, 1.0);
    float lDif = clamp( dot( nor, lPos ), 0.0, 2.0);

    if (lDif>0.05) lDif *= shadows(pos, lPos, 8.0, 12.0, geoLOD);

    color += (0.4*lAmb) * lCol;
    color *= (1.8*lDif) * lCol;	
    
	// Fog
	float fog = exp(-0.003 *res.x*res.x);
	color = mix(skyColor, color, fog);
		
	return color;
}

void main( ) {
	vec2 pos = 2.0 * ( gl_FragCoord.xy / iResolution.xy ) - 1.0;
	pos.x *= iResolution.x / iResolution.y;
    float t1 = iTime;
	// Camera
	float x = 0.0 + (0.5*t1);
	float y = 0.0;
	float z = 0.0 + sin(0.1*t1)*2.;
	vec3 cPos = vec3(x, y, z);
	cPos.y = terrain(cPos.xz, 1) + 2.5;
	
	const vec3 cUp = vec3(0., 1., 0.);
	vec3 cLook = vec3(cPos.x + 1.0, cPos.y*0.85, 0.0);
	
	// Camera matrix
	vec3 ww = normalize( cLook-cPos );
	vec3 uu = normalize( cross(ww, cUp) );
	vec3 vv = normalize( cross(uu, ww) );
	
	vec3 rd = normalize( pos.x*uu + pos.y*vv + 2.0*ww );
	
	// Render
	vec3 color = render(cPos, rd);
	
	gl_FragColor = vec4( color, 1.0 );
}

