[Vertex shader]
	uniform mat4 worldViewProj;
	void main()
	{
		gl_Position = worldViewProj * gl_Vertex;
	}

[Fragment shader]
	void main(void)
	{
		gl_FragColor = vec4(1.0, 1.0, 0.0, 1.0);
	}

[Geometry shader]
	//#version 330 core
	layout (triangles) in;
	layout (triangle_strip, max_vertices = 3) out;

	in VS_OUT {
		vec2 texCoord;
	} gs_in[];

	out VS_OUT {
		vec2 texCoord;
		vec3 viewVectorTangent;
		vec3 lightVectorTangent;
	} gs_out[];
//--------------------------------------------------------------------------------------
// Calculate vertex positions for procedural sphere mesh based on an input index buffer
//--------------------------------------------------------------------------------------
	void main()
	{   
		float meridianPart = ( input.index % ( g_SphereMeridianSlices + 1 ) ) / float( g_SphereMeridianSlices );
		float parallelPart = ( input.index / ( g_SphereMeridianSlices + 1 ) ) / float( g_SphereParallelSlices );
		
		float angle1 = meridianPart * 3.14159265 * 2.0;
		float angle2 = ( parallelPart - 0.5 ) * 3.14159265;
		
		float cos_angle1 = cos( angle1 );
		float sin_angle1 = sin( angle1 );
		float cos_angle2 = cos( angle2 );
		float sin_angle2 = sin( angle2 );
				
		vec3 VertexPosition;
		VertexPosition.z = cos_angle1 * cos_angle2;
		VertexPosition.x = sin_angle1 * cos_angle2;
		VertexPosition.y = sin_angle2;
		
		gl_Position = vec4( VertexPosition, 1.0 )   g_ModelViewProj;
		//TexCoord = gs_in[0].texCoord;
		texCoord = vec2( 1.0 - meridianPart, 1.0 - parallelPart );
		
		vec3 tangent = vec3( cos_angle1, 0.0, -sin_angle1 );
		vec3 binormal = vec3( -sin_angle1 * sin_angle2, cos_angle2, -cos_angle1 * sin_angle2 );
				
		vec3 viewVector = normalize(g_EyePosition - VertexPosition);
		
		viewVectorTangent.x = dot( viewVector, tangent );
		viewVectorTangent.y = dot( viewVector, binormal);
		viewVectorTangent.z = dot( viewVector, VertexPosition );
		
		vec3 lightVector = normalize( g_LightPosition );
		
		lightVectorTangent.x = dot( lightVector, tangent );
		lightVectorTangent.y = dot( lightVector, binormal);
		lightVectorTangent.z = dot( lightVector, VertexPosition );
		EmitVertex();
		EndPrimitive();
	}

