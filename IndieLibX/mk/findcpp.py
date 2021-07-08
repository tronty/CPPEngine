import os, sys, re

def find(filePath):
    input = open(filePath)
    text=input.read()
    input.close()
    v=re.findall(r"\bmemcmp\b", text)
    if len(v):
        print filePath+':'
    v=re.findall(r"\bmemcpy\b", text)
    if len(v):
        print filePath+':'
    v=re.findall(r"\bmemset\b", text)
    if len(v):
        print filePath+':'
    return

def myfun(dummy, dirr, filess):
	for child in filess:
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

L=['c3dbool', 'spine-2.3', 'STX', 'MeshRenderer2', 'Framework', 'Framework3', 'NatureSceneLibrary', 'NSGUI', 'otl', 'otl_', 'otl-nightly', 'RadeonTerrainDemoLibrary', 'Image3', 'SDKmesh', 'SDKmesh_', 'IndieLib']
for l in L:
	os.path.walk('../Libraries/'+l, myfun, 13)
os.path.walk('../Samples', myfun, 13)

