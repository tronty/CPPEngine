uniform sampler2D iChannel0;

//Tweet: https://twitter.com/XorDev/status/1505705176405061638
//Twigl: https://t.co/eHRjwY1S4i

//Explanation of techniques: https://www.shadertoy.com/view/fstSRH

//Sample Noise texture
#define T texture(iChannel0
//Sample map (approximate SDF)
#define M (T,a/180.).r+.3-length(a.xy)*.1)

void main()
{
    vec3 b = iResolution, //Resolution
    p = iTime/b/b/.1,     //Camera position
    a = b-b,              //Cell coords
    d = b.xxz;            //Reciprocal of ray direction
    
    for(d.xy /= b.xy-xlv_TEXCOORD0-xlv_TEXCOORD0; //Compute reciprocal
        b.z++<1e3 && M>.3;//End condition
                          //Step forward on voxel:
    a = ceil(p += min(a = fract(-abs(p))*abs(d)+3e-4,min(a.y,a.z)).x/d));
                          //Offset and sample map again to determine color
                          //Use SDF for cheap AO and multiply with texture
                          //Divide by distance for fog
    a--; gl_FragColor = vec4(M>.3?9.+b-b:vec3(9,6,4),a=p+.5)*M*M*T,ceil(p*8.)/16.).r/(p.z*.1-iTime+.5);
}
