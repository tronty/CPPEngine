import os, sys, re

def replaceStringInFile(filePath):
	#print '#1'
	"replaces all string by a regex substitution"
	tempName=filePath+'~~~'
	print filePath
	input = open(filePath)
	outtext=input.read()
	input.close()
	
	outtext=re.sub(r'\bVertex\b', 'OTL_Vertex', outtext)
	outtext=re.sub(r'\bTexture\b', 'OTL_Texture', outtext)
	outtext=re.sub(r'\bShader\b', 'OTL_Shader', outtext)
	
	output = open(tempName,'w')
	output.write(outtext)
	output.close()
	
	os.unlink(filePath)
	os.rename(tempName,filePath)
	#print '#2'

def myfun(dummy, dirr, filess):
	#print '#3'
	#print dummy, dirr, filess
	for child in filess:
	    print child
	    if '.h' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
	    	    replaceStringInFile(dirr+'/'+child)
	    	    print child
	    if '.inl' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
	    	    replaceStringInFile(dirr+'/'+child)
	    	    print child
	    if '.hxx' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile(dirr+'/'+child)
		    print child
	    if '.hpp' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile(dirr+'/'+child)
		    print child
	    
	    if '.c' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile(dirr+'/'+child)
		    print child
	    
	    if '.cc' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile(dirr+'/'+child)
		    print child
	    if '.cxx' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile(dirr+'/'+child)
		    print child
	    
	    if '.cpp' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile(dirr+'/'+child)
		    print child

#print '#4'
v=['./examples', './include', './src']
for i in v:
	os.path.walk(i, myfun, 13)
#os.system('rm -fr ../otl')
#os.system('cp -fr ./otl ../otl')

#print '#5'

