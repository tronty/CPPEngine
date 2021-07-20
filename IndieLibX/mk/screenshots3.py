import os, sys, re, math
from PIL import Image
from os import listdir
from os.path import isfile, join
import fnmatch
import shutil
import ftplib

def osmakedirs(fn):
	if not os.path.exists(fn):
		#print 'osmakedirs(%s)' % (fn)
		os.makedirs(fn) 

def ftplibFTP(filename, ddir):
	head, tail = os.path.split(filename)
	print filename+': '+ddir+'/'+tail
	#return
	
	if not os.path.exists(ddir):
		osmakedirs(ddir)

	shutil.copy(filename,ddir)
	return
	session = ftplib.FTP('www.tommironty.fi','tommiro1','')
	file = open(filename,'rb')                  # file to send
	session.storbinary('STOR '+ddir+'/'+tail, file)     # send the file
	file.close()                                    # close file and FTP
	session.quit()

'''
def copy_rename(old_file_name, new_file_name):
        src_dir= os.curdir
        dst_dir= os.path.join(os.curdir , "subfolder")
        src_file = os.path.join(src_dir, old_file_name)
        shutil.copy(src_file,dst_dir)
        
        dst_file = os.path.join(dst_dir, old_file_name)
        new_dst_file_name = os.path.join(dst_dir, new_file_name)
        os.rename(dst_file, new_dst_file_name)

#os.makedirs('../../public_html/screenshots2')
d='../../public_html/screenshots256x256/'

def xx(ln, fil):		
		img=Image.open(fil)
		#img.resize(256, 256)
		head, tail = os.path.split(fil)
		name, ext = os.path.splitext(tail)
		s=d+name+'256x256'+ext
		s=d+name+ext
		print s
		img.save(s)
		ln+='<DT><P ALIGN="CENTER"><img src="%s" alt="Screenshot(%s)"> %s</P></DT>' % (s, tail, name)
		return ln

def x(f, files):
	#print files
	for i in range(0,(len(files)/4)):
		ln='<DL>'
		print(files[4*i+0])
		ln+=xx(ln, files[4*i+0])
		ln+=xx(ln, files[4*i+1])
		ln+=xx(ln, files[4*i+2])
		ln+=xx(ln, files[4*i+3])
		ln+='</DL>\n'
		ln=ln.replace('../../public_html/','')
		ln=ln.replace('screenshots/','')
		ln=ln.replace('dx9_','../dx9_')
		ln=ln.replace(' ','_')
		ln=ln.replace('\t','_')



		ln=ln.replace('<img_src=','<img src=')
		ln=ln.replace('_alt=',' alt=')


		ln=ln.replace('>_ScreenShot_','> ScreenShot_')

		f.write(ln);

def zzz():
	f=open(d+'screenshots__.html','w')
	ln='<HTML><HEAD></HEAD><BODY><TITLE>Screenshots</TITLE><H1>Screenshots</H1><TABLE>\n'
	f.write(ln)
	matches_ = []
	matches = []
	for root, dirnames, filenames in os.walk('../../public_html'):
    		for filename in fnmatch.filter(filenames, '*.jpg):
        		matches_.append(os.path.join(root, filename))
	matches.append(matches_[0])
	matches.append(matches_[1])
	matches.append(matches_[2])
	matches.append(matches_[3])
	x(f, matches_)
	ln='</TABLE></BODY></HTML>\n'
	f.write(ln)
	f.close()

v_=[]
def replaceStringInFile(filePath):
	filePath_=filePath.replace('../../public_html/','./')
	print filePath_
	v_.append(filePath_)		
	img=Image.open(filePath)
	#img.resize(256, 256)
	img.thumbnail([256, 256], Image.ANTIALIAS)
	head, tail = os.path.split(filePath_)
	name, ext = os.path.splitext(tail)
	s=d+name+'256x256'+ext
	s=d+name+ext
	print s
	img.save(s)

def myfun(dummy, dirr, filess):
	#print '#3'
	#print dummy, dirr, filess
	#onlyfiles = [f for f in listdir(dirr) if isfile(join(dirr, f))]
	#print onlyfiles
	filess = [f for f in listdir(dirr) if isfile(join(dirr, f))]
	#print filess
	for child in filess:
	    #print child
	    if '.jpg == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile(dirr+'/'+child)
		    #print child

#v=['../../public_html']
#for i in v:
#	os.path.walk(i, myfun, 13)
#print len(v_), math.sqrt(len(v_))

path='../../public_html/screenshots256x256'
files = []
# r=root, d=directories, f = files
for r, d, f in os.walk(path):
    for file in f:
        if '.jpg in file:
            files.append(os.path.join(r, file))

files.sort()
for f in files:
    print f
print len(files)
'''

def whtml(files, name, d):
	f=open('./screenshots '+name+'.html','w')
	ln='<HTML>\n<HEAD>\n</HEAD>\n<BODY>\n<TITLE>Screenshots '+name+'</TITLE>\n<H1>Screenshots '+name+'</H1>\n<TABLE>\n'
	f.write(ln)
	for y in range(0,d+1):
		ln='<TR>'
		for x in range(0,d+1):
			ln_='<TD></TD>'
			if (d*y+x)<len(files):
				f_=files[d*y+x]
				#f_=f_.replace('../../public_html/screenshots256x256/','./')
				ln_='<TD><img src="%s" alt="Screenshot(%s)"></TD>' % (f_,f_)
			ln+=ln_
		f.write(ln+'</TR>\n')
	#f_='./tiny.jpg
	#ln='<TR><TD><img src="%s" alt="Screenshot(%s)"></TD></TR>\n' % (f_,f_)
	#f.write(ln)
	ln='</TABLE>\n</BODY>\n</HTML>\n'
	f.write(ln)
	f.close()

ATI_SDK='''3Dc.jpg
Alpha-to-Mask Coverage.jpg
Alternate Frame Rendering (AFR).jpg
ATI1N.jpg
Audio Processing.jpg
AudioProcessing.jpg
AutoGenMipMaps.JPG
Back Light.jpg
crowd.jpg
DepthBias.JPG
DepthOfField.JPG
EarlyOut.jpg
FixedFuncShader.JPG
Flow - Fluid Flow Screensaver.jpg
FrameBufferObject.jpg
HDR.jpg
High Dynamic Range Imaging.jpg
HighPrecisionNormalMaps.JPG
HorizonMapping.jpg
ImageAnalysis.jpg
Instancing.jpg
Logo.jpg
Mandelbrot.jpg
Materials.jpg
MotionBlur.jpg
NormalCompression.jpg
OcclusionQuery.JPG
ParallaxMapping.jpg
Parallax Occlusion Mapping.jpg
Parthenon.jpg
PlaneShadowMapping.jpg
PostProcessing.jpg
PS 2.0 Static Branching.jpg
QueryLightFlare.jpg
RayTracing.jpg
Render to Vertex Buffer Animation.jpg
Render to Vertex Buffer Cloth.jpg
Render to Vertex Buffer Collision.jpg
Render to Vertex Buffer Footprint.jpg
Render to Vertex Buffer IK.jpg
RenderToVertexBuffer.jpg
Render to Vertex Buffer N-Patches.jpg
Render To Vertex Buffer Ocean Water.jpg
Render to Vertex Buffer Particle System.jpg
Render to Vertex Buffer Shadow Volume.jpg
Render to Vertex Buffer Sort.jpg
Render to Vertex Buffer Terrain Morph.jpg
Render to Vertex Buffer Water.jpg
Ruby - Dangerous Curves.jpg
Ruby - The Doublecross.jpg
ShadowMapDST.JPG
ShadowMapFetch4.JPG
ShadowMap.JPG
SoftShadows.jpg
sRGBTest.JPG
Stereo2.jpg
Stereo.jpg
Subsurface Scattering.jpg
The Assassin.jpg
tools.jpg
ToyShop.jpg
VolumetricLighting.jpg'''
SDK_9_5='''./DEMOS/Direct3D9/src/SnowAccumulation/docs/sshot_md.png
./DEMOS/Direct3D9/src/CSAATutorial/docs/sshot_md.png
./DEMOS/Direct3D9/src/PracticalPSM/docs/sshot_md.png
./DEMOS/Direct3D9/src/HLSL_SoftShadows/docs/sshot_md.png
./DEMOS/Direct3D9/src/HLSL_HardwareShadowMap/docs/sshot_md.png
./DEMOS/Direct3D9/src/QuerySample/docs/sshot_md.png
./DEMOS/Direct3D9/src/HLSL_BumpyShinyVariations/docs/sshot_md.png
./DEMOS/Direct3D9/src/Direct3D_NV_ID/docs/sshot_md.png
./DEMOS/Direct3D9/src/BatchingViaTextureAtlases/AtlasComparisonViewer/docs/sshot_md.png
./DEMOS/Direct3D9/src/NearClipPlane/docs/sshot_md.png
./DEMOS/Direct3D9/src/NVTriStripTest/docs/sshot_md.png
./DEMOS/Direct3D9/src/HLSL_FresnelReflection/docs/sshot_md.png
./DEMOS/Direct3D9/src/StencilShadow/docs/sshot_md.png
./DEMOS/Direct3D9/src/HLSL_PaletteSkin/docs/sshot_md.png
./DEMOS/Direct3D9/src/FogPolygonVolumes3/docs/sshot_md.png
./DEMOS/Direct3D9/src/GetGPUAndSystemInfo/docs/sshot_md.png
./DEMOS/Direct3D9/src/AnisoDecimation/docs/sshot_md.png
./DEMOS/Direct3D9/src/HLSL_VertexLight/docs/sshot_md.png
./DEMOS/Direct3D9/src/HLSL_FilterBlit/docs/sshot_md.png
./DEMOS/Direct3D9/src/Glow/docs/sshot_md.png
./DEMOS/Direct3D9/src/HLSL_Aniso/docs/sshot_md.png
./DEMOS/Direct3D9/src/MeshMenderSimpleExample/docs/sshot_md.png
./DEMOS/Direct3D9/src/HLSL_DepthOfField/docs/sshot_md.png
./DEMOS/Direct3D9/src/Mandelbrot/docs/sshot_md.png
./DEMOS/Direct3D9/src/HLSL_BloodShader/docs/sshot_md.png
./DEMOS/Direct3D9/src/AntiAliasingWithPostProcessing/docs/sshot_md.png
./DEMOS/Direct3D9/src/AntiAliasingWithTransparency/docs/sshot_md.png
./DEMOS/Direct3D9/src/HLSL_OcclusionQueryDemo/docs/sshot_md.png
./DEMOS/Direct3D9/src/Membrane/docs/sshot_md.png
./DEMOS/Direct3D9/src/WaterInteraction/docs/sshot_md.png
./DEMOS/Direct3D9/src/DeferredShading/docs/sshot_md.png
./DEMOS/Direct3D9/src/StereoAPI/docs/sshot_md.png
./DEMOS/Direct3D9/src/DXSAS/Documentation/sshot_md.png
./DEMOS/Direct3D9/src/TwoSidedPolys/docs/sshot_md.png
./DEMOS/Direct3D9/src/HLSL_VertexMorph/docs/sshot_md.png
./DEMOS/OpenGL/src/glsl_pseudo_instancing/docs/sshot_md.png
./DEMOS/Direct3D9/src/HLSL_RainbowFogbow/docs/screenshot.png
./DEMOS/Direct3D9/src/HLSL_Instancing/docs/screenshot.png
./DEMOS/OpenGL/src/external_triangles/docs/screenshot.png
./DEMOS/OpenGL/src/glsl_thinfilm/docs/screenshot.png
./DEMOS/OpenGL/src/cullfragment/docs/screenshot.png
./DEMOS/OpenGL/src/layerz_3x/docs/screenshot.png
./DEMOS/OpenGL/src/glsl_digital_clock/docs/screenshot.png
./DEMOS/OpenGL/src/glsl_physics/docs/screenshot.png
./DEMOS/OpenGL/src/glsl_eye/docs/screenshot.png
./DEMOS/OpenGL/src/glsl_simple_lighting/docs/screenshot.png
./DEMOS/OpenGL/src/texshd_offset_2d/docs/screenshot.png
./DEMOS/OpenGL/src/simple_fragment_program2/docs/screenshot.png
./DEMOS/OpenGL/src/pbuffer_to_texture_rectangle/docs/screenshot.png
./DEMOS/OpenGL/src/volume_intersect/docs/screenshot.png
./DEMOS/OpenGL/src/perpixel_attenuation/docs/screenshot.png
./DEMOS/OpenGL/src/simple_framebuffer_object/docs/screenshot.png
./DEMOS/OpenGL/src/hw_shadowmaps_simple/docs/screenshot.png
./DEMOS/OpenGL/src/bumpy_shiny_patch/docs/screenshot.png
./DEMOS/OpenGL/src/glsl_bump_mapping/docs/screenshot.png
./DEMOS/OpenGL/src/glsl_metalpaint/docs/screenshot.png
./DEMOS/OpenGL/src/simple_fp16_blend/docs/screenshot.png
./DEMOS/OpenGL/src/bump_reflect_local_light/docs/screenshot.png
./DEMOS/OpenGL/src/offset_bump_mapping/docs/screenshot.png
./DEMOS/OpenGL/src/infinite_shadow_volumes/docs/screenshot.png
./DEMOS/OpenGL/src/simple_vertex_texture/docs/screenshot.png
./DEMOS/OpenGL/src/glsl_skinning/docs/screenshot.png
./DEMOS/OpenGL/src/oit_3x/docs/screenshot.png
./DEMOS/OpenGL/src/simple_render_texture/docs/screenshot.png
./DEMOS/OpenGL/src/ambient_occlusion/docs/screenshot.png
./DEMOS/OpenGL/src/paint/docs/screenshot.png
./DEMOS/OpenGL/src/GL_GameOfLife/docs/screenshot.png
./DEMOS/OpenGL/src/normalization_heuristics/docs/screenshot.png
./DEMOS/OpenGL/src/simple_float_pbuffer/docs/screenshot.png
./DEMOS/OpenGL/src/AntiAliasingWithPostProcessing/docs/screenshot.png
./DEMOS/OpenGL/src/motion_blur/docs/screenshot.png
./DEMOS/OpenGL/src/glsl_dispersion/docs/screenshot.png
./DEMOS/OpenGL/src/fog_compare/docs/screenshot.png
./DEMOS/OpenGL/src/simple_draw_buffers/docs/screenshot.png
./DEMOS/OpenGL/src/glsl_vnoise/docs/screenshot.png
./DEMOS/OpenGL/src/simple_texture_rectangle/docs/screenshot.png
./DEMOS/OpenGL/src/detail_normalmap/docs/screenshot.png
./DEMOS/OpenGL/src/superflow/docs/screenshot.png
./DEMOS/OpenGL/src/glsl_vertex_water/docs/screenshot.png
./DEMOS/OpenGL/src/simple_pbuffer/docs/screenshot.png
./DEMOS/OpenGL/src/occlusion_query/docs/screenshot.png'''
DXSDK_Jun10='''./Direct3D/PixelMotionBlur/PixelMotionBlur.jpg
./Direct3D/EmptyProject/EmptyProject.jpg
./Direct3D/EffectParam/EffectParam.jpg
./Direct3D/OptimizedMesh/OptimizedMesh.jpg
./Direct3D/PostProcess/postprocess.jpg
./Direct3D/Tutorials/Tut01_CreateDevice/Tut01_CreateDevice.jpg
./Direct3D/Tutorials/Tut04_Lights/Tut04_Lights.jpg
./Direct3D/Tutorials/Tut02_Vertices/Tut02_Vertices.jpg
./Direct3D/Tutorials/Tut03_Matrices/Tut03_Matrices.jpg
./Direct3D/Tutorials/Tut05_Textures/Tut05_Textures.jpg
./Direct3D/Tutorials/PIXGameDebugging/PIXGameDebugging.jpg
./Direct3D/Tutorials/Tut06_Meshes/Tut06_Meshes.jpg
./Direct3D/ParallaxOcclusionMapping/ParallaxOcclusionMapping.jpg
./Direct3D/IrradianceVolume/IrradianceVolume.jpg
./Direct3D/Pick/Pick.jpg
./Direct3D/ShadowVolume/ShadowVolume.jpg
./Direct3D/Instancing/Instancing.jpg
./Direct3D/StateManager/StateManager.jpg
./Direct3D/HLSLwithoutEffects/HLSLwithoutEffects.jpg
./Direct3D/PRTDemo/PRTDemo.jpg
./Direct3D/PRTCmdLine/PRTCmdLine.jpg
./Direct3D/CompiledEffect/CompiledEffect.jpg
./Direct3D/HDRFormats/hdrformats.jpg
./Direct3D/Text3D/Text3D.jpg
./Direct3D/BasicHLSL/BasicHLSL.jpg
./Direct3D/ConfigSystem/configsystem.jpg
./Direct3D/MultiAnimation/Floor.jpg
./Direct3D/MultiAnimation/MultiAnimation.jpg
./Direct3D/HDRLighting/HDRLighting.jpg
./Direct3D/HDRPipeline/HDRPipeline.jpg
./Direct3D/SkinnedMesh/skinnedmesh.jpg
./Direct3D/ShadowMap/ShadowMap.jpg
./Direct3D/DepthOfField/DepthofField.jpg
./Direct3D/LocalDeformablePRT/LocalDeformablePRT.jpg
./Direct3D/CustomUI/CustomUI.jpg
./Direct3D/HDRCubeMap/HDRCubeMap.jpg
./Direct3D10/SparseMorphTargets/SparseMorphTargets.jpg
./Direct3D10/Instancing10/instancing.jpg
./Direct3D10/10BitScanout10/10BitScanout10.jpg
./Direct3D10/EffectPools/EffectPools.jpg
./Direct3D10/AdvancedParticles/AdvancedParticles.jpg
./Direct3D10/Tutorials/Tutorial08/Tutorial08.jpg
./Direct3D10/Tutorials/Tutorial14/Tutorial14.jpg
./Direct3D10/Tutorials/Tutorial04/Tutorial04.jpg
./Direct3D10/Tutorials/Tutorial01/Tutorial01.jpg
./Direct3D10/Tutorials/Tutorial00/Tutorial00.jpg
./Direct3D10/Tutorials/Direct3D10WorkshopGDC2007/d3d10workshop2007.jpg
./Direct3D10/Tutorials/Direct3D10WorkshopGDC2007/D3DWkshpPlugin/D3DWkshpPlugin.jpg
./Direct3D10/Tutorials/Tutorial12/Tutorial12.jpg
./Direct3D10/Tutorials/Tutorial07/Tutorial07.jpg
./Direct3D10/Tutorials/PIXWorkshopGDC2007/pixworkshop2007.jpg
./Direct3D10/Tutorials/Tutorial03/Tutorial03.jpg
./Direct3D10/Tutorials/Tutorial09/Tutorial09.jpg
./Direct3D10/Tutorials/Tutorial13/Tutorial13.jpg
./Direct3D10/Tutorials/Tutorial06/Tutorial06.jpg
./Direct3D10/Tutorials/Tutorial11/Tutorial11.jpg
./Direct3D10/Tutorials/Direct3D10Workshop/d3d10workshop.jpg
./Direct3D10/Tutorials/Tutorial10/Tutorial10.jpg
./Direct3D10/Tutorials/Tutorial05/Tutorial05.jpg
./Direct3D10/Tutorials/Tutorial02/Tutorial02.jpg
./Direct3D10/DepthOfField10.1/DepthOfField10.1.jpg
./Direct3D10/ContentStreaming/ContentStreaming.jpg
./Direct3D10/ProceduralMaterials/ProceduralMaterials.jpg
./Direct3D10/RaycastTerrain/RaycastTerrain.jpg
./Direct3D10/DeferredParticles/DeferredParticles.jpg
./Direct3D10/SoftParticles/SoftParticles.jpg
./Direct3D10/NBodyGravity/NBodyGravity.jpg
./Direct3D10/PipesGS/pipesgs.jpg
./Direct3D10/ParticlesGS/particlesgs.jpg
./Direct3D10/TransparencyAA10.1/screenshot.jpg
./Direct3D10/GPUBoids/GPUBoids.jpg
./Direct3D10/FixedFuncEMU/ffemu.jpg
./Direct3D10/SubD10/SubD10.jpg
./Direct3D10/HDAO10.1/HDAO10.1.jpg
./Direct3D10/DDSWithoutD3DX/DDSWithoutD3DX.jpg
./Direct3D10/GPUSpectrogram/GPUSpectrogram.jpg
./Direct3D10/HDRFormats10/HDRFormats10.jpg
./Direct3D10/Motionblur10/MotionBlur10.jpg
./Direct3D10/MeshFromOBJ10/MeshFromOBJ10.jpg
./Direct3D10/MultiMon10/MultiMon10.jpg
./Direct3D10/Skinning10/Skinning10.jpg
./Direct3D10/CubeMapGS/CubeMapGS.jpg
./Direct3D11/FluidCS11/FluidCS11.jpg
./Direct3D11/ContactHardeningShadows11/Screenshot.jpg
./Direct3D11/ComputeShaderSort11/ComputeShaderSort11.jpg
./Direct3D11/DetailTessellation11/Screenshot.jpg
./Direct3D11/AdaptiveTessellationCS40/AdaptiveTessellationCS40.jpg
./Direct3D11/MultithreadedRendering11/MultithreadedRendering11.jpg
./Direct3D11/WindowsTouch/WindowsTouch.jpg
./Direct3D11/CascadedShadowMaps11/CascadedShadowMaps11.jpg
./Direct3D11/SimpleBezier11/SimpleBezier11.jpg
./Direct3D11/HDRToneMappingCS11/HDRToneMappingCS11.jpg
./Direct3D11/SubD11/SubD11.jpg
./Direct3D11/DynamicShaderLinkage11/DynamicShaderLinkage11.jpg
./Direct3D11/OIT11/OIT11.jpg
./Direct3D11/DecalTessellation11/DecalTessellation11.jpg
./Direct3D11/PNTriangles11/Screenshot.jpg
./Direct3D11/BasicCompute11/BasicCompute11.jpg
./Direct3D11/VarianceShadows11/VarianceShadows11.jpg'''

vulkan='''./multisampling_nomsaa.png
./vulkan_scene.png
./textoverlay.png
./font_distancefield.png
./shadow_omnidirectional.png
./texture_array.png
./basic_gears.png
./basic_mesh.png
./basic_triangle.png
./mesh_skeletalanimation.png
./occlusion_queries.png
./multisampling.png
./radial_blur.png
./shadowmapping.png
./basic_texture.png
./push_constants.png
./basic_pipelines.png
./spherical_env_mapping.png
./multithreading.jpg
./pbrbasic.jpg
./dynamicuniformbuffer.jpg
./tess_dynamicterrain.jpg
./pbrtexture.jpg
./tess_pntriangles.jpg
./geom_normals.jpg
./particlefire.jpg
./compute_nbody.jpg
./compute_raytracing.jpg
./hdr.jpg
./indirectdraw.jpg
./specialization_constants.jpg
./deferred_shading.jpg
./texture_cubemap.jpg
./compute_particles.jpg
./basic_offscreen.jpg
./ext_debugmarker.jpg
./scenerendering.jpg
./texture_mipmap_gen.jpg
./parallax_mapping.jpg
./cascaded_shadow_mapping.jpg
./subpasses.jpg
./compute_imageprocessing.jpg
./ssao.jpg
./pbribl.jpg
./deferred_shadows.jpg
./instancing.jpg
./compute_cullandlod.jpg
./tess_displacement.jpg
./bloom.jpg'''

'''
# 62 8x8
# 82 10x10
# 191-84=117 11x11
# 384-335=50 8x8

# ~/Asiakirjat/SDKs/screenshots/ATI_SDK_thumbnails
~/Asiakirjat/SDKs/SDK 9.5
~/Asiakirjat/SDKs/DXSDK_Jun10/screenshots
/home/tronty/Vulkan/screenshots

# http://tommironty.fi/screenshots/ATI_SDK
http://tommironty.fi/screenshots/SDK_9.5
http://tommironty.fi/screenshots/DXSDK_Jun10
http://tommironty.fi/screenshots/Vulkan

ati_sdk(mar2006)
NVIDIA_SDK_9.52.0324.0405
NVIDIA_SDK11_Direct3D_11.00.0328.2105

~/Asiakirjat/SDKs$ ls
'ATI SDK'   DXSDK_Jun10    'SDK 9.5'   Vulkan

'''

# ~/Asiakirjat/SDKs/screenshots$ ls
# 'ATI SDK'   DXSDK_Jun10   SDK_9.5   Vulkan
SDK_9_5=SDK_9_5.replace('./','/home/tronty/Asiakirjat/SDKs/screenshots/SDK_9.5/')
#SDK_9_5=SDK_9_5.replace('/','_')
v=SDK_9_5.split()
'''
for i in v:
	if not os.path.isfile(i):
		print i
	else:
		head, tail = os.path.split(i)
		head=head.replace('/DEMOS/', '/')
		head=head.replace('/src/', '/')
		head=head.replace('/docs', '')
		head=head.replace('/home/tronty/Asiakirjat/SDKs/', './screenshots/')
		
		#ftplibFTP(i,head)
'''
whtml(v, 'NVIDIA SDK 9.5', 10)

DXSDK_Jun10=DXSDK_Jun10.replace('./','/home/tronty/Asiakirjat/SDKs/screenshots/DXSDK_Jun10/Samples/C++/')
v=DXSDK_Jun10.split()
'''
for i in v:
	if not os.path.isfile(i):
		print i
	else:
		head, tail = os.path.split(i)
		head=head.replace('/screenshots/', '/')
		head=head.replace('/Samples/C++/', '/')
		head=head.replace('/home/tronty/Asiakirjat/SDKs/', './screenshots/')
		
		#ftplibFTP(i,head)
'''
whtml(v, 'DXSDK_Jun10', 11)

ATI_SDK=ATI_SDK.replace('./','/home/tronty/Asiakirjat/SDKs/screenshots/ATI SDK/')
v=ATI_SDK.split('\n')
#for i in v:
#	print i
whtml(v, 'ATI SDK (2006)', 8)

vulkan=vulkan.replace('./','/home/tronty/Asiakirjat/SDKs/screenshots/Vulkan/')
v=vulkan.split()
#for i in v:
#	print i
whtml(v, 'Vulkan', 8)

