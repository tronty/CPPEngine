vec3 gold = vec3(.9, .7, 0.);

float sdCircle(vec2 p, float r)
{
    return length(p) - r;
}

void main( )
{
    vec2 frag = xlv_TEXCOORD0.xy;
    vec2 uv = frag/iResolution.xy * 2. - 1.;
    uv.x *= iResolution.x / iResolution.y;
    float size = 6.;
    uv *= size;
    
    vec2 i_uv = floor(uv);
    if (mod(i_uv.x, 2.) != 0.)
    	uv.y += .5;
    
    vec2 f_uv = fract(uv) * 2. - 1.; 
    
    // Light source as the mouse
    vec2 ms = vec2(1.);
    if (iMouse.z > 0.)
    {
        ms = (iMouse.xy/iResolution.xy) * 2. - 1.;
        ms.x *= iResolution.x / iResolution.y;
        ms *= size;
    }
    
    float c = 1.-sdCircle(f_uv, .86);
    float m = 1.-sdCircle(uv-ms, 1.); // Radial light
    float sten = smoothstep(.9, .96, c);
 
    vec3 fragPos = vec3(f_uv*2., c);
    vec3 nor = normalize(fragPos);
    
    vec3 eye = vec3(0., 0., 6.6);
    vec3 refEye = normalize(reflect(eye, nor));
    
    // Diffuse lightning model
    vec3 lightPos = vec3(ms, size);
    vec3 lightDir = normalize(lightPos - vec3(uv, 0.));
    float al = c*.1;
    float dl = max(dot(lightDir, nor), 0.)*.25;
    vec3 eyeDir = normalize(eye - fragPos);
    vec3 halfWayDir = normalize(lightDir + eyeDir);
    float sl = pow(max(dot(halfWayDir,nor), 0.),64.);
    float l = al + dl; // White light
    
    // Time varying pixel color
    vec3 col = vec3(0.);
   //col += texture2D(iChannel0, refEye).rgb*sten*gold;
    col += l*sten*gold;
    col += sl*2.*sten;
    col += vec3(0.1)*m; // Mouse light

    // Output to screen
    gl_FragColor = vec4(col,1.0);
}
