[Vertex shader]

uniform vec3 lightPos;

varying vec3 lVec;
varying vec3 norm;

void main(){
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

	lVec = lightPos - gl_Vertex.xyz;
	norm = gl_Normal;
}

[Fragment shader]

uniform sampler1D CelShade;

varying vec3 lVec;
varying vec3 norm;

void main(){
	vec3 lightVec = normalize(lVec);
	float diffuse = dot(lightVec, norm);

	gl_FragColor = texture1D(CelShade, diffuse);
}
