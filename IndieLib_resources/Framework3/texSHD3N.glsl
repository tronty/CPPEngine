varying vec4 color;
varying vec2 texcoord;

[Vertex shader]
uniform mat4 worldViewProj;
void main( )
{
	vec4 v=vec4(gl_Vertex).xyz, 1);
 	gl_Position = worldViewProj * v;
    	texcoord = vec2(gl_MultiTexCoord0.xy);
	color = vec2(gl_Color);
}

[Fragment shader]
uniform sampler2D Sampler;
uniform int iSampler=0;
void main( )
{
	gl_FragColor = color;
	if(iSampler>0)
		gl_FragColor += texture2D( Sampler, texcoord );
	gl_FragColor=GammaCorrect4(gl_FragColor);
}

