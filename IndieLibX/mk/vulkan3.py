import os, sys, re
v=['instancing/instancing.cpp','textoverlay/textoverlay.cpp','offscreen/offscreen.cpp','debugmarker/debugmarker.cpp','texturearray/texturearray.cpp','geometryshader/geometryshader.cpp','subpasses/subpasses.cpp','sphericalenvmapping/sphericalenvmapping.cpp','distancefieldfonts/distancefieldfonts.cpp','tessellation/tessellation.cpp','pushconstants/pushconstants.cpp','texture3d/texture3d.cpp','radialblur/radialblur.cpp','terraintessellation/terraintessellation.cpp','computeparticles/computeparticles.cpp','texturecubemap/texturecubemap.cpp','multisampling/multisampling.cpp','particlefire/particlefire.cpp','computenbody/computenbody.cpp','indirectdraw/indirectdraw.cpp','shadowmapping/shadowmapping.cpp','raytracing/raytracing.cpp','deferred/deferred.cpp','viewportarray/viewportarray.cpp','ssao/ssao.cpp','dynamicuniformbuffer/dynamicuniformbuffer.cpp','skeletalanimation/skeletalanimation.cpp','mesh/mesh.cpp','texture/texture.cpp','imgui/main.cpp','specializationconstants/specializationconstants.cpp','hdr/hdr.cpp','deferredshadows/deferredshadows.cpp','screenshot/screenshot.cpp','occlusionquery/occlusionquery.cpp','bloom/bloom.cpp','shadowmappingomni/shadowmappingomni.cpp','deferredmultisampling/deferredmultisampling.cpp','triangle/triangle.cpp','computeshader/computeshader.cpp','multithreading/multithreading.cpp','scenerendering/scenerendering.cpp','pbribl/pbribl.cpp','parallaxmapping/parallaxmapping.cpp','vulkanscene/vulkanscene.cpp','displacement/displacement.cpp','texturemipmapgen/texturemipmapgen.cpp','pbrbasic/pbrbasic.cpp','texturesparseresidency/texturesparseresidency.cpp','pipelines/pipelines.cpp','computecullandlod/computecullandlod.cpp']

#','gears/gears.cpp
#','gears/vulkangear.cpp',

d='../Libraries/VulkanSamples/'

for i in v:
	name=i.split('/')[0]
	print 'Vulkan'+name

for i in v:
	name=i.split('/')[0]

	p='''
<project>
	<name>Vulkan'''+name+'''</name>
	<include_directories>Libraries/Vulkan/base Libraries/Vulkan/external Libraries/Vulkan/external/glm Libraries/Vulkan/external/gli Libraries/Vulkan/external/imgui</include_directories>
	<src_directory>Libraries/VulkanSamples/'''+name+'''</src_directory>
	<src_files>'''+name+'''.cpp</src_files>
	<construct>application</construct>
</project>'''
	print p


