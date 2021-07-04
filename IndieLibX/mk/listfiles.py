import os, sys, re, string
	
def myfun(dummy, dirr, filess):
	#print dummy, dirr, filess
	for child in filess:
	    print child
	    if '.c' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile(dirr+'/'+child)
		    print child
	    if '.cc' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile(dirr+'/'+child)
		    print child
	    if '.cxx' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile(dirr+'/'+child)
		    print child
	    if '.cpp' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile(dirr+'/'+child)
		    print child
	    if '.m' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile(dirr+'/'+child)
		    print child


  amd-dev.wpengine.netdna-cdn.com.wordpress.media.2012.10
  ATI SDK
  developer.download.nvidia.com.SDK.10
  Microsoft DirectX SDK (June 2010)
  NVIDIA Corporation
  NVIDIA Direct3D SDK 11
  www.humus.name

rd1=['~/IndieLibX/Libraries/SDL2-2.0.5/src','~/IndieLibX/Libraries/openal-soft-1.15.1','~/IndieLibX/Libraries/segvcatch-0.9.1/lib','~/IndieLibX/Libraries/zlib','~/IndieLibX/Libraries/png','~/IndieLibX/Libraries/jpeg-8c','~/IndieLibX/Libraries/openil/src-IL/src','~/IndieLibX/Libraries/openil/src-ILU/src','~/IndieLibX/Libraries/STX/src','~/IndieLibX/Libraries/STXCPP/src','~/IndieLibX/Libraries/aras-p-hlsl2glslfork-de4cd58','~/IndieLibX/Libraries/assimp--2.0.863-sdk','~/IndieLibX/Libraries/MeshRenderer','~/IndieLibX/Libraries/otl-nightly/otl/src/tinyxml','~/IndieLibX/Libraries/Framework3/src','~/IndieLibX/Libraries/IndieLib/IndieLib/Common/LibSources']

rd=['~/IndieLibX/Libraries/IndieLib/IndieLibSamples/IndieLib-Animation-Collision/src','~/IndieLibX/Libraries/IndieLib/IndieLibSamples/IndieLib-TankEnemies/src','~/IndieLibX/Libraries/IndieLib/IndieLibSamples/IndieLib-Blend/src','~/IndieLibX/Libraries/IndieLib/IndieLibSamples/IndieLib-TankEnemiesParallax/src','~/IndieLibX/Libraries/IndieLib/IndieLibSamples/IndieLib-Fading/src','~/IndieLibX/Libraries/IndieLib/IndieLibSamples/IndieLib-TankFollowsMouseFires/src','~/IndieLibX/Libraries/IndieLib/IndieLibSamples/IndieLib-FloatingText/src','~/IndieLibX/Libraries/IndieLib/IndieLibSamples/IndieLib-Transitions/src','~/IndieLibX/Libraries/IndieLib/IndieLibSamples/IndieLib-ParallaxScroll/src']

rd=['~/IndieLibX/Libraries/IndieLib/IndieLib_SDK/tutorials/source/a_01_Installing/common','~/IndieLibX/Libraries/IndieLib/IndieLib_SDK/tutorials/source/a_09_IND_3dMesh/common','~/IndieLibX/Libraries/IndieLib/IndieLib_SDK/tutorials/source/b_02_Blitting_2d_Directly/common','~/IndieLibX/Libraries/IndieLib/IndieLib_SDK/tutorials/source/a_02_IND_Surface/common','~/IndieLibX/Libraries/IndieLib/IndieLib_SDK/tutorials/source/a_10_IND_Light/common','~/IndieLibX/Libraries/IndieLib/IndieLib_SDK/tutorials/source/b_03_Blitting_3d_Directly/common','~/IndieLibX/Libraries/IndieLib/IndieLib_SDK/tutorials/source/a_03_IND_Image/common','~/IndieLibX/Libraries/IndieLib/IndieLib_SDK/tutorials/source/a_11_Animated_Tile_Scrolling/common'  ,'~/IndieLibX/Libraries/IndieLib/IndieLib_SDK/tutorials/source/b_04_Several_ViewPorts/common','~/IndieLibX/Libraries/IndieLib/IndieLib_SDK/tutorials/source/a_04_IND_Animation/common','~/IndieLibX/Libraries/IndieLib/IndieLib_SDK/tutorials/source/a_13_2d_Camera/common','~/IndieLibX/Libraries/IndieLib/IndieLib_SDK/tutorials/source/c_01_Alien_BenchMark/common','~/IndieLibX/Libraries/IndieLib/IndieLib_SDK/tutorials/source/a_05_IND_Font/common','~/IndieLibX/Libraries/IndieLib/IndieLib_SDK/tutorials/source/a_14_3d_Camera/common','~/IndieLibX/Libraries/IndieLib/IndieLib_SDK/tutorials/source/c_02_Rabbits_BenchMark/common','~/IndieLibX/Libraries/IndieLib/IndieLib_SDK/tutorials/source/a_06_Primitives/common','~/IndieLibX/Libraries/IndieLib/IndieLib_SDK/tutorials/source/a_15_Parallax_Scrolling/common','~/IndieLibX/Libraries/IndieLib/IndieLib_SDK/tutorials/source/a_07_IND_Input/common','~/IndieLibX/Libraries/IndieLib/IndieLib_SDK/tutorials/source/a_16_IND_Timer/common','~/IndieLibX/Libraries/IndieLib/IndieLib_SDK/tutorials/source/a_08_Collisions/common','~/IndieLibX/Libraries/IndieLib/IndieLib_SDK/tutorials/source/b_01_IND_Surface_Grids/common']

rd=['~/IndieLibX/Samples/2DAnimation','~/IndieLibX/Samples/hello-GLES2','~/IndieLibX/Samples/hello-SDL','~/IndieLibX/Samples/Image','~/IndieLibX/Samples/ParticleVS','~/IndieLibX/Samples/RGB8','~/IndieLibX/Samples/Blending','~/IndieLibX/Samples/NatureScene/src','~/IndieLibX/Samples/RadeonTerrainDemo/src']

for i in rd1:
	os.path.walk(i, myfun, 3)

INDIELIBROOT='../../..'

def load(file):
    print '33'
    xml_file = os.path.abspath(file)
    f=open(xml_file,'r')
    lines=f.readlines()
    f.close()
    lXmlAsString=string.join(lines,'')
    return lXmlAsString

def txt2array(txt):
    r = re.compile(r'\s*', re.MULTILINE)
    l=r.split(txt)
    l2=[]
    for i in l:
        l2.append(i.strip())
    return l2

class Project:
    def __init__(self):        
	#self.os='LinuxDarwin'
        #self.osl=[]
	self.name=''
        self.filename=''
        self.dep=[]
        self.text=''
        self.type=''
        self.srcs=[]
        self.incs=[]
        self.flags=[]
        self.libs=[]
        self.sharedlibs=[]
        self.active=True
        self.srcdir=''
        self.exedir='bin'

    def getactive(self):
        return self.active# AND (self.os in self.oss)
    def getname(self):
        return self.name
    def getfilename(self):
        return self.filename
    def setname(self,a):
        self.name=a
    def setfilename(self,a):
        self.filename=a
    def setdep(self,a):
        self.dep=a
    def getdep(self):
        return self.dep

    def settext(self,a):
        self.text=a
        pos=0
        regex1 = re.compile(r'(\<.*?\>\s*\=\s*\<.*?\>)', re.S|re.M)
        regex2 = re.compile(r'\<(.*?)\>\s*\=\s*\<(.*?)\>', re.S|re.M)
        l = regex1.findall(self.text)
        for i in l:
            match2 = regex2.match(i)
            if not match2:
                break
            var, val = match2.groups()

            var=var.strip()
            val=val.strip()
            #print 'var='+var
            #print 'val='+val
            ##raw_input('ProjectsLNX.input:')
            if var=='LOCAL_MODULE':
                self.name=val
            elif var=='UNICODE':
                print 'UNICODE'
            elif var=='LOCAL_EXE_DIR':
                self.exedir=val
            elif var=='LOCAL_INCLUDES':
                self.incs=txt2array(val)
            elif var=='INACTIVE':
                self.active=False
            elif var=='LOCAL_STATIC_LIBRARIES':
                self.libs=txt2array(val)
            elif var=='LOCAL_SHARED_LIBRARIES':
                self.sharedlibs=txt2array(val)
            elif var=='LOCAL_STATIC_LIBRARIES':
                self.libs=txt2array(val)
            elif var=='LOCAL_EXTRA_FLAGS':
                self.flags=txt2array(val)
            elif var=='CONSTRUCT_SHARED_LIBRARY':
                self.type='CONSTRUCT_SHARED_LIBRARY'
            elif var=='CONSTRUCT_STATIC_LIBRARY':
                self.type='CONSTRUCT_STATIC_LIBRARY'
            elif var=='CONSTRUCT_APP':
                self.type='CONSTRUCT_APP'
            elif var=='LOCAL_LDLIBS':
                print 'LOCAL_LDLIBS'
            elif var=='SDL_PATH':
                print 'SDL_PATH'
            elif var=='LOCAL_SHARED_LIBRARIES':
                print 'LOCAL_SHARED_LIBRARIES'
            elif var=='LOCAL_FLAGS':
                self.flags=txt2array(val)
            elif var=='CONSTRUCT_SHARED_LIBRARY':
                self.type='CONSTRUCT_SHARED_LIBRARY'
            elif var=='CONSTRUCT_STATIC_LIBRARY':
                self.type='CONSTRUCT_STATIC_LIBRARY'
            elif var=='CONSTRUCT_APP':
                self.type='CONSTRUCT_APP'
            elif var=='LOCAL_LDLIBS':
                print 'LOCAL_LDLIBS'
            elif var=='SDL_PATH':
                print 'SDL_PATH'
            elif var=='LOCAL_SRC_DIR':
                self.srcdir=val
            elif var=='LOCAL_INC_FILES':
		print 'LOCAL_INC_FILES'
	    elif var=='LOCAL_SRC_FILES':
                #s=val.find('LOCAL_SRC_FILES')
                #print val
                self.srcs=txt2array(val)
                if 0:
                    s2=val[0:len(val)].find('<')
                    print s2
                    if s2>0:
                        e=val[s2:len(val)].find('>')
                        print e
                        if e>0:
                            self.srcs=txt2array(val[s2:e])
                i=0
                for iiii in self.srcs:
                    self.srcs[i]=self.srcdir+'/'+self.srcs[i]
                    i=i+1

            else:
                print 'var='+var
                print 'val='+val
                exit()

    def gettext(self):
        return self.text
    def settype(self,a):
        self.type=a
    def gettype(self):
        return self.type
    def getexedir(self):
        return self.exedir

    def setincs(self,a):
        self.incs=txt2array(a)
    def getincs(self):
        r=' '
        if len(self.incs)>0:
            #-I/usr/include/wine -I/usr/include/wine/windows 
            r+='-I'+INDIELIBROOT+'/'+string.join(self.incs,' -I'+INDIELIBROOT+'/')+' '
        else:
            r=''
	r=r.replace('-I../../../Dependencies/BoostWorkaround0','')
        r=r.replace('-I../../../Dependencies/BoostWorkaround','')
        return r
    def setsrcs(self,a):
        self.srcs=txt2array(a)
	new=[]
	for i in self.srcs:
		new.append(os.path.abspath(i).replace(cwd+'/','./'))
	self.srcs=new
    def getsrcs(self):
        #return ' '+INDIELIBROOT+'/'+string.join(self.srcs,' \\\n        '+INDIELIBROOT+'/')+'\n'
        return ' '+INDIELIBROOT+'/'+string.join(self.srcs,' \\\n\t'+INDIELIBROOT+'/')+'\n'

    def getsrcsrm(self):
        r='$(DEL) '+INDIELIBROOT+'/'+string.join(self.srcs,' '+INDIELIBROOT+'/')+' '#'\n'
        r=r.replace('.cpp','.o')
        r=r.replace('.cxx','.o')
        r=r.replace('.c','.o')
        r=r.replace('.cc','.o')
        r=r.replace('.m','.o')
        return r+'\n\t$(DEL)  $(TARGET)\n'
    #-lSDL -lGL -lGLU -lCg -lCgGL
    #SDL2.lib OPENGL32.LIB GLU32.lib cg.lib cgGL.lib cgD3D8.lib cgD3D9.lib cgD3D10.lib  cgD3D11.lib
    def getlibs(self):
        r=''
        if len(self.libs)>0 and len(self.sharedlibs)>0:
            r=' -L '+INDIELIBROOT+'/lib -l'+string.join(self.libs,' -l')+' -l'+string.join(self.sharedlibs,' -l')+' -lSDL -lstdc++ -lm '
        elif len(self.libs)>0:
            r=' -L '+INDIELIBROOT+'/lib -l'+string.join(self.libs,' -l')+' -lSDL -lstdc++ -lm '
        elif len(self.sharedlibs)>0:
            r=' -L '+INDIELIBROOT+'/lib -l'+string.join(self.sharedlibs,' -l')+' -lSDL -lstdc++ -lm '
        r=r.replace('-l -l','-l')
        return r
    def setflags(self,a):
        self.flags=txt2array(a)
    def getflags(self):
        r=''
        if len(self.flags)>0:
            r=' -D'+string.join(self.flags, ' -D')
        if r==' -D':
            return ''
        return r


def fil2projects(fil):
    print '230'
    projects0=fil.split('<CLEAR_VARS>=<>')
    projects=[]
    for ii in projects0:
        p=Project()
        p.settext(ii)
        projects.append(p)
	print p.getsrcs()
    return projects

def run(file):
	fil=load(file)#'Projects.txt')
 	projects=fil2projects(fil)

run('../DesktopProjects/ProjectsILxIndieLibDesktop.txt')

