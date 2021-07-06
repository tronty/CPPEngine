import re

def readfile():
   input = open('../Libraries/Framework3/include/Framework3/gl.linux.h')
   s=input.read()
   #p = re.compile(r"\btypedef\s+.*?\s+\(\s*GLAPIENTRYP\s+PFNGL\w+PROC\s*\)\s+\(.*?\)\s*\;",re.M|re.S|re.I)
   #l2=p.findall(s)
   #p = re.compile(r"\btypedef\s+.*?\s+\(\s*.*?APIENTRY.*?\s+PFNGL\w+PROC\s*\)\s+\(.*?\)\s*\;",re.M|re.S|re.I)
   #l3=p.findall(s)
   #print len(l2)
   #print len(l3)

   #p = re.compile(r"((!define) \w*API).*?",re.M|re.S|re.I)
   #l2=p.findall(s)
   #print l2
   #exit()
   p = re.compile(r'\#define API.*?\n',re.M|re.S|re.I)
   s=p.sub('',s)
   p = re.compile(r'\#define GLAPI.*?\n',re.M|re.S|re.I)
   s=p.sub('',s)
   p = re.compile(r"\b\w*API\s+[a-zA-Z_0-9\* \t]+\s+\w*APIENTRY\s+\w+\s*\(.*?\)\s*\;",re.M|re.S|re.I)
   l2=p.findall(s)
   #p = re.compile(r"\bGLAPI\s+.*?\s+APIENTRY\s+(\w+)\s*\(.*?\)\s*\;",re.M|re.S|re.I)
   #l3=p.findall(s)
   #print len(l2)
   #print len(l3)
   input.close()
   
   input = open('../Libraries/Framework3/include/Framework3/glext.linux.h')
   s=input.read()
   p = re.compile(r'\#define API.*?\n',re.M|re.S|re.I)
   s=p.sub('',s)
   p = re.compile(r'\#define GLAPI.*?\n',re.M|re.S|re.I)
   s=p.sub('',s)
   #print s
   #exit()
   #p = re.compile(r"((!define) \w*API).*?",re.M|re.S|re.I)
   #l2=p.findall(s)
   #print l2
   #exit()
   #p = re.compile(r"\btypedef\s+.*?\s+\(\s*GLAPIENTRYP\s+PFNGL\w+PROC\s*\)\s+\(.*?\)\s*\;",re.M|re.S|re.I)
   #l2=p.findall(s)
   #p = re.compile(r"\btypedef\s+.*?\s+\(\s*APIENTRY.*?\s+PFNGL\w+PROC\s*\)\s+\(.*?\)\s*\;",re.M|re.S|re.I)
   #l3=p.findall(s)
   #print len(l2)
   #print len(l3)
   #p = re.compile(r"\bGLAPI\s+.*?\s+GLAPIENTRY\s+(\w+)\s*\(.*?\)\s*\;",re.M|re.S|re.I)
   #l2=p.findall(s)
   p = re.compile(r"\b\w*API\s+[a-zA-Z_0-9\* \t]+\s+\w*APIENTRY\s+\w+\s*\(.*?\)\s*\;",re.M|re.S|re.I)
   l3=p.findall(s)
   
   '''
   print len(l2)
   print len(l3)
   exit()
   
   print l2
   print l3
   '''
   input.close()
   
   for i in l2:
	p = re.compile(r"\b\w*API\s+?(.*?)\s+.*?APIENTRY\s+(\w+)\s*\((.*?)\)\s*\;",re.M|re.S|re.I)
   	m=p.match(i)
	if m:
		print "\tGLTYPEDEF("+m.group(1)+", "+m.group(2)+", ("+m.group(3)+"));"
   for i in l2:
	p = re.compile(r"\b\w*API\s+?(.*?)\s+.*?APIENTRY\s+(\w+)\s*\((.*?)\)\s*\;",re.M|re.S|re.I)
	m=p.match(i)
	if m:
		print "\tGLEXTERN("+m.group(2)+");"
   for i in l2:
	p = re.compile(r"\b\w*API\s+?(.*?)\s+.*?APIENTRY\s+(\w+)\s*\((.*?)\)\s*\;",re.M|re.S|re.I)
	m=p.match(i)
	if m:
		print "\tGLPROC("+m.group(2)+");"
   for i in l2:
	p = re.compile(r"\b\w*API\s+?(.*?)\s+.*?APIENTRY\s+(\w+)\s*\((.*?)\)\s*\;",re.M|re.S|re.I)
	m=p.match(i)
	if m:
		print "\tGLINIT("+m.group(2)+");"






   for i in l3:
	p = re.compile(r"\b\w*API\s+?(.*?)\s+.*?APIENTRY\s+(\w+)\s*\((.*?)\)\s*\;",re.M|re.S|re.I)
   	m=p.match(i)
	if m:
		print "\tGLTYPEDEF("+m.group(1)+", "+m.group(2)+", ("+m.group(3)+"));"
   for i in l3:
	p = re.compile(r"\b\w*API\s+?(.*?)\s+.*?APIENTRY\s+(\w+)\s*\((.*?)\)\s*\;",re.M|re.S|re.I)
	m=p.match(i)
	if m:
		print "\tGLEXTERN("+m.group(2)+");"
   for i in l3:
	p = re.compile(r"\b\w*API\s+?(.*?)\s+.*?APIENTRY\s+(\w+)\s*\((.*?)\)\s*\;",re.M|re.S|re.I)
	m=p.match(i)
	if m:
		print "\tGLPROC("+m.group(2)+");"
   for i in l3:
	p = re.compile(r"\b\w*API\s+?(.*?)\s+.*?APIENTRY\s+(\w+)\s*\((.*?)\)\s*\;",re.M|re.S|re.I)
	m=p.match(i)
	if m:
		print "\tGLINIT("+m.group(2)+");"


   x='''
   for i in l3:
	p = re.compile(r"APIENTRY\s+(\w+)\s*\(",re.M|re.S|re.I)
   	m=p.match(i)
	if m:
		print m.group(1)
   '''
readfile()
#readfile('../Libraries/Framework3/include/Framework3/glu.linux.h')
#typedef void (APIENTRYP PFNGLACTIVETEXTUREPROC) (GLenum texture);

#typedef void (APIENTRYP PFNGLBLENDEQUATIONPROC) (GLenum mode);

#GLAPI void GLAPIENTRY glActiveTexture( GLenum texture );
#GLAPI void APIENTRY glBlendColor (GLclampf, GLclampf, GLclampf, GLclampf);
