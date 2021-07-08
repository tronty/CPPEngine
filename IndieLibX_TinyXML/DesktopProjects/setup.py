import shutil
import os.path
import sys
import platform
import struct
from distutils.command.build_ext import build_ext
from distutils.sysconfig import customize_compiler
from distutils.core import setup, Extension

class my_build_ext(build_ext):
    def build_extensions(self):
        customize_compiler(self.compiler)
        try:
            self.compiler.compiler_so.remove("-Wstrict-prototypes")
        except (AttributeError, ValueError):
            pass
        build_ext.build_extensions(self)

def setup_():
	setup_args = { }
    	setup_args['cmdclass'] = {'build_ext': my_build_ext }
    	setup_args['name']='initVulkan3'
    	setup_args['version']='1.0'
	if platform.system() == 'Windows':
		setup_args['ext_modules']=[Extension('initVulkan3', ['C:\\IndieLibX\\DesktopProjects\\initVulkan3\\hello.c', 'C:\\IndieLibX\\DesktopProjects\\initVulkan3\\importvk.c'])]
	elif platform.system() == 'Darwin':
		setup_args['ext_modules']=[Extension('initVulkan3', ['./initVulkan3/hello.c', './initVulkan3/importvk.c'], include_dirs=['/usr/X11R6/include'], library_dirs=['/usr/X11R6/lib'], libraries=['X11'])]
	else:
		setup_args['ext_modules']=[Extension('initVulkan3', ['./initVulkan3/hello.c', './initVulkan3/importvk.c'])]
    	setup(**setup_args)

print 'platform.system():',platform.system()
if platform.system() == 'Windows':
    ppp=struct.calcsize("P")
    if not (8 * ppp==64):
	exit(0)
    class my_build_ext(build_ext):
        def build_extension(self, ext):
            try:
		os.makedirs(os.path.dirname(self.get_ext_fullpath(ext.name)))
            except WindowsError, e:
                if e.winerror != 183: # already exists
                    raise
	    shutil.copyfile('C:\\IndieLibX\\DesktopProjects\\initVulkan3\\initVulkan3.pyd',os.path.dirname(self.get_ext_fullpath(ext.name)))
    setup_()
elif platform.system() == 'Linux':
    srcfile='./initVulkan3/initVulkan3_LNX32.so'
    ppp=struct.calcsize("P")
    if 8 * ppp==64:
	srcfile='./initVulkan3/initVulkan3_LNX64.so'
    class my_build_ext(build_ext):
        def build_extension(self, ext):
            try:
		os.makedirs(os.path.dirname(self.get_ext_fullpath(ext.name)))
            except:
		raise
	    shutil.copyfile(srcfile,os.path.dirname(self.get_ext_fullpath(ext.name))+'/initVulkan3.so')
    setup_()
elif platform.system() == 'Darwin':
    ppp=struct.calcsize("P")
    if not (8 * ppp==64):
	exit(0)
    class my_build_ext(build_ext):
        def build_extension(self, ext):
            try:
		os.makedirs(os.path.dirname(self.get_ext_fullpath(ext.name)))
            except:
		raise
	    shutil.copyfile('./initVulkan3/initVulkan3_OSX.so',os.path.dirname(self.get_ext_fullpath(ext.name))+'/initVulkan3.so')
    setup_()
else:
	
	if platform.system() == 'Darwin':
		setup(	cmdclass = {'build_ext': my_build_ext }, \
			name='initVulkan3', \
      			version='1.0', \
			ext_modules=[Extension('initVulkan3', ['./initVulkan3/hello.c', './initVulkan3/importvk.c'], include_dirs=['/usr/X11R6/include'], library_dirs=['/usr/X11R6/lib'], libraries=['X11'])])
	else:
		setup(	cmdclass = {'build_ext': my_build_ext }, \
			name='initVulkan3', \
      			version='1.0', \
			ext_modules=[Extension('initVulkan3', ['./initVulkan3/hello.c', './initVulkan3/importvk.c'])])

