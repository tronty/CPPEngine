uniform sampler2D iChannel4;
uniform sampler2D iChannel5;
uniform sampler2D iChannel6;
// Modified version of the "Volcanic" shader by by inigo quilez - iq/2013
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.


float noise( in vec3 p ){    
        vec3 i = floor(p); p -= i; p *= p*(3. - 2.*p);
	p.xy = texture2D(iChannel4, (p.xy + i.xy + vec2(37, 17)*i.z + .5)/256., -100.).yx;
	return mix(p.x, p.y, p.z);
}


vec4 texcube( sampler2D sam, in vec3 p, in vec3 n )
{
	vec4 x = texture2D( sam, p.yz );
	vec4 y = texture2D( sam, p.zx );
	vec4 z = texture2D( sam, p.xy );
	return x*abs(n.x) + y*abs(n.y) + z*abs(n.z);
}

//=====================================================================

vec3 path( float time )
{
	return vec3(cos(0.51 * time) + sin(0.14 * time), 0.8 * sin(0.27 * time), time);
	
}


const mat3 m = mat3( 0.00,  0.80,  0.60,
                    -0.80,  0.36, -0.48,
                    -0.60, -0.48,  0.64 );

float cave( vec3 p )
{

    float f = 0.0;
    
    vec3 s = 1.2 * vec3(sin(p.z * 0.32), cos(p.z * 0.77), 1.0);
    
    vec3 d = (path(p.z) - p) * s;
    float dist = length(d);
    f -= dist;
    
    vec3 stalactites = vec3(6.0, 0.15, 6.0);
    
    f += 0.7500 * noise( stalactites * p ); p = m*p*2.05;
    f += 0.5000 * noise( p ); p = m*p*2.02;
    f += 0.2500 * noise( p ); p = m*p*2.03;
    f += 0.1250 * noise( p ); p = m*p*2.01;
    f += 0.0625 * noise( p ); 

	
    return f;
}

// Based on original by IQ.
float calculateAO(vec3 p, vec3 n){

    const float AO_SAMPLES = 5.0;
    float r = 0.0, w = 1.0, d;
    
    for (float i=1.0; i<AO_SAMPLES+1.1; i++){
        d = i/AO_SAMPLES;
        r += w*(d - cave(p + n*d));
        w *= 0.5;
    }
    
    return 1.0-clamp(r,0.0,1.0);
}


float raymarchTerrain( in vec3 ro, in vec3 rd )
{
	float maxd = 20.0;
    float t = 0.1;
    for( int i = 0; i< 160; i++ )
    {
	    float h = cave( ro + rd * t );
        if( h < (0.001 * t) || t > maxd ) break;
        t += (step(h, 1.) * .05 + 0.1) * h;
    }

    if( t>maxd ) t=-1.0;
    return t;
}

vec3 calcNormal( in vec3 pos, in float t )
{
    vec3 eps = vec3( max(0.02,0.001*t),0.0,0.0);
	return normalize( vec3(
           cave(pos+eps.xyy) - cave(pos-eps.xyy),
           cave(pos+eps.yxy) - cave(pos-eps.yxy),
           cave(pos+eps.yyx) - cave(pos-eps.yyx) ) );

}

//vec3 lig = normalize( vec3(-0.3,0.4,0.7) );
	



mat3 setCamera( in vec3 ro, in vec3 ta, float cr )
{
	vec3 cw = normalize(ta-ro);
	vec3 cp = vec3(sin(cr), cos(cr),0.0);
	vec3 cu = normalize( cross(cw,cp) );
	vec3 cv = normalize( cross(cu,cw) );
    return mat3( cu, cv, cw );
}

void main( )
{
    vec2 q = xlv_TEXCOORD0.xy / iResolution.xy;
	vec2 p = -1.0 + 2.0*q;
	p.x *= iResolution.x / iResolution.y;
	
	
    // camera	
	float off = step( 0.001, iMouse.z )*6.0*iMouse.x/iResolution.x;
	float time = off + 1.2 * iTime;
	vec3 ro = path( time+0.0 );
	vec3 ta = path( time+1.6 );
    
	//ta.y *= 0.35 + 0.25*sin(0.09*time);
	// camera2world transform
    mat3 cam = setCamera( ro, ta, 0.0 );

    // ray    
	float r2 = p.x*p.x*0.32 + p.y*p.y;
    p *= (7.0-sqrt(37.5-11.5*r2))/(r2+1.0);
    vec3 rd = cam * normalize(vec3(p.xy,2.1));

    vec3 col = vec3(0.0);
    
    // terrain	
	float t = raymarchTerrain(ro, rd);
    if( t>0.0 )
	{
		vec3 pos = ro + t*rd;
		vec3 nor = calcNormal( pos, t );
		vec3 ref = reflect( rd, nor );
	

        // lighting
		float bac = clamp( abs(dot( nor, rd)), 0.0, 1.0 );
        
        float ao = calculateAO(pos, nor);
	

		vec3 lin = ao * bac * vec3(0.7, 0.9, 1.0) / pow(t, 2.5);


        // surface shading/material	
        
		col = texcube( iChannel5, 0.5*pos, nor ).xyz;
	
		col = lin * col;
        
        // water
        if(pos.y < -1.0) {
            // blue tint
         	col.z += 0.005;
            // darken when deep
            col *= pow(0.4, pos.y * pos.y);
        }
		
    }
	

    // gamma	
	col = pow( clamp( col, 0.0, 1.0 ), vec3(0.45) );

    // contrast, desat, tint and vignetting	
	col = col*0.3 + 0.7*col*col*(3.0-2.0*col);
	col = mix( col, vec3(col.x+col.y+col.z)*0.33, 0.2 );
	col *= 1.3*vec3(1.06,1.1,1.0);
	col *= 0.5 + 0.5*pow( 16.0*q.x*q.y*(1.0-q.x)*(1.0-q.y), 0.1 );
	
	gl_FragColor = vec4( col, 1.0 );
}

