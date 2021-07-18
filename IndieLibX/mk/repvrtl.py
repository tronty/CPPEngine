# -*- coding: utf-8 -*-
# Python

X='''
45236
75449
../Libraries/Framework3/Framework3/include/Framework3/IRenderer.h
15621
45154
../Libraries/Framework3/Framework3/include/Framework3/IRenderer.h
28529
66881
../Libraries/Framework3/Framework3/include/Framework3/RendererHLSLCg.h
'''
s=[45236,15621,28529]
e=[75449,45154,66881]
import os, sys, re

mydir='../Libraries/Framework3/Framework3/include/Framework3'
hhh=['IRenderer.h','RendererHLSLCg.h']
ccc=['DummyRenderer','IRenderer','RendererHLSLCg']
#ddd=['struct RENDERERAPI DummyRenderer','struct RENDERERAPI IRenderer','struct RENDERERAPI RendererHLSLCg']

def stripfnnam(str):
	l=str.split()
	return l[len(l)-1]	

def stripparnam(str):
	l0=str.split(',')
	out=''
	c=0
	for i in l0:
		e=i.find('=')
		o=''
		if e>0:
			o=i[0:e]
		else:
			o=i
		l=o.split()
		o=l[len(l)-1]
		if c>0:
			out=out+', '+o
		else:
			out=o
		c=c+1
	return out
	
def stripdefpar(str):
	l=str.split(',')
	o=''
	c=0
	for i in l:
		s=''
		if c>0:
			s=', '
		c=c+1
		e=i.find('=')
		if e>0:
			o=o+s+i[0:e]
		else:
			o=o+s+i
	return o	

def replaceStringInFile(filePath,c,ss,e,u):
   "replaces all string by a regex substitution"
   tempName=filePath+'~~~'
   input = open(filePath)
   output = open(tempName,'w')
   s=input.read()
   outtext = s
   z='''
   outtext = re.sub(patn, replacemt, s)
   outtext = re.sub(patn2, replacemt2, outtext)
   #outtext = re.sub(patn3, replacemt3, outtext)
   #outtext = re.sub(patn4, replacemt4, outtext)
   '''
   zz='''
   ss=s.find('struct RENDERERAPI '+c)
   print ss
   if ss>0:
   	e=ss+s[ss:len(s)].find('};')
	print e
   '''
   '''
   p = re.compile(r"\bvirtual\b.*?\(.*?\)\s*\{")
   l=p.findall(s[ss:e])
   for i in l:	
   	p = re.compile(r"\bvirtual\b(.*?)\((.*?)\)")
   	m=p.match(i)
	if m:
		if m.group(2).find('=')>0:
			print i
   exit()
   '''
   p = re.compile(r"\bvirtual\b.*?\(.*?\)\s*\=.*?\;")
   l=p.findall(s[ss:e])
   #print l
   rrr=''
   for i in l:	
   	p = re.compile(r"\bvirtual\b(.*?)\((.*?)\)(\s*\=.*?\;)")
   	m=p.match(i)
	if m:
		if m.group(2).find('=')>0:
			if not u=='DummyRenderer':
				rrr='virtual'+m.group(1)+'Vrtl('+stripdefpar(m.group(2))+')'+m.group(3)+'\n\ninline'+m.group(1)+'('+m.group(2)+'){'+stripfnnam(stripdefpar(m.group(1)))+'Vrtl('+stripparnam(m.group(2))+');}'
			else:
				rrr='virtual'+m.group(1)+'('+stripdefpar(m.group(2))+')'
   
   			outtext=outtext.replace('virtual'+m.group(1)+'('+m.group(2)+')'+m.group(3),rrr)

   output.write(outtext)
   output.close()
   input.close()
   os.rename(tempName,filePath)
   print filePath

replaceStringInFile(mydir+'/'+hhh[0],ccc[0],s[0],e[0],'DummyRenderer')
replaceStringInFile(mydir+'/'+hhh[0],ccc[1],s[1],e[1],'IRenderer')
replaceStringInFile(mydir+'/'+hhh[1],ccc[2],s[2],e[2],'RendererHLSLCg')
x='''
def myfun(dummy, dirr, filess):
    for child in filess:
        if '.sln' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
            replaceStringInFile(dirr+'/'+child)
            print child
	
        if '.h' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
            replaceStringInFile(dirr+'/'+child)
            print child
	elif '.hxx' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
            replaceStringInFile(dirr+'/'+child)
            print child
	elif '.hpp' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
            replaceStringInFile(dirr+'/'+child)
            print child
	elif '.c' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
            replaceStringInFile(dirr+'/'+child)
            print child
	elif '.cxx' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
            replaceStringInFile(dirr+'/'+child)
            print child
	elif '.cpp' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
            replaceStringInFile(dirr+'/'+child)
            print child
	
os.path.walk(mydir, myfun, 3)
'''

