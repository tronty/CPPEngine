import os, sys, re, zipfile, shutil

def make_executable(path):
	mode = os.stat(path).st_mode
	mode |= (mode & 0444) >> 2    # copy R bits to X
	os.chmod(path, mode)

zipsrcfile='/media/tronty/Transcend/Paketit/Paketit1/25 tammi 2011/Paketit/NatureScene.zip'

#vsrcsGUI=['GUI/GUIEvent.cpp', 'GUI/GUISlider.cpp', 'GUI/GUIButton.cpp', 'GUI/GUICheckBox.cpp', 'GUI/GUISeparator.cpp', 'GUI/GUIRadioButton.cpp', 'GUI/GUITabbedPanel.cpp', 'GUI/GUIText.cpp', 'GUI/GUIClippedRectangle.cpp', 'GUI/GUITexCoordDescriptor.cpp', 'GUI/GUIFrame.cpp', 'GUI/GUIFont.cpp', 'GUI/GUIRectangle.cpp', 'GUI/GUIFontManager.cpp', 'GUI/GUILabel.cpp', 'GUI/GUIComboBox.cpp', 'GUI/GUITextBox.cpp', 'GUI/GUIAlphaElement.cpp', 'GUI/GUIMaterialSurface.cpp', 'GUI/GUIPanel.cpp']

#vsrcsGUI=['Parsers/XMLParser.cpp', 'Parsers/IOXMLObject.cpp', 'Appearance/Appearance.cpp', 'Appearance/Texture.cpp', 'Tools/Font.cpp', 'Tools/String.cpp', 'Tools/NamedObject.cpp', 'Managers/MediaPathManager.cpp', 'Managers/TexturesManager.cpp', 'Nodes/Transform3D.cpp']

vsrcs=['Managers/GeometryManager.cpp', 'Viewer/Frustum.cpp','Viewer/Camera.cpp', 'Nodes/Shape.cpp', 'Nodes/TransformGroup.cpp', 'Geometry/Geometry.cpp', 'Geometry/BoundsDescriptor.cpp', 'Terrain/TerrainUtils.cpp', 'Terrain/TerrainCell.cpp', 'Terrain/HeightMap.cpp', 'Terrain/GrassCell.cpp', 'Terrain/Portal.cpp', 'Terrain/Terrain.cpp', 'SceneFrame.cpp', 'Main.cpp']

headerfiles='../Samples/NatureScene/src'

dstdir='../Samples/NatureScene2/src'

def replaceStringInFile(filePath):
	#print '#1'
	"replaces all string by a regex substitution"
	tempName=filePath+'~~~'
	print filePath
	input = open(filePath)
	outtext=input.read()
	input.close()
	
	output = open(tempName,'w')

	#outtext=re.sub(r'\bstx_rand\b','rand', outtext)
	#outtext=re.sub(r'\bstx_srand\b','srand', outtext)
	outtext=re.sub(r'\bTuple3<T>','D3DXFROMWINEVECTOR3', outtext)
	outtext=re.sub(r'\bTuple2f\b','D3DXFROMWINEVECTOR2', outtext)
	outtext=re.sub(r'\bTuple3f\b','D3DXFROMWINEVECTOR3', outtext)
	outtext=re.sub(r'\bTuple4f\b','D3DXFROMWINEVECTOR4', outtext)
	outtext=re.sub(r'\bTuple2i\b','D3DXFROMWINEVECTOR2', outtext)
	outtext=re.sub(r'\bTuple3i\b','D3DXFROMWINEVECTOR3', outtext)
	outtext=re.sub(r'\bTuple4i\b','D3DXFROMWINEVECTOR4', outtext)
	outtext=re.sub(r'\bMatrix4f\b','D3DXFROMWINEMATRIX', outtext)
	outtext=re.sub(r'\bImage\b','Image3', outtext)
	outtext=re.sub(r'\bString\b','std::string', outtext)
	outtext=re.sub(r'\bNULL\b','0', outtext)
	
	outtext=re.sub(r'\bGL_RGBA','FORMAT_RGBA', outtext)
	outtext=re.sub(r'\bGL_RGB','FORMAT_RGB', outtext)

	outtext=re.sub(r'\bKeyEvent\b','GUIEvent', outtext)
	outtext=re.sub(r'\bCamera\b','NSCamera', outtext)
	outtext=re.sub(r'\bTexture\b','TextureID', outtext)

	outtext=re.sub(r'\bfastSquareRoot\b','sqrtf', outtext)

	outtext=re.sub(r'\bmemcpy\b','stx_memcpy', outtext)
	outtext=re.sub(r'\bmemset\b','stx_memset', outtext)

	outtext=re.sub(r'\bGLuint\b','unsigned int', outtext)
	outtext=re.sub(r'\bGLint\b','int', outtext)
	outtext=re.sub(r'\bGLubyte\b','ubyte', outtext)

	outtext=re.sub(r'\bLogger\:\:writeErrorLog\(','LOG_PRINT(', outtext)
	outtext=re.sub(r'\bLogger\:\:writeInfoLog\(','LOG_PRINT(', outtext)
	outtext=re.sub(r'\bLogger\:\:','//Logger::', outtext)
	outtext=re.sub(r'\breturn LOG_PRINT\b','{ /* return */ LOG_PRINT', outtext)

	outtext=re.sub(r'\bgl','//gl', outtext)
	outtext=re.sub(r'\#include \"\.\.\/\.\.\/','#include \"../', outtext)

	outtext=re.sub(r'\#include \"\.\.\/Math/MathUtils\.h\"','#include <Framework3/IRenderer.h>', outtext)
	outtext=re.sub(r'\#include \"\.\.\/Tools/Logger\.h\"','#include <Framework3/IRenderer.h>', outtext)

	outtext=re.sub(r'\#include \"\.\.\/Image3/Image3\.h\"','#include <Framework3/IRenderer.h>', outtext)
	outtext=re.sub(r'\#include \"\.\.\/Tools/MediaInfo\.h\"','#include <Framework3/IRenderer.h>', outtext)


	outtext=re.sub(r'\#include \"\.\.\/Renderer/Renderer\.h\"','#include <Framework3/IRenderer.h>', outtext)
	output.write(outtext)
	output.close()
	
	os.unlink(filePath)
	os.rename(tempName,filePath)
	#print '#2'

def replaceStringInFile2(filePath, from_, to):
	"replaces all string by a regex substitution"
	tempName=filePath+'~~~'
	print filePath
	input = open(filePath)
	outtext=input.read()
	input.close()
	
	output = open(tempName,'w')

	#outtext=outtext.replace(from_, to)
	outtext=re.sub(from_, to, outtext)

	output.write(outtext)
	output.close()
	
	os.unlink(filePath)
	os.rename(tempName,filePath)

def myfun3(dummy, dirr, filess):
	#print dummy, dirr, filess
	for child in filess:
	    print child
	    
	    if '.fx' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
	    	    replaceStringInFile(dirr+'/'+child)
	    	    print child
	    
	    if '.shd' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
	    	    replaceStringInFile(dirr+'/'+child)
	    	    print child

def myfun2(dummy, dirr, filess):
	#print dummy, dirr, filess
	for child in filess:
	    #print child
	    
	    if '.py' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
	    	    replaceStringInFile(dirr+'/'+child)
	    	    print child
	
def myfun(dummy, dirr, filess):
	#print '#3'
	#print dummy, dirr, filess
	for child in filess:
	    print child
	    if '.py' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile(dirr+'/'+child)
		    print child
	    '''
	    if '.py' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
	    	    replaceStringInFile(dirr+'/'+child)
	    	    print child
	    
	    if '.sh' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
	    	    replaceStringInFile(dirr+'/'+child)
	    	    print child
	    
	    if '.bat' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
	    	    replaceStringInFile(dirr+'/'+child)
	    	    print child

	    '''
	    if '.h' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
	    	    replaceStringInFile(dirr+'/'+child)
	    	    print child
	    if '.inl' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
	    	    replaceStringInFile(dirr+'/'+child)
	    	    print child
	    if '.hxx' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile(dirr+'/'+child)
		    print child
	    if '.hpp' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile(dirr+'/'+child)
		    print child
	    
	    if '.c' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile(dirr+'/'+child)
		    print child
	    
	    if '.cc' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile(dirr+'/'+child)
		    print child
	    if '.cxx' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile(dirr+'/'+child)
		    print child
	    
	    if '.cpp' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile(dirr+'/'+child)
		    print child
	    if '.cs' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile(dirr+'/'+child)
		    print child
	    if '.java' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile(dirr+'/'+child)
		    print child
	    if '.pl' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile(dirr+'/'+child)
		    print child
	    
	    if '.rb' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile(dirr+'/'+child)
		    print child

#print '#4'

v1=['../Libraries/NatureSceneLibrary','../Samples/NatureScene']
#,'otl']

v2=['../Samples/NatureScene2/src']
v2=[]
for i in v2:
	os.path.walk(i, myfun, 13)

#print '#5'

zip_ref = zipfile.ZipFile(zipsrcfile, 'r')
if os.path.exists(dstdir):
	shutil.rmtree(dstdir)

os.makedirs(dstdir)
zip_ref.extractall(dstdir)
zip_ref.close()

v=['Appearance','Extensions','Icons','Parsers','Tools','Image','Math','Renderer','Events','GUI','lib','Shaders','Window']
for i in v:
	shutil.rmtree('../Samples/NatureScene2/src/'+i)   

txt='''
rm -fr ../Samples/NatureScene2/src/Application\ Nature/Data
rm ../Samples/NatureScene2/src/Application\ Nature/SceneGraph.exe
rm ../Samples/NatureScene2/src/Application\ Nature/SceneGraph.suo
rm ../Samples/NatureScene2/src/Application\ Nature/SceneGraph.sln
rm ../Samples/NatureScene2/src/Application\ Nature/SceneGraph.vcproj

rm ../Samples/NatureScene2/src/Nodes/TransformGroup.cpp
rm ../Samples/NatureScene2/src/Nodes/TransformGroup.h
rm ../Samples/NatureScene2/src/Nodes/Shape.cpp
rm ../Samples/NatureScene2/src/Nodes/Shape.h
Transform3D
Sprite
Particles
rm ../Samples/NatureScene2/src/Managers/TexturesManager.cpp
rm ../Samples/NatureScene2/src/Managers/TexturesManager.h
rm ../Samples/NatureScene2/src/Managers/MediaPathManager.cpp
rm ../Samples/NatureScene2/src/Managers/MediaPathManager.h
rm ../Samples/NatureScene2/src/Managers/WindowsManager.cpp
rm ../Samples/NatureScene2/src/Managers/WindowsManager.h

mv ../Samples/NatureScene2/src/Application\ Nature/Terrain ../Samples/NatureScene2/src
mv ../Samples/NatureScene2/src/Application\ Nature/SceneFrame.cpp ../Samples/NatureScene2/src
mv ../Samples/NatureScene2/src/Application\ Nature/SceneFrame.h ../Samples/NatureScene2/src
mv ../Samples/NatureScene2/src/Application\ Nature/Main.cpp ../Samples/NatureScene2/src

rm -fr ../Samples/NatureScene2/src/Application\ Nature
'''
f=open('NS.sh','w')
f.write(txt)
f.close()
make_executable('NS.sh')
os.system('./NS.sh')

os.path.walk('../Samples/NatureScene2/src', myfun, 13)

