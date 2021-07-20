import os, sys, re

def replaceStringInFile(filePath):
	#print '#1'
	"replaces all string by a regex substitution"
	tempName=filePath+'~~~'
	print filePath
	input = open(filePath)
	outtext=input.read()
	input.close()
	
	output = open(tempName,'w')

	outtext=re.sub(r'\bNSGUI/ArialSmall.png\b','NSGUI/ArialSmall.tga', outtext)

	output.write(outtext)
	output.close()
	
	os.unlink(filePath)
	os.rename(tempName,filePath)

def myfun(dummy, dirr, filess):
	#print dummy, dirr, filess
	for child in filess:
	    print child
	    
	    if '.xml' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
	    	    replaceStringInFile(dirr+'/'+child)
	    	    print child

v=['../../IndieLib_resources']
for i in v:
	os.path.walk(i, myfun, 13)

#print '#5'


