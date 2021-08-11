#
# Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#

import os, sys, re

def replaceStringInFile(filePath):
	"replaces all string by a regex substitution"
	tempName=filePath+'~~~'
	print filePath
	input = open(filePath)
	outtext=input.read()
	input.close()
	
	output = open(tempName,'w')

	outtext=re.sub(r'\brand\(',r'randint(0, ', outtext)

	output.write(outtext)
	output.close()
	
	os.unlink(filePath)
	os.rename(tempName,filePath)
	
def myfun(dummy, dirr, filess):
	#print dummy, dirr, filess
	for child in filess:
	    #print child
	    #continue
	    
	    if '.py' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile(dirr+'/'+child)
		    print child

v=['../Samples/jbmaze-code/python']
for i in v:
	os.path.walk(i, myfun, 13)

