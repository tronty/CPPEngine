import os, sys, re

patn1='''.lnx'''
replacemt1='.lnx'
patn2='''LNX'''
replacemt2='LNX'

def replaceStringInFile(filePath):
	"replaces all string by a regex substitution"
	tempName=filePath+'~~~'
	print filePath
	input = open(filePath)
	outtext=input.read()
	input.close()
	
	output = open(tempName,'w')
	
	outtext = outtext.replace(patn1,replacemt1)
	outtext = outtext.replace(patn2,replacemt2)
	
	output.write(outtext)
	output.close()
	
	os.unlink(filePath)
	os.rename(tempName,filePath)
	
def myfun(dummy, dirr, filess):
	#print dummy, dirr, filess
	for child in filess:
	    print child
	    if '.py' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
	    	    replaceStringInFile(dirr+'/'+child)

os.path.walk('.', myfun, 3)

