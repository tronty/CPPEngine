uniform sampler2D iChannel0;

void main( )
{
	vec2 uv = xlv_TEXCOORD0.xy / iResolution.xy;
	gl_FragColor = texture(iChannel0, uv);
    
    vec3 rd = texture(iChannel0, vec2(0.5)).rgb;
    
    float res = .4;
    float steps = 3.14159*2.;
    float focus = 3.;
 	float depth = gl_FragColor.a;
    
    float dist = smoothstep(0.0, 2., depth-focus)*3.*dFdx(uv.x);
    vec3 tcol = vec3(.0);
    for (float i = 0.; i < steps; i = i + res)
    {
        vec2 _uv = uv+vec2(cos(i), sin(i))*dist;
        tcol += texture(iChannel0, _uv).rgb;    
    }

    gl_FragColor.rgb = tcol/(steps/res);
    gl_FragColor.rgb = smoothstep(0.0, 1.0, gl_FragColor.rgb); // contrast
    gl_FragColor.rgb = pow(gl_FragColor.rgb, vec3(1.0 / 2.2));
}
