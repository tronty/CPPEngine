//https://iquilezles.org/articles/distfunctions2d/
#define PI 3.14159265359

mat3 rotateX(float f)
{
    return mat3(
    vec3(1.0,    0.0,      0.0),
    vec3(0.0,	 cos(f),  -sin(f)), 	
	vec3(.0, sin(f), cos(f))
    );
}


mat3 rotateY(float f)
{
    return mat3(
    vec3(cos(f), 0.0,  sin(f)),
    vec3(0.0,	 1.0,  0.0), 	
	vec3(-sin(f), 0.0, cos(f))
    );
}

mat3 rotateZ(float f)
{
    return mat3(
    vec3(cos(f),    -sin(f),  0.0),
    vec3(sin(f),	 cos(f),  0.0), 	
	vec3(0.0, 0.0, 1.0)
    );
    
}

float aafi(vec2 p)
{
    float l = length(p);
    float fi = asin(abs(p.y)/l);
    if (p.y > 0.0 && p.x < 0.0)
        fi = PI - fi;
    if (p.y < 0.0 && p.x < 0.0)
        fi = fi + PI;    
    if (p.y < 0.0 && p.x > 0.0)
        fi = -fi + 2.0 * PI;   
    return fi;    
}

//converts a vector on a sphere to longitude and latitude

vec2 lonlat (vec3 p)
{
    float lon = aafi(p.xy)/2.0/PI;
    float lat = aafi(vec2(p.z, length(p.xy)))/PI;
    return vec2(1.0-lon, lat);
}

//=============================2D pic=========================
float sdBuild(vec2 p, float[30] f, float[30] r, int n)
{
   //float fi = atan(p.y, p.x);
   
   
   float l = length(p);
    float fi = asin(abs(p.y)/l);
    if (p.y > 0.0 && p.x < 0.0)
        fi = PI - fi;
    if (p.y < 0.0 && p.x < 0.0)
        fi = fi + PI;    
    if (p.y < 0.0 && p.x > 0.0)
        fi = -fi + 2.0 * PI;    
   
    
    for (int i = 1; i < 30; i++)
    {
        if (i == n)
            break;
        if (fi >= f[i-1] && fi <= f[i])
        {
            float f1 = fi - f[i-1];
            float f2 = f[i] - fi;
            float h1 = r[i-1] * sin(f1);
            float h2 = r[i] * sin(f2);
            vec2 v1 = vec2(r[i-1] * cos(f[i-1]), r[i-1] * sin(f[i-1]));
            vec2 v2 = vec2(r[i] * cos(f[i]), r[i] * sin(f[i]));
            vec2 v3 = v2 - v1;
            v3 *= h1 / (h1 + h2);
            vec2 v4 = v1 + v3;
            return length(p) - length(v4);
        }
    }
    
}

vec3 grid(vec2 uv)
{
        float rombr[30]; 
        rombr[0] = 1.0; 
        rombr[1] = 1.0; 
        rombr[2] = 1.0; 
        rombr[3] = 1.0; 
        rombr[4] = 1.0; 

        float rombf[30]; 
        rombf[0] = 0.0; 
        rombf[1] = PI/2.0;
        rombf[2] = PI;
        rombf[3] = 3.0*PI/2.0;
        rombf[4] = 2.0*PI;

    
        vec2 p =  vec2(fract(uv.x*20.0), fract(uv.y*10.0));
        p = (p-0.5)*2.0;
        
        vec3 col = vec3(1.0);
        float pst = sdBuild(p, rombf, rombr, 5);
        col = mix(col, vec3(1.0, 1.0, 0.0), step(pst, 0.0));
        for (int i = 0; i < 5; i++)
        {
            rombr[i] = 0.8;
        }
        pst = sdBuild(p, rombf, rombr, 5);
        col = mix(col, vec3(0.0, 0.0, 1.0), step(pst, 0.0));

        for (int i = 0; i < 5; i++)
        {
            rombr[i] = 0.6;
        }
        pst = sdBuild(p, rombf, rombr, 5);
        col = mix(col, vec3(1.0, 0.0, 0.0), step(pst, 0.0));

        for (int i = 0; i < 5; i++)
        {
            rombr[i] = 0.4;
        }
        pst = sdBuild(p, rombf, rombr, 5);
        col = mix(col, vec3(0.0, 1.0, 0.0), step(pst, 0.0));

        return col;
}
//=============================2D pic=========================

void main( )
{
    float r = 0.7;
    vec3 col = vec3(0.7, 0.7, 0.9);
    vec3 light = vec3(-10.0, 0.0, 10.0);
    
    vec2 p = (2.0*xlv_TEXCOORD0-iResolution.xy)/iResolution.y;
    float l = length(p);
    if (l > r ) 
    {
         gl_FragColor = vec4(col, 1.0);
         return;
    }
    //get vector on sphere in Orthographic Camera
    float z = r*sin(acos(l/r));
    vec3 sp = vec3(p, z);


    vec3 sp_rot = rotateZ(-iTime)*rotateX(PI/2.0 + PI/5.0)*sp; //rotate sphere
    vec2 pt = lonlat(sp_rot); //get longitude and latitude
    //col = texture(iChannel0, pt).rgb;  //get color from texture
    col = grid(pt);
    
    //Lambertian
    float diffuse = dot(normalize(sp), normalize(light));
    diffuse = clamp(diffuse, 0.3, 1.0);
    col *= diffuse;

    // gamma
    col = pow( col, vec3(0.4545) );

    gl_FragColor = vec4(col, 1.0);
    
}

