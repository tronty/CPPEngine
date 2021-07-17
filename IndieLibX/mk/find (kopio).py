import os, sys, re

def find(filePath):
    input = open(filePath)
    text=input.read()
    input.close()
    print filePath+':'
    v=re.findall(r"\bnoise\b", text)
    for i in v:
        print i
    return
    v=re.findall(r"\"(.*?\.fx)\"", text)
	for i in v:
		print i

	v=re.findall(r"\"(.*?\.shd)\"", text)
	for i in v:
		print i

	v=re.findall(r"\"(.*?\.png)\"", text)
	for i in v:
		print i

	v=re.findall(r"\"(.*?\.jpg)\"", text)
	for i in v:
		print i

	v=re.findall(r"\"(.*?\.tga)\"", text)
	for i in v:
		print i
	
def myfun(dummy, dirr, filess):
	#print dummy, dirr, filess
	for child in filess:
	    #print child
	    
	    if '.shd' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
	    	    find(dirr+'/'+child)
        return
	    
	    x='''
	    if '.py' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
	    	    find(dirr+'/'+child)
	    	    #print child
	    
	    if '.sh' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
	    	    find(dirr+'/'+child)
	    	    #print child
	    
	    if '.bat' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
	    	    find(dirr+'/'+child)
	    	    #print child
	    '''
	    
	    if '.h' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
	    	    find(dirr+'/'+child)
	    	    #print child
	    if '.inl' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
	    	    find(dirr+'/'+child)
	    	    #print child
	    if '.hxx' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    find(dirr+'/'+child)
		    #print child
	    if '.hpp' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    find(dirr+'/'+child)
		    #print child
	    
	    if '.c' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    find(dirr+'/'+child)
		    #print child
	    if '.cc' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    find(dirr+'/'+child)
		    #print child
	    if '.cxx' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    find(dirr+'/'+child)
		    #print child
	    
	    if '.cpp' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    find(dirr+'/'+child)
		    #print child

v=['../../IndieLib_resources/www.shadertoy.com']

for i in v:
	os.path.walk(i, myfun, 13)

