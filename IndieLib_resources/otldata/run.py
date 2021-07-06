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

	#outtext=re.sub(r'\"\, \.\./\.\./IndieLib_resources/otldata/', '", "../../IndieLib_resources/otldata/', outtext)
	outtext=re.sub(r'\>data\/', '>otldata/', outtext)

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
	    if '.xml' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile(dirr+'/'+child)
		    print child

#print '#4'
v=['.']
for i in v:
	os.path.walk(i, myfun, 13)
#os.system('rm -fr ../otl')
#os.system('cp -fr ./otl ../otl')

#print '#5'

