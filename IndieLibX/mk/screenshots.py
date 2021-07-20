import os, sys, re
from PIL import Image
from os import listdir
from os.path import isfile, join

fscreenshots=open('../../public_html/screenshots/screenshots_.html','w')
f800x660=open('../../public_html/screenshots/800x660.html','w')
f800x600=open('../../public_html/screenshots/800x600.html','w')
f640x512=open('../../public_html/screenshots/640x512.html','w')
felse=open('../../public_html/screenshots/else.html','w')
v800x660=[]
v800x600=[]
v640x512=[]
velse=[]

def replaceStringInFile(filePath):
	'''
	output = open(tempName,'w')
	output.write(outtext)
	output.close()
	'''
	i1=Image.open(filePath)
	if (i1.size[0]==800) and (i1.size[1]==660):
		v800x660.append(filePath)
		ln='%s: (%d, %d)\n' % (i, i1.size[0], i1.size[1])
		ln='''<DT><a target="_blank" href="%s">%s</A></DT>\n''' % (filePath, filePath)
		ln='''<DT><img src="%s" alt="Screenshot(%s)"></DT>\n''' % (filePath, filePath)
		ln='''<DT><P ALIGN="CENTER"><img src="%s" alt="Screenshot(%s)"> %s</P>''' % (filePath, filePath, filePath)
		ln=ln.replace('../../public_html/','')
		ln=ln.replace('screenshots/','')
		ln=ln.replace('dx9_','../dx9_')
		f800x660.write(ln);
	elif (i1.size[0]==800) and (i1.size[1]==600):
		v800x600.append(filePath)
		ln='%s: (%d, %d)\n' % (i, i1.size[0], i1.size[1])
		ln='''<DT><a target="_blank" href="%s">%s</A></DT>\n''' % (filePath, filePath)
		ln='''<DT><img src="%s" alt="Screenshot(%s)"></DT>\n''' % (filePath, filePath)
		ln='''<DT><P ALIGN="CENTER"><img src="%s" alt="Screenshot(%s)"> %s</P>''' % (filePath, filePath, filePath)
		ln=ln.replace('../../public_html/','')
		ln=ln.replace('screenshots/','')
		ln=ln.replace('dx9_','../dx9_')
		f800x600.write(ln);
	elif (i1.size[0]==640) and (i1.size[1]==512):
		v640x512.append(filePath)
		ln='%s: (%d, %d)\n' % (i, i1.size[0], i1.size[1])
		ln='''<DT><a target="_blank" href="%s">%s</A></DT>\n''' % (filePath, filePath)
		ln='''<DT><img src="%s" alt="Screenshot(%s)"></DT>\n''' % (filePath, filePath)
		ln='''<DT><P ALIGN="CENTER"><img src="%s" alt="Screenshot(%s)"> %s</P>''' % (filePath, filePath, filePath)
		ln=ln.replace('../../public_html/','')
		ln=ln.replace('screenshots/','')
		ln=ln.replace('dx9_','../dx9_')
		f640x512.write(ln);
	else:
		#print filePath, i1.size[0], i1.size[1]
		velse.append(filePath)
		ln='%s: (%d, %d)\n' % (i, i1.size[0], i1.size[1])
		ln='''<DT><a target="_blank" href="%s">%s</A></DT>\n''' % (filePath, filePath)
		ln='''<DT><img src="%s" alt="Screenshot(%s)"></DT>\n''' % (filePath, filePath)
		ln='''<DT><P ALIGN="CENTER"><img src="%s" alt="Screenshot(%s)"> %s</P>''' % (filePath, filePath, filePath)
		ln=ln.replace('../../public_html/','')
		ln=ln.replace('screenshots/','')
		ln=ln.replace('dx9_','../dx9_')
		felse.write(ln);

def myfun(dummy, dirr, filess):
	#print '#3'
	#print dummy, dirr, filess
	#onlyfiles = [f for f in listdir(dirr) if isfile(join(dirr, f))]
	#print onlyfiles
	filess = [f for f in listdir(dirr) if isfile(join(dirr, f))]
	#print filess
	for child in filess:
	    #print child
	    if '.jpg' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile(dirr+'/'+child)
		    #print child


ln='''<HTML><HEAD></HEAD><BODY><TITLE>Screenshots</TITLE><H1>Screenshots</H1><DL>\n'
<DT><a target="_blank" href="800x660.html">Screenshots (800, 660)</A></DT>
<DT><a target="_blank" href="800x600.html">Screenshots (800, 600)</A></DT>
<DT><a target="_blank" href="640x512.html">Screenshots (640, 512)</A></DT>
<DT><a target="_blank" href="else.html">Screenshots</A></DT>
</DL><p></DL><p></BODY></HTML>
'''

ln1='<HTML><HEAD></HEAD><BODY><TITLE>Screenshots</TITLE><H1>Screenshots</H1><DL>\n'
ln2='</DL><p></DL><p></BODY></HTML>\n'

fscreenshots.write(ln)
f800x660.write(ln1)
f800x600.write(ln1)
f640x512.write(ln1)
felse.write(ln1)
v=['../../public_html']
for i in v:
	os.path.walk(i, myfun, 13)



f800x660.write(ln2)
f800x600.write(ln2)
f640x512.write(ln2)
felse.write(ln2)
f800x660.close()
f800x600.close()
f640x512.close()
felse.close()
fscreenshots.close()

