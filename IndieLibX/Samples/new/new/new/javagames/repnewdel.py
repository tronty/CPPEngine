import os, sys, re

def replaceStringInFile(filePath):
    print filePath
    "replaces all string by a regex substitution"
    tempName=filePath+'~~~'
    input = open(filePath)
    outtext=input.read()
    input.close()

    outtext=re.sub(r'\bnew Point', ' Point', outtext)
    outtext=re.sub(r'\bnew Rectangle', ' Rectangle', outtext)
    outtext=re.sub(r'\bnew Window', ' Window', outtext)
    outtext=re.sub(r'////', '//', outtext)

    
    outtext=re.sub(r'\bclass\b', 'struct', outtext)
    outtext=re.sub(r'\bpackage\b', '//package', outtext)
    outtext=re.sub(r'\bimport\b', '//import', outtext)
    outtext=re.sub(r'\bassert\b', '//assert', outtext)
    outtext=re.sub(r'\bpublic\s*?', '', outtext)
    outtext=re.sub(r'\bprotected\s*?', '', outtext)
    outtext=re.sub(r'\bprivate\s*?', '', outtext)
    outtext=re.sub(r'\bBufferedImage\b', 'Image3', outtext)

    outtext=re.sub(r'\bthis\.', 'this->', outtext)
    outtext=re.sub(r'\bboolean\b', 'bool', outtext)
    outtext=re.sub(r'\bfinal\b', 'const', outtext)
    outtext=re.sub(r'\bstatic\s*?', '', outtext)

    outtext=re.sub(r'\@Override\s*?', '', outtext)
    outtext=re.sub(r'\babstract\s*?', '', outtext)
    outtext=re.sub(r'\bArrayList', 'std::vector', outtext)

    outtext=re.sub(r'\bthis\s*-\s*>\s*getClass\s*\(\s*\)\s*\.\s*getResource\s*\(\s*\"/', 'IRenderer::GetRendererInstance()->addImageLibTexture(\"/', outtext)

    outtext=re.sub(r'/resources/images/', '/', outtext)
    
    outtext1='''/*
  Copyright (c) 2021 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
    '''
    outtext2='''#endif\n'''
    output = open(tempName,'w')
    #output.write(outtext1)
    output.write(outtext)
    #output.write(outtext2)
    output.close()
    os.unlink(filePath)
    os.rename(tempName,filePath)

def myfun(dummy, dirr, filess):
	for child in filess:
		if '.h' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
			replaceStringInFile(dirr+'/'+child)
			#print child

files=['./src/concurrent-pacman/']
for i in files:
	os.path.walk(i, myfun, 13)

