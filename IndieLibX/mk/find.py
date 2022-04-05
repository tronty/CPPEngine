import os, sys, re

def replaceString(filePath):
    input = open(filePath)
    text=input.read()
    input.close()

    text=re.sub(r'\btexture\b', 'texture2D', text)
    text=re.sub(r'\btexelFetch\b', 'texture2D', text)
    text=re.sub(r'\btextureLod\b', 'texture2D', text)

    return text

def replaceStringInFile(filePath):
    "replaces all string by a regex substitution"
    #print filePath
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
	#print dummy, dirr, filess
	for child in filess:
	    #print child
	    
	    if '.shd' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
	    	    replaceStringInFile(dirr+'/'+child)
	    
	    if '.glsl' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
	    	    replaceStringInFile(dirr+'/'+child)

v=['../../IndieLib_resources/shd/']

for i in v:
	os.path.walk(i, myfun, 13)

