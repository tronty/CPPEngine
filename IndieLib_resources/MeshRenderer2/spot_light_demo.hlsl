<?xml version="1.0" encoding="ISO-8859-1" standalone="yes"?>
<!--
==============================================================================
FILE		: spot_light_demo.xml
AUTHOR		: JeGX
DATE		: 20.01.2006
PURPOSE		: Phong lighting in GLSL - Spot light.
==============================================================================
-->


<hyperion version="1.10">


<scene name= "Phong_Lighting_Demo"  display_fps="TRUE" vsync="TRUE" show_ref_grid="FALSE" >
	<window_size width="1024" height="768" />
	<global_ambient_light r="0.1" g="0.1" b="0.1" a="1.0" />
	<background_color r="0.0" g="0.0" b="0.0" />
	<check_hardware_caps glsl="TRUE" />
</scene>



<light name="Light_01" render="TRUE" 
       type="SPOT" spot_angle="20.0" display_tripod="TRUE"  >
	<position x="0.0" y="150.0" z="150.0" />
	<orientation pitch="30.0" yaw="180.0" />
	<ambient r="0.0" g="0.0" b="0.0" a="1.0" />
	<diffuse r="1.0" g="1.0" b="1.0" a="1.0" />
	<specular r="1.0" g="1.0" b="1.0" a="1.0" />
</light>



<camera name="Main_Camera" navigation_mode="FLY" >
	<position x="30.0" y="100.0" z="200.0" />
</camera>



<!--
FLOOR.
-->
<material name="floor_mat" shader_program_name="SpotLightShader"
	shader_program_name_for_ambient_pass="AmbientPassShader" >
	<ambient r="0.7" g="0.7" b="0.7" a="1.0" />
	<diffuse r="0.4" g="0.9" b="0.4" a="1.0" />
	<specular r="0.4" g="0.4" b="0.4" a="1.0" exp="60.0" />
</material>

<mesh name="floor" shape_type="PLANE" lighting="TRUE" texturing="FALSE" polygon_mode="SOLID" use_vbo="TRUE" >
	<plane x_size="500.0" z_size="500.0" num_segs_x="20" num_segs_z="20" />
	<attach_material name="floor_mat" />
</mesh>



<!--
TORUS.
-->
<material name="mat_torus" shader_program_name="SpotLightShader" >
	<ambient r="0.3" g="0.3" b="0.3" a="1.0" />
	<diffuse r="0.9" g="0.5" b="0.5" a="1.0" />
	<specular r="0.6" g="0.6" b="0.6" a="1.0" exp="60.0" />
</material>

<mesh render="TRUE" name="torus" shape_type="TORUS" lighting="TRUE" use_vbo="TRUE"
      improve_specular_highlights="TRUE" shadow_caster="TRUE" remove_seam="TRUE" >
	<torus vertex_density="50"/>
	<position x="-70.0" y="50.0" z="0.0" />
	<attach_material name="mat_torus" /> 
</mesh>



<!--
SPHERE.
-->
<material name="mat_sphere" shader_program_name="SpotLightShader" >

	<ambient r="0.3" g="0.3" b="0.3" a="1.0" />
	<diffuse r="0.5" g="0.5" b="0.9" a="1.0" />
	<specular r="0.4" g="0.4" b="0.4" a="1.0" exp="20.0" />
</material>

<mesh render="TRUE" name="sphere" shape_type="SPHERE" lighting="TRUE" use_vbo="TRUE"
      improve_specular_highlights="TRUE" shadow_caster="TRUE" remove_seam="TRUE"  >
	<sphere radius="40.0" stacks="40" slices="40"/>
	<position x="70.0" y="40.0" z="0.0" />
	<attach_material name="mat_sphere" /> 
</mesh>




<!--
The GLSL shader for the ambient pass.
Only with shadow-volumes.
-->
<shader_program name="AmbientPassShader"  >
	
	<raw_data><![CDATA[	 
		[Vertex_Shader]
		
		void main(void)
		{
			gl_Position = ftransform();
		}
		
		[Pixel_Shader]
		
		void main (void)
		{
			gl_FragColor = gl_FrontLightModelProduct.sceneColor * gl_FrontMaterial.ambient;	
		}
	   	]]></raw_data>

</shader_program>


<shader_program name="SpotLightShader"  >
	
	<raw_data><![CDATA[	 
	
		[Vertex_Shader]
		
		varying vec3 normal, lightDir, eyeVec;

		void main()
		{	
			normal = gl_NormalMatrix * gl_Normal;
		
			vec3 vVertex = vec3(gl_ModelViewMatrix * gl_Vertex);
			lightDir = vec3(gl_LightSource[0].position.xyz - vVertex);
			eyeVec = -vVertex;
	
			gl_Position = ftransform();		
		}
		[Pixel_Shader]
		
		varying vec3 normal, lightDir, eyeVec;
		
		void main (void)
		{
			vec4 final_color = (gl_FrontLightModelProduct.sceneColor * gl_FrontMaterial.ambient) + 
			(gl_LightSource[0].ambient * gl_FrontMaterial.ambient);
			
									
			vec3 L = normalize(lightDir);
			vec3 D = normalize(gl_LightSource[0].spotDirection);
			
			if (dot(-L, D) > gl_LightSource[0].spotCosCutoff) 
			{
				vec3 N = normalize(normal);
			
				float lambertTerm = max( dot(N,L), 0.0);
				if(lambertTerm > 0.0)
				{
					final_color += gl_LightSource[0].diffuse * gl_FrontMaterial.diffuse * lambertTerm;	
				
					vec3 E = normalize(eyeVec);
					vec3 R = reflect(-L, N);
					float specular = pow( max(dot(R, E), 0.0), gl_FrontMaterial.shininess );
					final_color += gl_LightSource[0].specular * gl_FrontMaterial.specular * specular;	
				}
			}
	
			gl_FragColor = final_color;			
		}
	   	]]></raw_data>

</shader_program>



</hyperion>
