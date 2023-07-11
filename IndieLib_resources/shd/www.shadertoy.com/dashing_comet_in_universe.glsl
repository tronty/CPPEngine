/*original  https://www.shadertoy.com/view/MdXSzS */
void main( )
{
	vec2 uv = (xlv_TEXCOORD0.xy / iResolution.xy) - .5;
	float t = iTime * .1 + ((.25 + .00 * sin(iTime * .1))/(length(uv.xy) + 5.01)) * 0.0;
	float si = sin(t);
	float co = cos(t);
	mat2 ma = mat2(co, si, -si, co);
  
	float v1, v2, v3, v4;
	v1 = v2 = v3 = v4 = 0.0;
	
	float s = 0.0;
	for (int i = 0; i < 150; i++)
	{
      vec2 trs = vec2(cos(iTime*1.0),sin(iTime*1.2));

		vec3 p = s * vec3(uv+trs, 2.5*cos(iTime*0.5));
		p.xy *= ma;
		p += vec3(.22, .3, s - 1.5 * sin(iTime * .5) * .1);
		for (int i = 0; i < 8; i++)	p = abs(p) / dot(p,p) - 0.659;
		v1 += dot(p,p) * .0015 * (.8 + sin(length(uv.xy * 13.0) + .5  - iTime * .2));
		v2 += dot(p,p) * .0113 * (1.5 + sin(length(uv.xy * 14.5) + 1.2 - iTime * 1.3));
        
		v3 += length(p.xy*10.) * .0003;
        v4 += dot(p,p) * .0013 * (0.5 + length(uv.xy * 0.5) + 1.2 );
		s  += .035;
	}
	
	float len = length(uv);
	v1 *= smoothstep(.7, .1, len);
	v2 *= smoothstep(.5, .20, len);
	v3 *= smoothstep(.9, .0, len);
	v4*=v3 *= smoothstep(.3, .0, len);
     
	vec3 col = vec3( v3 * (cos(iTime)*2.5 + fract(sin(iTime * 1.5)) * 1.0),
					(v1 + v3) * 0.5,
					 v2) + smoothstep(0.3, .0, len) * .85 + smoothstep(cos(iTime)*2.5, .6, v3) * 4.3*fract(v4);

	gl_FragColor=vec4(min(pow(abs(col), vec3(1.2)), 1.0), 1.0);
}

