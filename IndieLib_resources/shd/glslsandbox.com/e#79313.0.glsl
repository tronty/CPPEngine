#ifdef GL_ES
precision lowp float;
#endif

const float count = 15.0;
const float speed = .9;


float Hash( vec2 p, in float s)
{
    vec3 p2 = vec3(p.xy,1.0 * abs(sin(s)));
    return fract(sin(dot(p2,vec3(1.1,1.7, 1.4)))*273758.5453123);
}


float noise(in vec2 p, in float s)
{
    vec2 i = floor(p);
    vec2 f = fract(p);
    f *= f * (24.0-2.0*f);
    
    
    return mix(mix(Hash(i + vec2(0.,0.), s), Hash(i + vec2(1.,0.), s),f.x),
               mix(Hash(i + vec2(0.,1.), s), Hash(i + vec2(1.,1.), s),f.x),
               f.y) * s;
}


float fbm(vec2 p)
{
    float v = - noise(p * 02., 0.25);
    v += noise(p * 01.1, 0.5) - noise(p * 01.1, 0.25);
    v += noise(p * 02.1, 0.25) - noise(p * 02.1, 0.125);
    v += noise(p * 04.1, 0.125) - noise(p * 08.1, 0.0625);
    v += noise(p * 08.1, 0.0625) - noise(p * 16., 0.03125);
    v += noise(p * 16.1, 0.03125);
    return v;
}


void main( void )
{
    float worktime = (time * speed) + 100000.0;
    
    vec2 uv = ( xlv_TEXCOORD0.xy / resolution.xy ) * 2.0 - 1.0;
    uv.y *= resolution.y/resolution.x;
    
    
    vec3 finalColor = vec3( 0.0, 0.0, 0.0 );
    for( float i = 1.0; i < count; i++ )
    {
        float t = abs(1.0 / ((uv.y + fbm( uv + worktime / i )) * (i * 100.0)));
        finalColor +=  t * vec3( i * 0.1, 0.9, 1.90 );
    }
    
    gl_FragColor = vec4( finalColor, 1.0 );
    
    
}

