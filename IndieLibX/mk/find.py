import os, sys, re

def find(filePath):
    input = open(filePath)
    text=input.read()
    input.close()
    v=re.findall(r"\bnoise\b", text)
    if len(v):
        print filePath+':'
    return
	
def myfun(dummy, dirr, filess):
	#print dummy, dirr, filess
	for child in filess:
	    #print child
	    
	    if '.shd' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
	    	    find(dirr+'/'+child)

v=['../../IndieLib_resources/www.shadertoy.com']

for i in v:
	os.path.walk(i, myfun, 13)

