import os, sys, re

def inserttabInFile(filePath):
	tempName=filePath+'~~~'
	input = open(filePath)
	v=input.read().split('\n')
	input.close()

	v2=[]
	c=1
	for i in v:
		if c >= 351:
			v2.append('\t'+i)
		else:
			v2.append(i)
		c=c+1

   	outtext='\n'.join(v2)
	output = open(tempName,'w')
	output.write(outtext)
	output.close()
	
	os.unlink(filePath)
	os.rename(tempName,filePath)

#351,6161

inserttabInFile('/home/tronty/IndieLibX/DesktopProjects/Projects.py')

