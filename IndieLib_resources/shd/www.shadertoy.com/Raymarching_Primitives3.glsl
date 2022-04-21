float RoundBox(vec2 p , vec2 BoxOrigin ,float width, float height, float sharpness)
{
    p = abs(p - BoxOrigin);
    p.x += max(1. - width ,0.);
    p.y += max(1. - height,0.);
    p = pow( p , vec2(sharpness));
    float box = p.x + p.y;
    
    return max(1. - box,0.);
}

void main( )
{
	vec2 uv = xlv_TEXCOORD0/iResolution.xy;   
    uv -= 0.5;
    uv.x *= iResolution.x / iResolution.y;
    float phi = 0.61803399;
    float anim = mix(10.,70.,sin(iTime)*0.5+0.5);
    
    gl_FragColor = vec4(  RoundBox(uv,vec2(0.,0.),.5,phi * 0.5, anim)  );
}

