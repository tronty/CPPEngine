import os, sys, re, math
from PIL import Image
from os import listdir
from os.path import isfile, join
import fnmatch

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
		ln+='''<DT><P ALIGN="CENTER"><img src="%s" alt="Screenshot(%s)"> %s</P></DT>''' % (s, tail, name)
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
	    if '.png' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile(dirr+'/'+child)
		    #print child

def main(fn='screenshots256x256'):
	#v=['../../public_html']
	#for i in v:
	#	os.path.walk(i, myfun, 13)
	#print len(v_), math.sqrt(len(v_))

	path='../../public_html/'+fn
	files = []
	# r=root, d=directories, f = files
	for r, d, f in os.walk(path):
		for file in f:
        		if '.png' in file:
            			files.append(os.path.join(r, file))

	files.sort()
	for f in files:
		print f
	print len(files)


	f=open(fn+'.html','w')
	ln='<HTML>\n<HEAD>\n</HEAD>\n<BODY>\n<TITLE>Screenshots</TITLE>\n<H1>Screenshots</H1>\n<TABLE>\n'
	f.write(ln)
	for y in range(0,20):
		ln='<TR>'
		for x in range(0,20):
			f_=files[20*y+x]
			f_=f_.replace('../../public_html/'+fn+'/','./')
			ln_='<TD><img src="%s" alt="Screenshot(%s)"></TD>' % (f_,f_)
			ln+=ln_
		f.write(ln+'</TR>\n')
	ln='</TABLE>\n</BODY>\n</HTML>\n'
	f.write(ln)
	f.close()

main()

