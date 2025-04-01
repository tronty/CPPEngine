import os, sys, re

def replaceStringInFile(filePath):
    print filePath
    "replaces all string by a regex substitution"
    tempName=filePath+'~~~'
    input = open(filePath)
    outtext=input.read()
    input.close()

    outtext=re.sub(r'\bRenderer\.', 'IRenderer::GetRendererInstance()->', outtext)

    '''
    outtext=re.sub(r'\bclass\b', 'struct', outtext)
    outtext=re.sub(r'\bpackage\b', '//package', outtext)
    outtext=re.sub(r'\bimport\b', '//import', outtext)
    outtext=re.sub(r'\bpublic\s*?', '', outtext)
    outtext=re.sub(r'\bprotected\s*?', '', outtext)
    outtext=re.sub(r'\bprivate\s*?', '', outtext)

    outtext=re.sub(r'\bthis\.', 'this->', outtext)
    outtext=re.sub(r'\bboolean\b', 'bool', outtext)
    outtext=re.sub(r'\bstatic\s*?', '', outtext)

    # var type:int;
    # , y:int, size:Number,
		public var rotate_offset:Point;
		public var visible:Boolean;
    var bitmap:Bitmap;
		public var bitmap:Bitmap;
		public static var Renderer:BitmapData;
		
		private var ship:Ship
		
		private var keys_down:Array;
		private var keys_up:Array;
    
    outtext=re.sub(r'\bString\b', 'std::string', outtext)
    outtext=re.sub(r'\bArray\b', 'std::vector', outtext)
    outtext=re.sub(r'\bMath\.round\b', 'roundf', outtext)
    outtext=re.sub(r'\bMath\.PI\b', 'D3DXFROMWINE_PI', outtext)
    outtext=re.sub(r'\bMath\.sin\b', 'sinf', outtext)
    outtext=re.sub(r'\bMath\.cos\b', 'cosf', outtext)
    outtext=re.sub(r'\bBitmapData\b', 'Image3', outtext)
    outtext=re.sub(r'\bPoint\b', 'D3DXFROMWINEVECTOR2', outtext)
    outtext=re.sub(r'\bNumber\b', 'float', outtext)
    outtext=re.sub(r'////', '//', outtext)
    outtext=re.sub(r'(\w+)\:(\w+)', r'\g<2> \g<1>', outtext)
    outtext=re.sub(r'\bvar\s*?', '', outtext)

    outtext=re.sub(r'\boverride\b', '', outtext)
    outtext=re.sub(r'\bfunction\b', '', outtext)

    outtext=re.sub(r'\bMatrix\b', 'D3DXFROMWINEMATRIX', outtext)
    outtext=re.sub(r'\bGame\.Renderer\.', 'IRenderer::GetRendererInstance()->', outtext)		
    outtext=re.sub(r'\bsuper\s*\.', '', outtext)
    outtext=re.sub(r'\btrace\b', '//trace', outtext)
    outtext=re.sub(r'\bBoolean\b', 'bool', outtext)
    outtext=re.sub(r'\.push\(new\b', '.push_back(', outtext)
    outtext=re.sub(r'\bBitmap\b', 'Image3', outtext)

    outtext=re.sub(r'\bIRenderer\:\:GetRendererInstance\(\)\-\>width\b', 'STX_Service::GetWindowInstance()->GetWidth', outtext)
    outtext=re.sub(r'\bIRenderer\:\:GetRendererInstance\(\)\-\>height\b', 'STX_Service::GetWindowInstance()->GetHeight', outtext)

    outtext=re.sub(r'\bMath\.floor\b', '', outtext)
    outtext=re.sub(r'\bMath\.random\b', '', outtext)
    '''
    '''
  Copyright (c) 2019 Tuomas Roenty   http://www.tuomasronty.fi/
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

files=['./src']
for i in files:
	os.path.walk(i, myfun, 13)

