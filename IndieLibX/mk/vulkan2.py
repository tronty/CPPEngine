import os 
import os.path
import re
import string
import struct
import getopt
import sys
from xml.etree import ElementTree
from os.path import expanduser
import shutil
import platform

v=['instancing', 'textoverlay', 'offscreen', 'debugmarker', 'texturearray', 'geometryshader', 'subpasses', 'sphericalenvmapping', 'distancefieldfonts', 'tessellation', 'pushconstants', 'texture3d', 'radialblur', 'terraintessellation', 'computeparticles', 'texturecubemap', 'multisampling', 'particlefire', 'computenbody', 'indirectdraw', 'shadowmapping', 'raytracing', 'gears', 'deferred', 'viewportarray', 'ssao', 'dynamicuniformbuffer', 'skeletalanimation', 'mesh', 'texture', 'imgui', 'specializationconstants', 'hdr', 'deferredshadows', 'screenshot', 'occlusionquery', 'bloom', 'shadowmappingomni', 'deferredmultisampling', 'triangle', 'computeshader', 'multithreading', 'scenerendering', 'pbribl', 'parallaxmapping', 'vulkanscene', 'displacement', 'texturemipmapgen', 'pbrbasic', 'texturesparseresidency', 'pipelines', 'computecullandlod']

def osmakedirs(fn):
	if not os.path.exists(fn):
		#print 'osmakedirs(%s)' % (fn)
		os.makedirs(fn) 

def ossystem(cmd):
	print cmd
	os.system(cmd)

ossystem('rm -fr ~/Vulkan_')
osmakedirs('~/Vulkan_')
for i in v:
	ossystem('cp -fr ~/Vulkan/'+i+' ~/Vulkan_')

ossystem('rm -fr ~/Vulkan__')
ossystem('cp -fr ~/Vulkan ~/Vulkan__')
for i in v:
	ossystem('rm -fr ~/Vulkan__/'+i)

'''
prequisities:
cd ~
git clone https://github.com/SaschaWillems/Vulkan.git

_WIN32
__ANDROID__
__linux__
'''

