import os, sys, re

def replaceStringInGLFile(filePath):
    "replaces all string by a regex substitution"
    tempName=filePath+'~~~'
    input = open(filePath)
    outtext=input.read()
    input.close()

    #outtext=re.sub(r'\:\s*register\s*\(\s*s%d\s*\)', '', outtext)
    outtext=re.sub(r'\:\s*register\s*\(\s*s0\s*\)', '', outtext)
    outtext=re.sub(r'\:\s*register\s*\(\s*s1\s*\)', '', outtext)
    outtext=re.sub(r'\:\s*register\s*\(\s*s2\s*\)', '', outtext)
    outtext=re.sub(r'\:\s*register\s*\(\s*c0\s*\)', '', outtext)

    output = open(tempName,'w')
    output.write(outtext)
    output.close()
    os.unlink(filePath)
    os.rename(tempName,filePath)

def myfunGL(dummy, dirr, filess):
	for child in filess:
		if '.shd' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
			replaceStringInGLFile(dirr+'/'+child)
			print child

GL=['.']
for i in GL:
	os.path.walk(i, myfunGL, 13)

