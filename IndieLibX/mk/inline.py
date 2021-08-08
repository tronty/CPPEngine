# -*- coding: utf-8 -*-
# Python

tempName='/home/tronty/IndieLibX/Libraries/Framework3/include/Framework3/IRenderer2.inl'

X='''
/home/tronty/IndieLibX/Libraries/Framework3/include/Framework3/IRenderer.h
473
1482
/home/tronty/IndieLibX/Libraries/STX/include/STX/STXinterfaces.h
430
475
511
529
884
1004
1854
1892
'''
f=['/home/tronty/IndieLibX/Libraries/Framework3/include/Framework3/IRenderer.h','/home/tronty/IndieLibX/Libraries/STX/include/STX/STXinterfaces.h']
s=[473, 884, 430,  511,  1341, 1854]
e=[1293, 1004, 475, 529,  1449, 1892]

ff=['STX_Service::GetInputInstance()->','STX_Service::Get1Instance()->','STX_Service::GetWindowInstance()->','STX_Service::GetAudioInstance()->','STX_Service::Get2Instance()->','STX_Service::Get3Instance()->']


import os, sys, re

def raw_string(s):
    if isinstance(s, str):
        s = s.encode('string-escape')
    elif isinstance(s, unicode):
        s = s.encode('unicode-escape')
    return s

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

class Project:
    def __init__(self):
	self.name=''
        self.params=''
        self.params2=''
    def __init__(self, afnname, aparams, aparams2):
	self.name=afnname
        self.params=aparams
        self.params2=aparams2
    def getsign(self):
        return self.name
    def getfnname(self):
	v=self.name.split()
        return v[len(v)-1]
    def getparams(self):
        return self.params
    def getparams2(self):
        return self.params2

def swapElements(aList):
    i  = 0
    while( i < len(aList) - 1 ):
        if (aList[i].getfnname() > aList[i+1].getfnname()):
            temp = aList[i]
            aList[i] = aList[i+1]
            aList[i+1] = temp
        i = i+1
           

def bubbleSort(aList):
    for num in range(len(aList) - 1 ):
        swapElements(aList)
    return aList

def replaceStringInFile(output,filePath,ss,e,ff,j):
   "replaces all string by a regex substitution"
   #tempName=filePath+'~~~'
   input = open(filePath)
   s=input.read().split('\n')
   #outtext = s
   #print s

   txt='\n'.join(s[ss:e])
   #print txt

   l=[]
   if j=='virtual':
   	p = re.compile(r"\bvirtual\b\s+\b[a-zA-Z0-9_* \t]+?\b\s*\(.*?\)")
   	l=p.findall(txt)
   elif j=='static':
   	p = re.compile(r"\bstatic\b\s+\b[a-zA-Z0-9_* \t]+?\b\s*\(.*?\)")
   	l=p.findall(txt)
   #print l
   rrr=''
   v=[]
   for i in l:
	m=False
	if j=='virtual':
   		p = re.compile(r"\bvirtual\b\s+\b([a-zA-Z0-9_* \t]+?)\b\s*\((.*?)\)")
   		m=p.match(i)
	elif j=='static':
   		p = re.compile(r"\bstatic\b\s+\b([a-zA-Z0-9_* \t]+?)\b\s*\((.*?)\)")
   		m=p.match(i)
	if m:
		#print 'x'
		
   		#p2 = re.compile(r"Vrtl\b")
   		#m2=p2.match(m.group(1))
		#if m2:
		#	continue

		txt2=(m.group(1)+'('+m.group(2)+')').replace('*','').replace('&','').replace(')','')
		fn=stripfnnam(txt2)
		pn=stripparnam(txt2)
		#print pn
		#print 'inline '+m.group(1)+'('+m.group(2)+')\n{\n\treturn IRenderer::GetRendererInstance()->'+m.group(1)+'('+pn+');\n}\n'

		xx=m.group(1).replace('*','').replace('&','').replace(')','')
		p=Project(xx, m.group(2), pn)
		v.append(p)

   for i in bubbleSort(v):
	print i.getfnname()
   	p = re.compile(r"\bvoid\b")
   	m=p.match(i.getsign())
	if not m:
		#print '1'
		txt='inline '+i.getsign()+'('+i.getparams()+')\n{\n\treturn '+ff+i.getfnname()+'('+i.getparams2()+');\n}\n'
		output.write(txt.replace(''+i.getfnname()+'('+i.getfnname()+'(',i.getfnname()+'('))
	else:
		#print '2'
		txt='inline '+i.getsign()+'('+i.getparams()+')\n{\n\t'+ff+i.getfnname()+'('+i.getparams2()+');\n}\n'
		output.write(txt.replace(''+i.getfnname()+'('+i.getfnname()+'(',i.getfnname()+'('))
        
  

output = open(tempName,'w')
v=['virtual','static']
for j in v:
	output.write('#ifndef _IRendererX2_H_\n')
	output.write('#define _IRendererX2_H_\n')
	for i in range(2,4):
		replaceStringInFile(output,f[1],s[i],e[i],ff[i],j)
	output.write('#endif\n')
output.close()


