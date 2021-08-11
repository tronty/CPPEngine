#
# Copyright (c) 2017 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
import os, sys, re, string
import os.path
import getopt

v=['/ParticleVS/Particles.shd', 'main', 'main', '/Texture/Texture.shd', 'main', 'main', '/InfiniteTerrainII/terrain.shd', 'main', 'main', '/InfiniteTerrainII/generateTerrain.shd', 'main', 'main', '/InfiniteTerrainII/tangentSpace.shd', 'main', 'main', '/InfiniteTerrainII/skybox.shd', 'main', 'main', '/InfiniteTerrain/InfiniteTerrain.shd', 'main', 'main', '/fx_volumetric_lines/VolumeLines.fx', 'main', 'main', '/Shape3D/fireball.fx', 'RayMarchVS', 'RayMarchPS', '/Shape3D/vbomb.fx', 'mainVS', 'hotPS', '/Shape3D/Flame.fx', 'flameVS', 'flamePS', '/Orcs/PerlinFire.fx', 'PerlinFireVS', 'PerlinFire3DPS', '/Orcs/PerlinFire.fx', 'PerlinFireVS', 'PerlinFire3DFlowPS', '/Orcs/PerlinFire.fx', 'PerlinFireVS', 'PerlinFire4DPS', '/DeferredParticles/DeferredParticles.fx', 'RenderParticlesVS', 'RenderParticlesDeferredPS', '/DeferredParticles/DeferredParticles.fx', 'RenderParticlesVS', 'RenderParticlesPS', '/DeferredParticles/DeferredParticles.fx', 'CompositeParticlesVS', 'CompositeParticlesPS', '/SimpleTexture/SimpleTexture3.shd', 'main', 'main', '/SimpleTexture/SimpleTexture2.shd', 'main', 'main', '/SimpleTexture/SimpleTexture.shd', 'main', 'main', 'LitVertex11.shd', 'mainVS', 'mainPS', '/RadeonTerrainDemo/LitVertex.shd', 'mainVS', 'mainPS', '/17_Racing_Track/trackEffect.fx', 'main', 'main', '/17_Racing_Track/Effect.fx', 'main', 'main', '/17_Racing_Track/Effect.fx', 'main', 'main', '/12_Explosion_Particle_Effect/explosionEffect.fx', 'main', 'main', '/cube/cube.shd', 'main', 'main', '/Stars/Stars3.shd', 'main', 'main', '/Stars/Stars4.shd', 'main', 'main', '/DDSAnimation/DDSAnimation.shd', 'main', 'main', '/libnoise/rendersample.shd', 'main', 'main', '/R2VB-Sort/smoke.shd', 'main', 'main', '/R2VB-Sort/distance.shd', 'main', 'main', '/R2VB-Sort/bitonicSort.shd', 'main', 'main', '/R2VB-Sort/makeQuads.shd', 'main', 'main', '/R2VB-Sort/initSmoke.shd', 'main', 'main', '/R2VB-Sort/particles.shd', 'main', 'main', '/AccurateAtmosphericalScattering/SkyFromSpace.shd', 'main', 'main', '/AccurateAtmosphericalScattering/SkyFromAtmosphere.shd', 'main', 'main', '/AccurateAtmosphericalScattering/GroundFromSpace.shd', 'main', 'main', '/AccurateAtmosphericalScattering/GroundFromAtmosphere.shd', 'main', 'main', '/AccurateAtmosphericalScattering/SpaceFromSpace.shd', 'main', 'main', '/AccurateAtmosphericalScattering/SpaceFromAtmosphere.shd', 'main', '/DeferredParticles/DeferredParticles.shd', 'RenderParticlesVS', 'RenderParticlesDeferredPS', '/DeferredParticles/DeferredParticles.shd', 'RenderParticlesVS', 'RenderParticlesPS', '/DeferredParticles/DeferredParticles.shd', 'CompositeParticlesVS', 'CompositeParticlesPS', '/DeferredParticles/DeferredParticles.shd', 'MeshVS', 'MeshPS', '/DeferredParticles/DeferredParticles.shd', 'MeshInstVS', 'MeshPS', '/SkyBox/skybox2.shd', 'main', 'main', '/SkyBox/skybox.shd', 'main', 'main', '/Billboard/billboard.shd', 'main', 'main', '/RadeonTerrainDemo/LitVertex.shd', 'mainVS', 'mainPS', '/DDSAnimation/DDSAnimation.shd', 'main', 'main', '/cg_explosion/cg_disc_shock.cg', 'main', 'main', '/cg_explosion/cg_Volumelines.cg', 'main', 'main', '/cg_explosion/cg_disc.cg', 'main', 'main', '/cg_explosion/cg_explosion.cg', 'main', 'main', '/cg_ShinySprite/cg_ShinySprite.cg', 'main', 'main', '/cg_Volumelines/cg_Volumelines.cg', 'main', 'main', '/Shape3D/vbomb.fx', 'mainVS', 'hotPS', '/Orcs/PerlinFire.fx', 'PerlinFireVS', 'PerlinFire3DPS', '/Orcs/PerlinFire.fx', 'PerlinFireVS', 'PerlinFire3DFlowPS', '/Orcs/PerlinFire.fx', 'PerlinFireVS', 'PerlinFire4DPS', '/Orcs/PerlinFire.fx', 'SimpleVS', 'SimplePS', '/Orcs/PerlinFire.fx', 'ShadowVS', '', '/15_Ocean_Water/oceanwater.fx', 'main', 'main', '/NatureScene/WaterShader.shd', 'main', 'main', '/NatureScene/SkyShader.shd', 'main', 'main', '/NatureScene/TerrainShader.shd', 'main', 'main', '/NatureScene/GrassShader.shd', 'main', 'main', '/NatureScene/WaterShader.shd', 'main', 'main', '/NatureScene/SkyShader.shd', 'main', 'main', '/Digital_Clock/digital_clock.shd', 'main', 'main', '/AMM-3DEngine/BasicColor.shd', 'main', 'main', '/otldata/shaders/parallax.shd', 'main', 'main', '/otldata/shaders/normals.shd', 'main', 'main', '/RadeonTerrainDemo/LitVertex.shd', 'mainVS', 'mainPS', '/ParticleVS/Particles.shd', 'main', 'main', '/Texture/Texture.shd', 'main', 'main', '/InfiniteTerrainII/terrain.shd', 'main', 'main', '/InfiniteTerrainII/generateTerrain.shd', 'main', 'main', '/InfiniteTerrainII/tangentSpace.shd', 'main', 'main', '/InfiniteTerrainII/skybox.shd', 'main', 'main', '/InfiniteTerrain/InfiniteTerrain.shd', 'main', 'main', '/fx_volumetric_lines/VolumeLines.fx', 'main', 'main', '/Shape3D/fireball.fx', 'RayMarchVS', 'RayMarchPS', '/Shape3D/vbomb.fx', 'mainVS', 'hotPS', '/Shape3D/Flame.fx', 'flameVS', 'flamePS', '/Orcs/PerlinFire.fx', 'PerlinFireVS', 'PerlinFire3DPS', '/Orcs/PerlinFire.fx', 'PerlinFireVS', 'PerlinFire3DFlowPS', '/Orcs/PerlinFire.fx', 'PerlinFireVS', 'PerlinFire4DPS', '/DeferredParticles/DeferredParticles.fx', 'RenderParticlesVS', 'RenderParticlesDeferredPS', '/DeferredParticles/DeferredParticles.fx', 'RenderParticlesVS', 'RenderParticlesPS', '/DeferredParticles/DeferredParticles.fx', 'CompositeParticlesVS', 'CompositeParticlesPS', '/SimpleTexture/SimpleTexture3.shd', 'main', 'main', '/SimpleTexture/SimpleTexture2.shd', 'main', 'main', '/SimpleTexture/SimpleTexture.shd', 'main', 'main', '/RadeonTerrainDemo/LitVertex.shd', 'mainVS', 'mainPS', '/17_Racing_Track/trackEffect.fx', 'main', 'main', '/17_Racing_Track/Effect.fx', 'main', 'main', '/17_Racing_Track/Effect.fx', 'main', 'main', '/12_Explosion_Particle_Effect/explosionEffect.fx', 'main', 'main', '/cube/cube.shd', 'main', 'main', '/Stars/Stars3.shd', 'main', 'main', '/Stars/Stars4.shd', 'main', 'main', '/DDSAnimation/DDSAnimation.shd', 'main', 'main', '/libnoise/rendersample.shd', 'main', 'main', 'smoke.shd', 'main', 'main', 'distance.shd', 'main', 'main', 'bitonicSort.shd', 'main', 'main', 'makeQuads.shd', 'main', 'main', 'initSmoke.shd', 'main', 'main', 'particles.shd', 'main', 'main', '/AccurateAtmosphericalScattering/SkyFromSpace.shd', 'main', 'main', '/AccurateAtmosphericalScattering/SkyFromAtmosphere.shd', 'main', 'main', '/AccurateAtmosphericalScattering/GroundFromSpace.shd', 'main', 'main', '/AccurateAtmosphericalScattering/GroundFromAtmosphere.shd', 'main', 'main', '/AccurateAtmosphericalScattering/SpaceFromSpace.shd', 'main', 'main', '/AccurateAtmosphericalScattering/SpaceFromAtmosphere.shd', 'main', 'main', '/DeferredParticles/DeferredParticles.shd', 'RenderParticlesVS', 'RenderParticlesDeferredPS', '/DeferredParticles/DeferredParticles.shd', 'RenderParticlesVS', 'RenderParticlesPS', '/DeferredParticles/DeferredParticles.shd', 'CompositeParticlesVS', 'CompositeParticlesPS', '/DeferredParticles/DeferredParticles.shd', 'MeshVS', 'MeshPS', '/DeferredParticles/DeferredParticles.shd', 'MeshInstVS', 'MeshPS', '/SkyBox/skybox2.shd', 'main', 'main', '/SkyBox/skybox.shd', 'main', 'main', '/Billboard/billboard.shd', 'main', 'main', '/RadeonTerrainDemo/LitVertex.shd', 'mainVS', 'mainPS', '/DDSAnimation/DDSAnimation.shd', 'main', 'main', '/AccurateAtmosphericalScattering/SkyFromSpace.shd', 'main', 'main', '/AccurateAtmosphericalScattering/SkyFromAtmosphere.shd', 'main', 'main', '/AccurateAtmosphericalScattering/GroundFromSpace.shd', 'main', 'main', '/AccurateAtmosphericalScattering/GroundFromAtmosphere.shd', 'main', 'main', '/AccurateAtmosphericalScattering/SpaceFromSpace.shd', 'main', 'main', '/AccurateAtmosphericalScattering/SpaceFromAtmosphere.shd', 'main', 'main', '/cg_explosion/cg_disc_shock.cg', 'main', 'main', '/cg_explosion/cg_Volumelines.cg', 'main', 'main', '/cg_explosion/cg_disc.cg', 'main', 'main', '/cg_explosion/cg_explosion.cg', 'main', 'main', '/cg_ShinySprite/cg_ShinySprite.cg', 'main', 'main', '/cg_Volumelines/cg_Volumelines.cg', 'main', 'main', '/Shape3D/vbomb.fx', 'mainVS', 'hotPS', '/HLSL-noise/HLSL-noise1.shd', 'mainVS', 'fragmentShaderPNoise', '/HLSL-noise/HLSL-noise2.shd', 'mainVS', 'fragmentShaderPNoise', '/HLSL-noise/HLSL-noise3.shd', 'mainVS', 'fragmentShaderPNoise', '/HLSL-noise/HLSL-noise4.shd', 'mainVS', 'fragmentShaderPNoise', '/HLSL-noise/HLSL-noise5.shd', 'mainVS', 'fragmentShaderPNoise', '/Orcs/PerlinFire.fx', 'PerlinFireVS', 'PerlinFire3DPS', '/Orcs/PerlinFire.fx', 'PerlinFireVS', 'PerlinFire3DFlowPS', '/Orcs/PerlinFire.fx', 'PerlinFireVS', 'PerlinFire4DPS', '/Orcs/PerlinFire.fx', 'SimpleVS', 'SimplePS', '/Orcs/PerlinFire.fx', 'ShadowVS', '', '/15_Ocean_Water/oceanwater.fx', 'main', 'main', '/NatureScene/WaterShader.shd', 'main', 'main', '/NatureScene/SkyShader.shd', 'main', 'main', '/NatureScene/TerrainShader.shd', 'main', 'main', '/NatureScene/GrassShader.shd', 'main', 'main', '/NatureScene/WaterShader.shd', 'main', 'main', '/NatureScene/SkyShader.shd', 'main', 'main', '/Digital_Clock/digital_clock.shd', 'main', 'main', '/AMM-3DEngine/BasicColor.shd', 'main', 'main', '/otldata/shaders/parallax.shd', 'main', 'main', '/otldata/shaders/normals.shd', 'main', 'main', '/RadeonTerrainDemo/LitVertex.shd', 'mainVS', 'mainPS']
#addShaderFromFile(str, 'VSBasic', 'PSTerrain', 
#addShaderFromFile(str, 'VSBasic', 'PSTerrainBump', 
#addShaderFromFile(str, 'VSBasic', 'PSTerrain', 
#addShaderFromFile(file, 'main', 'main', 
#g_hRenderTileDiff = g_pApp->m_pRenderer->addShaderFromFile(str, 'VSBasic', 'PSTerrain', 
#g_hRenderTileBump = g_pApp->m_pRenderer->addShaderFromFile(str, 'VSBasic', 'PSTerrainBump', 
#g_hRenderTileWire = g_pApp->m_pRenderer->addShaderFromFile(str, 'VSBasic', 'PSTerrain',
#otl/src/renderer/shader_cg.cpp:  idfile, 'main', 'main',
'''
typedef enum _D3DDECLUSAGE
{
    D3DDECLUSAGE_POSITION = 0,
    D3DDECLUSAGE_BLENDWEIGHT,   // 1
    D3DDECLUSAGE_BLENDINDICES,  // 2
    D3DDECLUSAGE_NORMAL,        // 3
    D3DDECLUSAGE_PSIZE,         // 4
    D3DDECLUSAGE_TEXCOORD,      // 5
    D3DDECLUSAGE_TANGENT,       // 6
    D3DDECLUSAGE_BINORMAL,      // 7
    D3DDECLUSAGE_TESSFACTOR,    // 8
    D3DDECLUSAGE_POSITIONT,     // 9
    D3DDECLUSAGE_COLOR,         // 10
    D3DDECLUSAGE_FOG,           // 11
    D3DDECLUSAGE_DEPTH,         // 12
    D3DDECLUSAGE_SAMPLE,        // 13
} D3DDECLUSAGE;
'''
'''
technique10 VolumeRenderer
{
    pass CompRayData_Back
    {
        SetVertexShader(CompileShader( vs_4_0, VS_RAYDATA_BACK() ));
        SetGeometryShader ( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_RAYDATA_BACK() ));
        SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetRasterizerState(CullFront);
        SetDepthStencilState( DisableDepth, 0 );
    }
'''

debug=False
iii_=1

def load(file):
    xml_file = os.path.abspath(file)
    f=open(xml_file,'r')
    lines=f.readlines()
    f.close()
    lXmlAsString=string.join(lines,'')
    return lXmlAsString

def finduniforms(txt):
	cl2=r'\buniform\b\s+(\w+)\s+(\w+\b)'
	return re.findall(cl2,txt,re.S|re.M)

def findstructs(txt):
	cl2=r'\bstruct\b\s+(\b\w+\b)'
	return re.findall(cl2,txt,re.S|re.M)

def findColorField(txt, structname):
	cl=r'\bstruct\b\s+%s\s*\{.*?\b\w+\s+(\w+)\s*\:\s*COLOR0\b' % (structname)
	p = re.compile(cl,re.S|re.M)
	v1=p.findall(txt)
	if len(v1):
		#print '1:',v1[0]
		return '.'+v1[0]
	cl=r'\bstruct\b\s+%s\s*\{.*?\b\w+\s+(\w+)\s*\:\s*COLOR\b' % (structname)
	p = re.compile(cl,re.S|re.M)
	v2=p.findall(txt)
	if len(v2):
		#print '2:',v2[0]
		return '.'+v2[0]
	return ''

def findmain_(txt, type, main):
	if debug:
		print type, main
	mainargstypes=[]
	mainargsnames=[]
	uniforms=[]
	maintype=''
	mainname=''
	outvar=''
	cl=r'\s+%s\s+%s\s*\(.*?\)\s*\{\s*%s\s+(\w+)' % (type,main,type)
	m = re.search(cl,txt,re.S|re.M)
	if m:
		outvar=m.group(1)
	cl=r'\s+(%s)\s+(%s)\s*\((.*?)\)' % (type,main)

	m = re.search(cl,txt,re.S|re.M)
	if m and (m.groups()>2):
		mainargs=m.group(3).split(',')
		for i in mainargs:
			v1=i.strip().split()
			if v1[0]=='uniform':
				uniforms.append(string.join(v1, ' ')+';\n')
			else:
				mainargstypes.append(v1[len(v1)-2])
				mainargsnames.append(v1[len(v1)-1])

		maintype=m.group(1)
		mainname=m.group(2)
	if debug:
		print maintype, mainname, mainargstypes, mainargsnames, uniforms, outvar
	return maintype, mainname, mainargstypes, mainargsnames, uniforms, outvar

def repmain_(txt, main):
	cl=r'(\b%s\b)' % (main)
	txt=re.sub(cl, r'__\1__', txt)
	return txt

def findmain(txt, e):
	p = re.compile(e,re.S|re.M)
	v=p.findall(txt)
	return txt

def find(txt):
	txt=findmain(txt, r'\bstruct\b\s+\b\w+\b')
	txt=findmain(txt, r'\bfloat[\d]+\s+\w+\s*:\s*POSITION[\d]*\b')
	txt=findmain(txt, r'\bfloat[\d]+\s+\w+\s*:\s*TEXCOORD[\d]*\b')
	txt=findmain(txt, r'\buniform\b.*?[,;]')
	return txt

def regex_replace(txt,e,replacement):
	txt2=txt
	txt2 = re.sub(e, replacement, txt2)
	return txt2

def replace(txt):

	e=r'\bmul\s*\((.*?),(.*?)\)'
	m = re.search(e, txt,re.S|re.M)
	if m:
		txt=re.sub(e, m.group(1)+'*'+m.group(2), txt, re.M|re.S)

	e = re.compile(r'\bfloat2\b',re.S|re.M)
	txt=regex_replace(txt,e,'vec2')
	e = re.compile(r'\bfloat3\b',re.S|re.M)
	txt=regex_replace(txt,e,'vec3')
	e = re.compile(r'\bfloat4\b',re.S|re.M)
	txt=regex_replace(txt,e,'vec4')
	e = re.compile(r'\bfloat4x4\b',re.S|re.M)
	txt=regex_replace(txt,e,'mat4')
	e = re.compile(r'\bmatrix\b',re.S|re.M)
	txt=regex_replace(txt,e,'mat4')
	e = re.compile(r'\bsampler\b',re.S|re.M)
	txt=regex_replace(txt,e,'uniform sampler')
	e = re.compile(r'\btex\b',re.S|re.M)
	txt=regex_replace(txt,e,'texture')
	e = re.compile(r'\bsampler1D\b',re.S|re.M)
	txt=regex_replace(txt,e,'uniform sampler1D')
	e = re.compile(r'\btex1D\b',re.S|re.M)
	txt=regex_replace(txt,e,'texture1D')
	e = re.compile(r'\bsampler2D\b',re.S|re.M)
	txt=regex_replace(txt,e,'uniform sampler2D')
	e = re.compile(r'\btex2D\b',re.S|re.M)
	txt=regex_replace(txt,e,'texture2D')
	e = re.compile(r'\btex2Dlod\b',re.S|re.M)
	txt=regex_replace(txt,e,'textureLod')
	e = re.compile(r'\bsampler3D\b',re.S|re.M)
	txt=regex_replace(txt,e,'uniform sampler3D')
	e = re.compile(r'\btex3D\b',re.S|re.M)
	txt=regex_replace(txt,e,'texture3D')
	e = re.compile(r'\blerp\b',re.S|re.M)
	txt=regex_replace(txt,e,'mix')
	e = re.compile(r'\bfrac\b',re.S|re.M)
	txt=regex_replace(txt,e,'fract')

	e=r':\s*\w+\s*([;{])'
	m = re.search(e, txt)
	if m:
		txt=re.sub(e, m.group(1), txt, re.M|re.S)

	return txt

def convert_(txt, mainvs='main', mainfs='main', vsmaintype_='VsOut', fsmaintype_='float4'):

	if (-1==txt.find('[Vertex shader]')) or (-1==txt.find('[Fragment shader]')):
		print 'Shader not well formed!'
		print 'Exiting ...'
		exit(0)

	cl=r'(\/\/.*?\n)'
	txt=re.sub(cl, r'\n', txt, 0, re.S|re.M)

	cl=r'(/\*.*?\*/)'
	txt=re.sub(cl, r'', txt, 0, re.S|re.M)

	e = re.compile(r'\bROW_MAJOR\s+\b',re.S|re.M)
	txt=regex_replace(txt,e,'')
	e = re.compile(r'\b\s+MVPSEMANTIC\b',re.S|re.M)
	txt=regex_replace(txt,e,'')

	txt=re.sub(r'(\)\s*\:\s*)\bSV_Target\b', r'\1', txt, 0, re.S|re.M)
	txt=re.sub(r'\b\w+State\b\s+\w+\s*\{.*?\}', r'', txt, 0, re.S|re.M)
	txt=re.sub(r'\bTexture\b', 'sampler', txt, 0, re.S|re.M)
	txt=re.sub(r'\bTexture1D\b', 'sampler1D', txt, 0, re.S|re.M)
	txt=re.sub(r'\bTexture2D\b', 'sampler2D', txt, 0, re.S|re.M)
	txt=re.sub(r'\bTexture3D\b', 'sampler3D', txt, 0, re.S|re.M)
	txt=re.sub(r'\bTextureCube\b', 'sampler', txt, 0, re.S|re.M)
	txt=re.sub(r'(\b\w+)\s*\.\s*SampleLevel\b\s*\(\s*.*?\s*,\s*(.*?)\s*,\s*.*?\s*\)', r'tex2D(\1, \2)', txt, 0, re.M|re.S)
	txt=re.sub(r'(\b\w+)\s*\.\s*Sample\b\s*\(\s*.*?\s*,\s*(.*?)\s*\)', r'tex2D(\1, \2)', txt, 0, re.M|re.S)

	ColorField=findColorField(txt, fsmaintype_)

	p = re.compile(r'\bstruct\s+(\w+\b)',re.S|re.M)
	vertexshaderstructnames=list(set(p.findall(txt)))

	vsmaintype, vsmainname, vsmainargstype, vsmainargsname, vsuniforms, vsoutvar=findmain_(txt, vsmaintype_, mainvs)
	fsmaintype, fsmainname, fsmainargstype, fsmainargsname, fsuniforms, fsoutvar=findmain_(txt, fsmaintype_, mainfs)

	txt=repmain_(txt, mainvs)
	txt=repmain_(txt, mainfs)

	shaderattributes=['\n']
	shadervaryingso=['\n']
	shadervaryingsi=['\n']
	vertexshaderins=''
	vertexshaderouts=''
	fragmentshaderins=''
	vertexshaderuniforms=string.join(vsuniforms, '')
	fragmentshaderuniforms=string.join(fsuniforms, '')

	if debug:
		print 'structs:', vertexshaderstructnames
		print 'vsmainargstype:', vsmainargstype
		print 'vsmainargsname:', vsmainargsname
		print 'vsmaintype:', vsmaintype
		print 'fsmainargstype:', fsmainargstype
		print 'fsmainargsname:', fsmainargsname
		print 'fsmaintype:', fsmaintype

	dv=''
	for i in range(0,len(vsmainargsname)):
		dv+=vsmainargstype[i]+' '+vsmainargsname[i]+';\n'
	df=''
	for i in range(0,len(fsmainargsname)):
		df+=fsmainargstype[i]+' '+fsmainargsname[i]+';\n'

	for n_ in vsmainargstype:
		nn=n_
		cl=r'\bstruct\s+%s\s*\{(.*?)\}' % (n_)
		m = re.search(cl, txt, re.M|re.S)
		if m:
			t_=m.group(1)
			p = re.compile(r'(\bfloat[\d]+)\s+(\w+)\s*\:\s*(\w+)([\d]*\b)',re.S|re.M)
			v=p.findall(t_)
			for v_ in v:
				l='attribute '+v_[0]+' '+nn+v_[1]+';'
				shaderattributes.append(l)
				vertexshaderins+='\t'+vsmainargsname[0]+'.'+v_[1]+'='+nn+v_[1]+';\n'

	cl=r'\bstruct\s+%s\s*\{(.*?)\}' % (vsmaintype)
	m = re.search(cl, txt, re.M|re.S)
	if m:
		t_=m.group(1)
		p = re.compile(r'(\bfloat[\d]+)\s+(\w+)\s*\:\s*(\w+)([\d]*\b)',re.S|re.M)
		v=p.findall(t_)
		for v_ in v:
			no=vsmaintype # 'OUT' # ???
			ni=vsmaintype # 'IN' # ???
			lo='varying '+v_[0]+' '+no+v_[1]+';'
			li='varying '+v_[0]+' '+ni+v_[1]+';'
			shadervaryingso.append(lo)
			shadervaryingsi.append(li)
			vertexshaderouts+='\t'+vsmaintype_+v_[1]+'=OUT.'+v_[1]+';\n'
			fragmentshaderins+='\t'+fsmainargsname[0]+'.'+v_[1]+'='+ni+v_[1]+';\n'

	txt=txt.replace('[Vertex shader]','[Vertex shader]\n'+string.join(shaderattributes,'\n')+string.join(shadervaryingso,'\n'))
	txt=txt.replace('[Fragment shader]','[Fragment shader]\n'+string.join(shadervaryingsi,'\n'))
	
	txt=txt.replace('[Fragment shader]', '\nvoid '+mainvs+'()\n{\n\t'+dv+vertexshaderins+'\t'+vsmaintype+' OUT=__'+mainvs+'__('+string.join(vsmainargsname, ', ')+');\n'+vertexshaderouts+'}\n[Fragment shader]')
	txt+='\nvoid '+mainfs+'()\n{\n\t'+df+fragmentshaderins+'\tglFragColor=__'+mainfs+'__('+string.join(fsmainargsname, ', ')+')'+ColorField+';\n}\n'

	txt=replace(txt)

	txt=re.sub(r'(\b\w+\b)\s+;', r'\1;', txt, 0, re.M|re.S)

	return txt

def convert(fn, mainvs='main', mainfs='main', vsmaintype_='VsOut', fsmaintype_='float4'):
	txt=load(fn)
	vv=re.findall(r'\bSetVertexShader\s*\(\s*CompileShader\(\s*\w+\s*,\s*(\w+\b)', txt,re.S|re.M)
	vf=re.findall(r'\bSetPixelShader\s*\(\s*CompileShader\(\s*\w+\s*,\s*(\w+\b)', txt,re.S|re.M)
	if len(vv)>0 and len(vf)>0:
		for i in range(0,len(vv)):
			txt=convert_(txt, vv[i], vf[i])
	else:
		txt=convert_(txt, mainvs, mainfs, vsmaintype_, fsmaintype_)
	return txt

def findp(txt, main, p):
	p1=0
	l=0
	cl='(\s+\w+\s+%s\s*\(.*?\))' % (main)
	cl2=r''+cl
	m = re.search(cl2, txt[p:len(txt)], re.M|re.S)
	if m:
		p1=string.find(txt, m.group(), p)+len(m.group())
		l=len(m.group())
	blocks(txt)	

def main(filename, vsmain, fsmain):
	fn=''
	'''
	p = re.compile(r'(\baddShaderFromFile\(.*?\))',re.S|re.M)
	v=p.findall(x)
	for i in v:	
   		p = re.compile(r'(\baddShaderFromFile\(.*?\))',re.S|re.M)
   		m=p.match(i)
		if m:
			#print m.group(1)
	'''
	if filename!='' and vsmain!='' and fsmain!='':
		fn=filename
		txt=load(filename)
		txt=convert_(txt, vsmain, fsmain)
	elif iii_ ==1:
		fn='../../IndieLib_resources/SimpleTexture/SimpleTexture.shd'
		txt=load(fn)
		txt=convert_(txt, 'main', 'main', 'VsOut', 'float4')
	elif iii_ ==2:
		fn='../../IndieLib_resources/NatureScene/TerrainShader.shd'
		txt=load(fn)
		txt=convert_(txt, 'main', 'main', 'TerrainVS_OUTPUT', 'TerrainPS_OUTPUT')
	elif iii_ ==3:
		fn='../../IndieLib_resources/Smoke/VolumeRenderer.fx'
		txt=load(fn)
		vv=re.findall(r'\bSetVertexShader\s*\(\s*CompileShader\(\s*\w+\s*,\s*(\w+\b)', txt,re.S|re.M)
		vf=re.findall(r'\bSetPixelShader\s*\(\s*CompileShader\(\s*\w+\s*,\s*(\w+\b)', txt,re.S|re.M)
		if len(vv)>0 and len(vf)>0:
			for i in range(0,len(vv)):
				txt=convert_(txt, vv[i], vf[i])
		#txt=convert(txt, 'VS_GLOW_H_9', 'PS_GLOW_9', 'VS_OUTPUT_GLOW_9', 'float4')

	if True:
		print txt
		return
	elif False:
		fn=fn.replace('.shd', '.GLSL.shd')
		f=open(fn,'w')
		f.write(txt)
		f.close()

def main2():
	print len(v)
	v_=[]
	notfound=[]
	notwellformed=[]
	fn_='../../IndieLib_resources'
	for i in range(0,len(v)/3):
		print i
		str=v[3*i+0]
		fn=''
		if str[0]=='/':
			fn=fn_+str
		else:
			fn=fn_+'/'+str
		if fn in v_:
			continue
		else:
			v_.append(fn)
		if not os.path.isfile(fn):
			notfound.append(fn)
			continue
		txt=load(fn)

		if (-1==txt.find('[Vertex shader]')) or (-1==txt.find('[Fragment shader]')):
			notwellformed.append(fn)

	print 'notfound:'
	for i in notfound:
		print i
	print 'notwellformed:'
	for i in notwellformed:
		print i

def usage():
	print 'usage:   python HLSL330toGLSL4.py [filename=<shader filename>] [vsmain=<vertexshader main entryfunction name>] [fsmain=<fragmentshader main entryfunction name>] [debug=<True|False>]'
	print 'example: python HLSL330toGLSL4.py filename=../../IndieLib_resources/SimpleTexture/SimpleTexture.shd vsmain=main fsmain=main debug=True'

if __name__ == '__main__':
	filename=''
	vsmain=''
	fsmain=''
	#debug=True
	try:
		options, remainder = getopt.getopt(sys.argv[1:], '', [	'filename=',
									'vsmain=',
									'fsmain=',
									'debug=',
									'i='
									])
		for opt, arg in options:
			#print 'opt='+opt+'\narg='+arg
			if opt=='--filename':
				filename = arg

			elif opt=='--vsmain':
				vsmain = arg

			elif opt=='--fsmain':
		        	fsmain = arg

			elif opt=='--debug':
		        	if arg=='True':
					debug=True

			elif opt=='--i':
		        	iii_ = arg

	except getopt.GetoptError as err:
		#print str(err)
		usage()
		sys.exit(2)

	main(filename, vsmain, fsmain)

