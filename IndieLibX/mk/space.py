import re
import os
import string
import sys

def load(file):
    xml_file = os.path.abspath(file)
    f=open(xml_file,'r')
    lines=f.readlines()
    f.close()
    lines2=[]
    for i in lines:
	r = re.compile(r'\w+')#, re.MULTILINE)
    	m=r.match(i)
	if not m:
		continue
	lines2.append(i)
    lXmlAsString=string.join(lines2,'')
    print lXmlAsString
	
def myfun(dummy, dirr, filess):
	#print dummy, dirr, filess
	for child in filess:
	    print child
	    if '.h' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
	    	    load(dirr+'/'+child)
	    	    print child
	    if '.inl' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
	    	    load(dirr+'/'+child)
	    	    print child
	    if '.hxx' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    load(dirr+'/'+child)
		    print child
	    if '.hpp' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    load(dirr+'/'+child)
		    print child
	    if '.c' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    load(dirr+'/'+child)
		    print child
	    if '.cxx' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    load(dirr+'/'+child)
		    print child
	    if '.cpp' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    load(dirr+'/'+child)
		    print child

dir=os.environ['HOME']+'/IndieLibX/Samples/NatureScene/src0'
os.path.walk(dir, myfun, 3)

