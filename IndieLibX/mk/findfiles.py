import os, sys, re

def find(filePath):
    input = open(filePath)
    text=input.read()
    input.close()
    v=re.findall(r"\bApplicationLogic\b", text)
    if len(v):
        print filePath+':'
    return
	
def myfun(dummy, dirr, filess):
	#print dummy, dirr, filess
	for child in filess:
	    #print child
	    
	    if '.cpp' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
	    	    find(dirr+'/'+child)
	    
	    if '.cxx' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
	    	    find(dirr+'/'+child)
	    
	    if '.cc' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
	    	    find(dirr+'/'+child)
	    
	    if '.c' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
	    	    find(dirr+'/'+child)
	    
	    if '.h' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
	    	    find(dirr+'/'+child)
	    
	    if '.hxx' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
	    	    find(dirr+'/'+child)
	    
	    if '.inl' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
	    	    find(dirr+'/'+child)

v=['../Samples/new']

for i in v:
	os.path.walk(i, myfun, 13)

