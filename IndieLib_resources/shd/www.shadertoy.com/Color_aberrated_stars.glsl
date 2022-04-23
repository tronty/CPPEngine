
#define hue(v) ( .6 + .6 * cos( 2.*PI*(v) + vec3(0,-2.*PI/3.,2.*PI/3.) ) )
#define PI 3.14159265359


// Settings
#define LAYER_COUNT 5.
#define ABERRATION 0.25
#define ABERRATION_SIZE 0.3
#define DOT_SIZE 0.05
float hash12(vec2 p)
{
    vec3 p3 = fract(vec3(p.xyx) * .1031);
    p3 += dot(p3, p3.yzx + 33.33);
    return fract((p3.x + p3.y) * p3.z);
}
vec3 dotLayer(vec2 uv, vec2 uvNorm, float uvLength, float fade, float layerID)
{
    vec2 gv = fract(uv) -.5;
    vec2 ID = floor(uv);
    vec3 col = vec3(0);
    for (int x = -1; x <= 1; x++)
        for (int y = -1; y <= 1; y++)        
        {
            vec2 offs = vec2(x,y);            
            float rnID = hash12(ID + offs + layerID);
            float variantID = 123.123 * rnID ;            
            vec2 rndOffs = vec2(sin(variantID), cos(variantID)) ;
            vec2 pos = gv - offs + rndOffs;
            
            float starScale = 0.5 + (0.5 * rnID);
            float dotScale = DOT_SIZE * starScale * fade;
            float abbrsize = 0.01 + (ABERRATION_SIZE * uvLength) * fade * starScale;
            float p =smoothstep(dotScale,dotScale*.5   ,length(pos));
            col += p;
            
            float aberr = ABERRATION * starScale * fade;
            float fC = 0.;
            for(int i = 0; i < 3; i++)
            {                  
                fC += 0.33333;
                float cDist =   (fC * uvLength * aberr);
                
                col[i] += pow(smoothstep(abbrsize   , 0., length(pos + (uvNorm * cDist))), 2.);
                
                
            }
            
        }
        return col * fade;
}


mat2 Rot(float a) {
    float s=sin(a), c=cos(a);
    return mat2(c, -s, s, c);
}
void main( )
{
    
    vec2 uv = (xlv_TEXCOORD0.xy -.5 * iResolution.xy) / iResolution.y;
    float uvLength = length(uv);
    uv *= Rot(iTime*0.1);
    vec2 uvNorm = normalize(uv);
    
    
    
        
    vec3 col = vec3(0);            
    
    float speed = iTime * 0.2;
    for(float i=0.; i < 1.; i+=1./LAYER_COUNT)
    {
        float depth = fract(speed+i) ;
        col += dotLayer(uv * (20. - (19. * depth)) , uvNorm, uvLength , smoothstep(1.,0.8, depth), i)  * depth;
    }
                                
    vec3 bgCol = abs(dot(uvNorm.x * uvLength, uvNorm.y))* (hue(uvLength + (iTime* 0.2)) * 0.1); 
    col += bgCol;

    gl_FragColor = vec4(col,1.0);
}

