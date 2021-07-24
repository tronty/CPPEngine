import re
import os
import string

ind='''import IndieLib

$(global)
$(declarations)
def init()$(init)

def render()$(render)

def run():
	IndieLib.CIndieLib.Instance();
	IndieLib.CIndieLib.Instance().Init("$(title)");
	
	t=IndieLib.IND_Timer()
	t.Start()
	v=IndieLib.D3DXFROMWINEVECTOR4( 400.0, 500.0, 0.0, 0.0 )
	
	while ((not IndieLib.CIndieLib.Instance().Input.OnKeyPress (IndieLib.KEY_ESCAPE)) and (not IndieLib.CIndieLib.Instance().Input.Quit())):
		IndieLib.CIndieLib.Instance().Input.Update ();
		IndieLib.CIndieLib.Instance().Render.ClearViewPort (60, 60, 60);
		IndieLib.CIndieLib.Instance().Render.BeginScene ();
		IndieLib.CIndieLib.Instance().Render.EndScene ();	
	print t.GetTicks()
	print v.x
	IndieLib.CIndieLib.Instance().End();

if __name__=="__main__":
	run()'''

#~/IndieLibGLES2_/Libraries/IndieLib/IndieLibSamples/
#~/IndieLibGLES2_/Libraries/IndieLib/IndieLib_SDK/

#virtual int init(unsigned int width, unsigned int height)
#virtual int render()
#GameLogic gl;

samples='''./IndieLib-TankFollowsMouseFires/src/main.cpp
./IndieLib-ParallaxScroll/src/main.cpp
./IndieLib-Animation-Collision/src/main.cpp
./IndieLib-Blend/src/main.cpp
./IndieLib-Fading/src/main.cpp'''

tutorials='''./tutorials/source/a_06_Primitives/common/Main_vc2008.cpp
./tutorials/source/b_01_IND_Surface_Grids/common/Main_vc2008.cpp
./tutorials/source/a_05_IND_Font/common/Main_vc2008.cpp
./tutorials/source/b_04_Several_ViewPorts/common/Main_vc2008.cpp
./tutorials/source/a_15_Parallax_Scrolling/common/Main_vc2008.cpp
./tutorials/source/a_04_IND_Animation/common/Main_vc2008.cpp
./tutorials/source/a_09_IND_3dMesh/common/Main_vc2008.cpp
./tutorials/source/a_11_Animated_Tile_Scrolling/common/Main_vc2008.cpp
./tutorials/source/a_16_IND_Timer/common/Main_vc2008.cpp
./tutorials/source/b_03_Blitting_3d_Directly/common/Main_vc2008.cpp
./tutorials/source/a_08_Collisions/common/Main_vc2008.cpp
./tutorials/source/a_13_2d_Camera/common/Main_vc2008.cpp
./tutorials/source/b_02_Blitting_2d_Directly/common/Main_vc2008.cpp
./tutorials/source/a_02_IND_Surface/common/Main_vc2008.cpp
./tutorials/source/a_14_3d_Camera/common/Main_vc2008.cpp
./tutorials/source/a_10_IND_Light/common/Main_vc2008.cpp
./tutorials/source/a_03_IND_Image/common/Main_vc2008.cpp
./tutorials/source/a_07_IND_Input/common/Main_vc2008.cpp
./tutorials/source/a_01_Installing/common/Main_vc2008.cpp
'''

cl='''./IndieLib-TankEnemiesParallax/src/main.cpp
./IndieLib-TankEnemiesParallax/src/PropFighter.cpp
./IndieLib-TankEnemiesParallax/src/explosion.cpp
./IndieLib-TankEnemiesParallax/src/Bullet.cpp
./IndieLib-FloatingText/src/main.cpp
./IndieLib-FloatingText/src/FloatingText.cpp
./IndieLib-TankEnemies/src/main.cpp
./IndieLib-TankEnemies/src/PropFighter.cpp
./IndieLib-TankEnemies/src/explosion.cpp
./IndieLib-TankEnemies/src/Bullet.cpp
./IndieLib-Transitions/src/main.cpp
./IndieLib-Transitions/src/Transition.cpp
./tutorials/source/c_01_Alien_BenchMark/common/CUfo.cpp
./tutorials/source/c_01_Alien_BenchMark/common/CTerrain.cpp
./tutorials/source/c_01_Alien_BenchMark/common/Main_vc2008.cpp
./tutorials/source/c_02_Rabbits_BenchMark/common/CRabbit.cpp
./tutorials/source/c_02_Rabbits_BenchMark/common/Main_vc2008.cpp'''

def invglobal(txt):
	r = re.compile(r'\n', re.MULTILINE)
    	v=r.split(txt)
	txt2=''
	for i in v:
		w=i.split()
		if not len(w)>0:
			continue
		if w[0]=='char':
			txt2=txt2+w[1]+'=\'\'\n'
		elif w[0]=='int':
			txt2=txt2+w[1]+'=0\n'
		elif w[0]=='float':
			txt2=txt2+w[1]+'=0.0\n'
	return txt2

def invdecl(txt):
	r = re.compile(r'\n', re.MULTILINE)
    	v=r.split(txt)
	txt2=''
	for i in v:
		w=i.split()
		print 'w='
		print w
		if len(w)<1:
			continue
		txt2=txt2+w[1]+'='+w[0]+'()\n'
	return txt2

def txt2arrayinv(txt):
    x='''
    txt=re.sub(r'\s+', ' ', txt)
    r = re.compile(r';', re.MULTILINE)
    l=r.split(txt)
    l2=[]
    for i in range(len(l),0):
        l2.append(l[i])
    '''
    return txt#'\n'.join(l2)

def txt2array(txt):
    txt=re.sub(r'\s+', '', txt)
    r = re.compile(r';', re.MULTILINE)
    l=r.split(txt)
    l2=[]
    for i in l:
        l2.append(i.replace('\n','').strip())
    return '\n'.join(l2)

def repl(txt):
	txt=re.sub(r'/\*.*?\*/', '', txt, flags=re.MULTILINE)
	txt=re.sub(r'//.*?\n', '\n', txt)#, flags=re.IGNORECASE)
	txt=re.sub(r'#.*?\n', '\n', txt)#, flags=re.IGNORECASE)
	
	
	txt=txt.replace('(int)','')
	txt=txt.replace('(float)','')
	txt=txt.replace('INDIELIBRESOURCES"','"../resources')
	txt=txt.replace('IND_','IndieLib.IND_')
	txt=txt.replace('KEY_','IndieLib.KEY_')
	txt=txt.replace('\\\\','/')
	txt=txt.replace(';','')
	txt=txt.replace('&','')
	txt=txt.replace('::','.')
	txt=txt.replace('->','.')

	txt=txt.replace('mI.','IndieLib.CIndieLib.Instance().')
	txt=txt.replace('g_pIndieLib=mI=CIndieLib.Instance()','IndieLib.CIndieLib.Instance()')
	
	txt=re.sub(r'\)return', ' return ', txt)#, flags=re.IGNORECASE)
	txt=txt.replace('return0','return 0')
	txt=txt.replace('return -1','return -1')
	txt=txt.replace('if(!','if (not ')
	txt=re.sub(r'\belseif\b','elif', txt)

	return txt

def indent(txt):
	indentation=1
	txt2=''
	for i in range(0,len(txt)):
		if txt[i]=='{':
			indentation=indentation+1
			txt2=txt2+':\n'
		elif txt[i]=='}':
			indentation=indentation-1
		elif txt[i]=='\n':
			txt2=txt2+'\n'
			for ii in range(0,indentation):
				txt2=txt2+'\t'
		else:
			txt2=txt2+txt[i]

	#txt=txt.replace('{',':')
	#txt=txt.replace('}','')
	return txt2

def load(file):
    #xml_file = os.path.abspath(file)
    xml_file = file.replace('~',os.environ['HOME']+'')
    f=open(xml_file,'r')
    lines=f.readlines()
    f.close()
    lXmlAsString=string.join(lines,'')
    return lXmlAsString

def write1(name,name0):
	txt0=load(name0)
	txt0=txt0.replace('virtual int render()=0;','')
	txt0=txt0.replace('virtual int init(unsigned int width, unsigned int height)=0;','')

	p1=txt0.find('virtual int init(unsigned int width, unsigned int height)')+len('virtual int init(unsigned int width, unsigned int height)')
	p22=txt0.find('virtual int render()')	
	p21=txt0.find('virtual int render()')+len('virtual int render()')
	p3=txt0.find('GameLogic gl;')

	p4=txt0.find('class IGameLogic')

	p5=txt0.find('virtual ~GameLogic(){}')+len('virtual ~GameLogic(){}')
	p6=txt0.find('virtual int init(unsigned int width, unsigned int height')

	txt=ind	
	txt=txt.replace('$(title)',name)
	txt=txt.replace('$(init)',indent(txt2array(txt0[p1:p22])))
	txt=txt.replace('$(render)',indent(txt2array(txt0[p21:p3])))
	txt=txt.replace('$(declarations)',invdecl(indent(txt2arrayinv(txt0[p5:p6]))))
	txt=txt.replace('$(global)',invglobal(indent(txt2arrayinv(txt0[0:p4]))))
	txt=repl(txt)
       
	fn='../scripting/'+name+'.py'
	fn=fn.replace('?.','.')
	f=open(fn,'w')
	f.write(txt)
	f.close()

def write2(v1,v2):
	f=open('../scripting/run.py','w')
	n=''
	for i in v1:
		n=i.replace('./','').replace('/src/main.cpp','').replace('-','_')
		f.write('import '+n+'\n')
	for i in v2:
		n=i.replace('./tutorials/source/','').replace('/common/Main_vc2008.cpp','').replace('-','_')
		f.write('import '+n+'\n')

	for i in v1:
		n=i.replace('./','').replace('/src/main.cpp','').replace('-','_')
		f.write(n+'.run()\n')
	for i in v2:
		n=i.replace('./tutorials/source/','').replace('/common/Main_vc2008.cpp','').replace('-','_')
		f.write(n+'.run()\n')

	#import x 
	#x.run()
	f.close()

v1=samples.split()
for i in v1:
	n0=i.replace('./','~/IndieLibGLES2_/Libraries/IndieLib/IndieLibSamples/')
	n=i.replace('./','').replace('/src/main.cpp','').replace('-','_')
	print n
	write1(n,n0)

v2=tutorials.split()
for i in v2:
	n0=i.replace('./','~/IndieLibGLES2_/Libraries/IndieLib/IndieLib_SDK/')
	n=i.replace('./tutorials/source/','').replace('/common/Main_vc2008.cpp','').replace('-','_')
	print n
	write1(n,n0)
write2(v1,v2)


