#!/usr/bin/env python

import os
from os import path
import sys
import re

from os.path import join, getsize
def search(dir):
	ext=['csln']
	for root, dirs, files in os.walk(dir):
    		c=len(files)
    		ii=0
    		while ii<c:
    			file=files[ii]
    			#dir=dirs[ii]
    			ii=ii+1
    			for i in ext:
    				p = re.compile( r'^[a-zA-Z0-9\_\-\\\/ ]+\.'+i+'$')
    				m=p.match(file)
    				if m:
    					#print "csln2sln: "+ os.path.join(root, file)
    					##os.unlink(os.path.join(root, file))
					cwd=os.getcwd()
					os.chdir(root)
					#sln2csln(os.path.join('', file),'', file)
					os.chdir(cwd)
					



def main():
	for root, dirs, files in os.walk('C:\IndieLib'):    			
		for dir in dirs:
			search(dir)

main()
