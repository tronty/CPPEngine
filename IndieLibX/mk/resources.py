import os, sys, re

def replaceString(outtext):
	if True:
		outtext=re.sub(r'//uniform //float', '//uniform float', outtext)
		outtext=re.sub(r'//uniform //int', '//uniform int', outtext)
	else:
		outtext=re.sub(r'\buniform\s+float\s+time\s*;', '//uniform float time;', outtext)
		outtext=re.sub(r'\bfloat\s+time\s*;', '//float time;', outtext)
		outtext=re.sub(r'\buniform\s+int\s+time\s*;', '//uniform int time;', outtext)
		outtext=re.sub(r'\bint\s+time\s*;', '//int time;', outtext)
		outtext=re.sub(r'\bDWORD\s+time\s*;', '//DWORD time;', outtext)
	return outtext

def replaceStringInFile(filePath):
    "replaces all string by a regex substitution"
    tempName=filePath+'~~~'
    input = open(filePath)
    outtext=input.read()
    input.close()
    outtext=replaceString(outtext)

    output = open(tempName,'w')
    output.write(outtext)
    output.close()
    os.unlink(filePath)
    os.rename(tempName,filePath)

def myfun(dummy, dirr, filess):
	for child in filess:
	    if '.fx' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile(dirr+'/'+child)
	    elif '.hlsl' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile(dirr+'/'+child)
	    elif '.glsl' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile(dirr+'/'+child)

v=['../../IndieLib_resources']
for i in v:
	os.path.walk(i, myfun, 13)



