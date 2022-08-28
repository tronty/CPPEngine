uniform sampler2D HeightMap;
/*
    "Midnight" by @XorDev

    Variant of my "Mars" shader: https://shadertoy.com/view/flKcDW

    //Tweet: twitter.com/XorDev/status/1563001681063079936
    //Twigl: t.co/mh0wPftU4P
*/

void main()
{
    vec4 O; vec2 I=xlv_TEXCOORD0;

    //Resolution for scaling
    vec3 r = iResolution,
    //Ray direction for raymarching and sky color
    d = (vec3(I+I,r)-r)/r.x,
    //Ray origin position
    p = r-r,
    //Iteration counter
    i = p;
    
    //Loop 200 times
    for(; i.x<2e2;
        //Raymarch with heightmap
        p += d*(p.y+.34-.3*texture2D(HeightMap,(p*r+iTime*r*.3).xz/4e2).r)/r)
        //Swap octave scale every 5 iterations
        r = exp(mod(++i,5.));
    
    //Set color using d, sun, depth and heightfog
    O.grb = .03 / length(d.xy-.3) * ++d - 1./++p.z - min(p.y,0.)/.3;
	gl_FragColor=O;
}
