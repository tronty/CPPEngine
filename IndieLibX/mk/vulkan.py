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

dir_='../../Vulkan/%s/%s.vcxproj'
v=['instancing', 'textoverlay', 'offscreen', 'debugmarker', 'texturearray', 'geometryshader', 'subpasses', 'sphericalenvmapping', 'distancefieldfonts', 'tessellation', 'pushconstants', 'texture3d', 'radialblur', 'terraintessellation', 'computeparticles', 'texturecubemap', 'multisampling', 'particlefire', 'computenbody', 'indirectdraw', 'shadowmapping', 'raytracing', 'gears', 'deferred', 'viewportarray', 'ssao', 'dynamicuniformbuffer', 'skeletalanimation', 'mesh', 'texture', 'imgui', 'specializationconstants', 'hdr', 'deferredshadows', 'screenshot', 'occlusionquery', 'bloom', 'shadowmappingomni', 'deferredmultisampling', 'triangle', 'computeshader', 'multithreading', 'scenerendering', 'pbribl', 'parallaxmapping', 'vulkanscene', 'displacement', 'texturemipmapgen', 'pbrbasic', 'texturesparseresidency', 'pipelines', 'computecullandlod']

xmlns='''xmlns="http://schemas.microsoft.com/developer/msbuild/2003"'''

drive,tail=os.path.splitdrive(os.path.expanduser("~"))

SDL2version=''
wk='10.0.14393.0'
wk='10.0.15063.0'

def osmakedirs(fn):
	if not os.path.exists(fn):
		#print 'osmakedirs(%s)' % (fn)
		os.makedirs(fn) 

def ossystem(cmd):
	print cmd
	os.system(cmd)

def rrr(v):
	f=open('../run.sh','w')
	f.write('')
	ff=open('../run.bat','w')
	ff.write('')
	cc=0
	for i in v:
	        if True:
			filename=i
			#print 'XXX:'+filename
	        	exedir='bin'
	        	vv=exedir.split('/')
	        	ret='..'
	        	cret='..'
	        	c=0
			cexedir=exedir.replace('/','\\\\')
	        	for ii in vv:
	        		if c>0:
	        			ret=ret+'/..'
	        	    		cret=cret+'\\..'
	        		c=c+1
			Linuxfn='cd ./'+exedir+';./'+i+'.bin;cd '+ret+''
			if sys.platform=='darwin':
				Linuxfn='cd ./bin/'+i+'.app/Contents/MacOS;./'+i+';cd ../../../..'

			if not cc==0:
				f.write(' &&\n'+Linuxfn)
			else:
	        	    	f.write(Linuxfn) 
	        	ff.write('cd '+cexedir+'\ncall '+i+'.exe\ncd '+cret+'\n') 
	    		cc=cc+1
	f.close()
	ff.close()

class Configuration:
	def getwindowsphone(self):
		return False
	def getwin10(self):
		return True
	def getbitnesstxt(self):
		return 'x86'
	def getwin8(self):
		return False
	def getINDIELIBROOT(self):
		return ''
	def getbitness(self):
		return 32
	def getDebug(self):
		return True
ccc=Configuration()

class ProjectNMAKE:
	def __init__(self):
		self.skip=[]#['hlsl2glsl']
		return

	def getincs(self):
		return []
	def getflags(self):
		return []
	def getuc(self):
		return True
	def getexedir(self):
		return 'bin'
	def gettype(self):
		return 'exe'
	
	def mkx(self, name, PreprocessorDefinitions, AdditionalIncludeDirectories, AdditionalDependenciesDir, AdditionalDependencies, srcfiles):
		windowsphone=ccc.getwindowsphone()

		#print srcfiles
		acppsrcsv=srcfiles
		r=''
		if len(acppsrcsv)>0:
	        	r+='\t\t'+string.join(acppsrcsv,' \\\n\t\t\t')
		r=r.replace('/','\\')
		acppsrcs=r
		
		r='/LIBPATH:\".\\lib\" \\\n'
		#r+='\t\t/LIBPATH:\"..\\Vulkan/libs/vulkan\" \\\n'
		#r+='\t\t/LIBPATH:\"..\\Vulkan/libs/assimp\" \\\n'

		#r+='\t\t/LIBPATH:\"'+os.path.expanduser("~")+'\\Documents\\lib\" \\\n'
	
		if ccc.getbitness()==64:
			r+='\t\t/LIBPATH:\".\\Dependencies\\SDL2-'+SDL2version+'\\lib\\x64\" \\\n'
			r+='\t\t/LIBPATH:\".\\Dependencies\\openal-soft-1.16.0-bin\\libs\\Win64\" \\\n'
		else:
			r+='\t\t/LIBPATH:\".\\Dependencies\\SDL2-'+SDL2version+'\\lib\\x86\" \\\n'
			r+='\t\t/LIBPATH:\".\\Dependencies\\openal-soft-1.16.0-bin\\libs\\Win32\" \\\n'


		
		if len(AdditionalDependenciesDir):
			r+='\t\t/LIBPATH:\"'+string.join(AdditionalDependenciesDir,'\" \\\n\t\t/LIBPATH:\"')+'\" \\'
	        else:
	            r+=''
			
		#r+='\t\tvulkan-1.lib assimp.lib assimp-vc140-mt.lib '
		r+='\n\t\t'+string.join(AdditionalDependencies, ' ')		
		r=r.replace('.lib .lib','.lib')
		alibs=r	

		v=[]
		for i in self.getincs():
			x=i.replace('/','\\')
			v.append(x)
	        r=''
	        r+='\t/I\".\\Libraries\\SDL2-'+SDL2version+'\\include\" \\'
		r+='\n\t\t\t/I\".\\Libraries\\BoostWorkaround\" \\'
		r+='\n\t\t\t/I\".\\Libraries\\openal-soft-1.15.1\\include\" '
	        if len(v)>0:
	
	            r+=' \\\n\t\t\t/I\"'+string.join(v,'\" \\\n\t\t\t/I\"'+ccc.getINDIELIBROOT()+'\\')+'\" '
	        else:
	            r+=''



	        if len(AdditionalIncludeDirectories)>0:
	
	            r+=' \\\n\t\t\t/I\"'
		    r+=string.join(AdditionalIncludeDirectories,'\" \\\n\t\t\t/I\"')+'\" '
	        else:
	            r+=''


		aincs=r
	        r=''
	        r+='\t/D"_MSC_VER=1600" \\\n'
		if ccc.getbitness()==32: 
			r+='\t\t\t/D"WIN32" \\\n' 
			r+='\t\t\t/D"_WIN32" \\\n' 
			r+='\t\t\t/D"__WIN32__" \\\n'
			r+='\t\t\t/D"__32_BIT__" \\\n'
		elif ccc.getbitness()==64: 
			r+='\t\t\t/D"WIN64" \\\n' 
			r+='\t\t\t/D"_WIN64" \\\n' 
			r+='\t\t\t/D"__WIN64__" \\\n'
			r+='\t\t\t/D"__64_BIT__" \\\n'
		
	    	if not ccc.getDebug():
			r+='\t\t\t/D"NDEBUG" \\\n' 
	    	else:
			r+='\t\t\t/D"DEBUG" \\\n' 
			r+='\t\t\t/D"_DEBUG" \\\n'

		r+='\t\t\t/D"_HAS_ITERATOR_DEBUGGING=0" \\\n'
		r+='\t\t\t/D"_SECURE_SCL=0" \\\n'
		r+='\t\t\t/D"_SCL_SECURE_NO_WARNINGS" \\\n'
		r+='\t\t\t/D"_CRT_SECURE_NO_WARNINGS" \\\n'
		r+='\t\t\t/D"PROFILE" \\\n'
		r+='\t\t\t/D"D3DXFX_LARGEADDRESS_HANDLE" \\\n'
	
	        if len(self.getflags())>0:
		    r+='\t\t\t/D\"'+string.join(self.getflags(), '\" \\\n\t\t\t/D\"')+'\" \\\n'
	
	        if len(PreprocessorDefinitions)>0:
		    r+='\t\t\t/D\"'+string.join(PreprocessorDefinitions, '\" \\\n\t\t\t/D\"')+'\" \\\n'
	
		if self.getuc()==True:
			r+='\t\t\t/D"_UNICODE" \\\n\t\t\t/D"UNICODE" \n'
		else:
			r+='\t\t\t/D"_MBCS" \n'
	
	        if r==' /D\"\"':
	            r=''
		aflags=r
	
		mkf ='TARGET_NAME\t=\t'
		if self.gettype()=="exe":
	   		mkf+='.\\'+self.getexedir().replace('/','\\')+'\\'
	   	elif self.gettype()=="lib":
	   		mkf+='.\\lib\\'
	   	elif self.gettype()=="dll":
			mkf+='.\\'+self.getexedir().replace('/','\\')+'\\'
		mkf+=name+'.'+self.gettype()+'\n'
	
		mkf+='DIR_INCLUDE\t= '+aincs+'\n'
		
		if self.gettype()=="exe":
	   		mkf+='LIBRARIES\t= \\\n\t\t'+alibs+'\n'
		else:
	   		mkf+='LIBRARIES\t=\n'
		mkf+='FLAGS\t\t='+aflags
		mkf+='DIR_BIN\t\t= .\\bin\n'
		mkf+='DIR_BIN_X86\t= $(DIR_BIN)\n'
		mkf+='DIR_LIB\t\t= .\\lib\n'
		mkf+='DIR_LIB_X86\t= $(DIR_LIB)\n'

		for x in range(0,len(acppsrcsv)):
			src=acppsrcsv[x].replace('\\/','/')
			head,tail=os.path.split(src)
			#print 'src ='+src
			#print 'head='+head
			#print 'tail='+tail
			obj=src.replace('.cpp','.obj')
			src=src.replace('/','\\')
			obj=obj.replace('/','\\')
			mkf+='\n'+obj+': '+src+'\n'	
			mkf+='\tcl.exe $(DIR_INCLUDE) $(FLAGS) /Od /Zi /EHsc /MDd /Fo\"'+head.replace('/','\\')+'\\\\\" /c \"'+src+'\"\n'
	
		mkf+='\nCPPSRCS\t= '+acppsrcs+'\n\n'
	
		mkf+='OBJ_FILES = $(CPPSRCS:.cpp=.obj) $(CSRCS:.c=.obj)\n'
	
		mkf+='\n$(TARGET_NAME): $(OBJ_FILES)\n'
		
		if type=="exe":
	   		mkf+='\tlink.exe /out:$(TARGET_NAME) /debug '# /COD ???
	   	elif type=="lib":
	   		mkf+='\tlib.exe /out:$(TARGET_NAME) '# /COD ???
	   	elif type=="dll":
	   		mkf+='\tlink.exe /DLL /out:$(TARGET_NAME) /debug '# /COD ???
		mkf+='$(OBJ_FILES) $(LIBRARIES)\n'
	
		mkf+='\nclean:\n\t@echo clean'
	   	mkf+='\n\tdel $(OBJ_FILES)\n\n'
		mkf+='all:\t$(TARGET_NAME)\n\n'
		
		mkf=mkf.replace('\\\\Libraries\\','\\Libraries\\')
		mkf=mkf.replace('\\\\Samples\\','\\Samples\\')
	
		mkf=mkf.replace('.\\'+drive+'\\IndieLibX\\','.\\')
	
		mkf=mkf.replace('.\\\\'+drive+'\\IndieLibX\\','.\\')
	
		mkf=mkf.replace('.\\'+drive+'\\Libraries\\','.\\Libraries\\')
	
		mkf=mkf.replace('.\\'+drive+'\\Samples\\','.\\Samples\\')
	
		mkf=mkf.replace('DesktopProjects\\Libraries\\','Libraries\\')	
		
		mkf=mkf.replace('\\'+drive+'\\IndieLibX\\DesktopProjects\\','.\\')
		
		mkf=mkf.replace('.\\DesktopProjects\\','.\\')

		if not os.path.exists('../DesktopProjects/projectsX/'+name):
			osmakedirs('../DesktopProjects/projectsX/'+name)
	    	fn='../DesktopProjects/projectsX/'+name+'/Makefile.msc'
	    	f=open(fn,'w')
		f.write(mkf)
		f.close()
	
	def pmain(self, projects):
	    f=open('../DesktopProjects/projectsX/Makefile.msc','w')
	    
	    nmake='nmake'
	    f.write('\nall:\n')
	    for i in projects:
		if i.gettype()=='static_library':
			if os.path.isfile(os.path.expanduser("~")+'\\Documents\\lib\\'+i.getname()+'.lib'):
				continue
		if i.getname() in ccc.getSkip():
			continue
		f.write('\t'+nmake+' /F DesktopProjects/projectsX/'+i.getname()+'/Makefile.msc all\n')
	        
	    f.write('\nclean:\n')
	    for i in projects:
		if i.gettype()=='static_library':
			if os.path.isfile(os.path.expanduser("~")+'\\Documents\\lib\\'+i.getname()+'.lib'):
				continue
		if i.getname() in ccc.getSkip():
			continue
	    	f.write('\t'+nmake+' /F DesktopProjects/projectsX/'+i.getname()+'/Makefile.msc clean\n')
	    f.close()
	
	def run(self, name, PreprocessorDefinitions, AdditionalIncludeDirectories, AdditionalDependenciesDir, AdditionalDependencies, srcfiles):
		self.mkx(name, PreprocessorDefinitions, AdditionalIncludeDirectories, AdditionalDependenciesDir, AdditionalDependencies, srcfiles)
		#self.pmain(projects)

def rep(txt):
	txt=txt.replace(';%(PreprocessorDefinitions)','')
    	txt=txt.replace(';%(AdditionalIncludeDirectories)','')
    	txt=txt.replace(';%(AdditionalDependencies)','')
	#txt=txt.replace(';',' ')
	txt=txt.replace('\\','/')
	return txt

def parse_(filePath,n):
	#print filePath
	input = open(filePath)
	txt=input.read()
	input.close()
	txt=txt.replace(xmlns,'')
	tree = ElementTree.fromstring(txt)
	c=0
	for node in tree.findall('.//ProjectConfiguration'):
    		event_name=node.attrib.get('Include')
		if 'Debug|x64'==event_name:
			break
		c=c+1
    		#print event_name
	v1=tree.findall('.//PreprocessorDefinitions')
    	event_name1=v1[c].text
	event_name1=rep(event_name1)	
	PreprocessorDefinitions=event_name1.split(';')

	#PreprocessorDefinitions.remove('WIN32')
	PreprocessorDefinitions.append('_WIN32')
	#PreprocessorDefinitions.append('__ANDROID__')
	#PreprocessorDefinitions.append('__linux__')

    	#print event_name1
	v2 = tree.findall('.//AdditionalIncludeDirectories')
    	event_name2=v2[c].text
	event_name2=rep(event_name2)	
	AdditionalIncludeDirectories=event_name2.split(';')
	tmp=[]
	for i in AdditionalIncludeDirectories:
		txt=i
		if i.startswith('..'):
			txt=i.replace('..','../Vulkan')
		tmp.append(txt)
	AdditionalIncludeDirectories=tmp
    	#print event_name2
	v3 = tree.findall('.//AdditionalDependencies')
	event_name3=v3[c].text
	event_name3=rep(event_name3)	
	AdditionalDependencies_=event_name3.split(';')

	AdditionalDependenciesDir=[]
	AdditionalDependencies=[]
	for i in AdditionalDependencies_:
		if i.endswith('.lib'):
			head,tail=os.path.split(i)
			if not head=='':
				AdditionalDependenciesDir.append(head)
			if not tail=='':
				AdditionalDependencies.append(tail)


    	#print event_name3
	v4=[]
	for node in tree.findall('.//ClCompile'):
    		event_name4=node.attrib.get('Include')
    		if event_name4:
			event_name4=event_name4.replace('\\','/')
			v4.append('../Vulkan/'+n+'/'+event_name4)

	#print v4
	v1txt=''
	if len(PreprocessorDefinitions):
		v1txt+=string.join(PreprocessorDefinitions,' ')
	v2txt=''
	if len(AdditionalIncludeDirectories):
		v2txt+=string.join(AdditionalIncludeDirectories,' ')
	v3txt=''
	if len(AdditionalDependencies):
		v3txt+=string.join(AdditionalDependencies,' ')
	v4txt=''
	if len(v4):
		v4txt+=string.join(v4,' ')
	x='''\t<project>
		<name>'''+n+'''</name>
		<flags>'''+v1txt+'''</flags>
		<include_directories>'''+v2txt+'''</include_directories>
		<dependencies>'''+v3txt+'''</dependencies>
		<src_files>'''+v4txt+'''</src_files>
		<construct>application</construct>
	</project>
	'''
	#print x
	p=ProjectNMAKE()
	p.run(n, PreprocessorDefinitions, AdditionalIncludeDirectories, AdditionalDependenciesDir, AdditionalDependencies, v4)
	return
	
def pmain(projects):
	f=open('../DesktopProjects/projectsX/Makefile.msc','w')    
	nmake='nmake'
	f.write('\nall:\n')
	for i in projects:
		f.write('\t'+nmake+' /F DesktopProjects/projectsX/'+i+'/Makefile.msc all\n')
	        
	f.write('\nclean:\n')
	for i in projects:
	    	f.write('\t'+nmake+' /F DesktopProjects/projectsX/'+i+'/Makefile.msc clean\n')
	f.close()
	
for i in v:
	filePath=dir_ % (i,i)
	parse_(filePath,i)
pmain(v)
rrr(v)

'''
prequisities:
cd ~
git clone https://github.com/SaschaWillems/Vulkan.git

_WIN32
__ANDROID__
__linux__
'''

