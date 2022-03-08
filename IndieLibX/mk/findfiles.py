import os, sys, re

def replaceStringInFile(filePath):
    "replaces all string by a regex substitution"
    tempName=filePath+'~~~'
    input = open(filePath)
    outtext=input.read()
    input.close()

    outtext=re.sub(r'\.HLSL.shd\b', '.hlsl', outtext)
    outtext=re.sub(r'\.shd\b', '.hlsl', outtext)

    output = open(tempName,'w')
    output.write(outtext)
    output.close()
    os.unlink(filePath)
    os.rename(tempName,filePath)
    print filePath

def myfun(dummy, dirr, filess):
	for child in filess:
		if '.cpp' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
			replaceStringInFile(dirr+'/'+child)
		if '.cxx' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
			replaceStringInFile(dirr+'/'+child)
		if '.cc' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
			replaceStringInFile(dirr+'/'+child)
		if '.c' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
			replaceStringInFile(dirr+'/'+child)
		if '.hpp' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
			replaceStringInFile(dirr+'/'+child)
		if '.hxx' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
			replaceStringInFile(dirr+'/'+child)
		if '.hh' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
			replaceStringInFile(dirr+'/'+child)
		if '.h' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
			replaceStringInFile(dirr+'/'+child)
		if '.inl' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
			replaceStringInFile(dirr+'/'+child)

v=['../Libraries/Framework3']
v=['../Samples']
for i in v:
	os.path.walk(i, myfun, 13)

