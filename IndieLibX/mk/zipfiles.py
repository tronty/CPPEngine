f=['amd-dev.wpengine.netdna-cdn.com.wordpress.media.2012.10','ATI SDK','developer.download.nvidia.com.SDK.10','Microsoft DirectX SDK (June 2010)','NVIDIA Corporation','NVIDIA Direct3D SDK 11','www.humus.name']

f1=['Asiakirjat/zips/www.humus.name','Asiakirjat/zips/amd-dev.wpengine.netdna-cdn.com.wordpress.media.2012.10','Asiakirjat/zips/developer.download.nvidia.com.SDK.10']

f2=['Asiakirjat/zips/Microsoft DirectX SDK (June 2010)/Samples/C++/Direct3D11','Asiakirjat/zips/Microsoft DirectX SDK (June 2010)/Samples/C++/Direct3D10','Asiakirjat/zips/Microsoft DirectX SDK (June 2010)/Samples/C++/Direct3D','Asiakirjat/zips/Microsoft DirectX SDK (June 2010)/Samples/C++/DXUT11/Core','Asiakirjat/zips/Microsoft DirectX SDK (June 2010)/Samples/C++/DXUT11/Optional','Asiakirjat/zips/Microsoft DirectX SDK (June 2010)/Samples/C++/DXUT/Core','Asiakirjat/zips/Microsoft DirectX SDK (June 2010)/Samples/C++/DXUT/Optional','Asiakirjat/zips/Microsoft DirectX SDK (June 2010)/Samples/C++/Effects11','Asiakirjat/zips/ATI SDK/Samples/DirectX 9.0','Asiakirjat/zips/ATI SDK/Samples/OpenGL','Asiakirjat/zips/ATI SDK/Samples/Framework']

f3=['Asiakirjat/zips/SDK 9.5/DEMOS/Direct3D9/inc','Asiakirjat/zips/SDK 9.5/DEMOS/Direct3D9/src','Asiakirjat/zips/SDK 9.5/DEMOS/OpenGL/inc','Asiakirjat/zips/SDK 9.5/DEMOS/OpenGL/src','Asiakirjat/zips/NVIDIA Direct3D SDK 11/Include','Asiakirjat/zips/NVIDIA Direct3D SDK 11/source']

import os, sys, re, string
	
def myfun(dummy, dirr, filess):
	#print dummy, dirr, filess
	for child in filess:
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
	    if '.m' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile(dirr+'/'+child)
		    print child

for i in rd1:
	os.path.walk(i, myfun, 3)

