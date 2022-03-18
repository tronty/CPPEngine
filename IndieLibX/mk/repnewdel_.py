import os, sys, re

def replaceString(outtext):
    outtext=re.sub(r'\bgl_FragCoord\b', 'xlv_TEXCOORD0', outtext)
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
	#print dummy, dirr, filess
	for child in filess:
	    print child
	    
	    if '.hlsl' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
	    	    print dirr+'/'+child
	    	    replaceStringInFile(dirr+'/'+child)
	    
	    if '.glsl' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
	    	    print dirr+'/'+child
	    	    replaceStringInFile(dirr+'/'+child)
	    
	    if '.shd' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
	    	    print dirr+'/'+child
	    	    replaceStringInFile(dirr+'/'+child)



os.path.walk('../../IndieLib_resources/shd', myfun, 13)

