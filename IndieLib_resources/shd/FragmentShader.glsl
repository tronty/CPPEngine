uniform sampler2D iChannel0;
void main()
{
	gl_FragColor = texture2D(iChannel0, xlv_TEXCOORD0);
}

void main2()
{
	gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}

