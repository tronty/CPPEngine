#!/bin/bash

MYSTRING="Do something in bash"
echo $MYSTRING

cd /home/tronty/IndieLibX/Samples/RadeonTerrainDemo
rm -fr tmp
unzip -d tmp RadeonTerrainDemoDX9.zip

mv ./tmp/RadeonTerrainDemoDX9/*.* ./tmp
rm -fr ./tmp/RadeonTerrainDemoDX9

cd tmp
rm -fr ATI.ICO sky.dat ground.dat path.dat RadeonTerrainDemo.sln RadeonTerrainDemo.suo terrain.dat RadeonTerrainDemo.vcproj TerrainDemo.rc material.dat
rm -fr texture

cd /home/tronty/IndieLibX/Samples/NatureScene
rm -fr tmp
unzip -d tmp NatureScene.zip

#mv ./tmp/NatureScene/*.* ./tmp
rm -fr ./tmp/NatureScene

cd tmp
rm -fr Extensions Icons Image Renderer lib Window
rm -fr './Application Nature/Data'
mv './Application Nature/Terrain' .

mv './Application Nature/Main.cpp' .
mv './Application Nature/SceneFrame.h' .   
mv './Application Nature/SceneFrame.cpp' .
rm -fr './Application Nature'

mv './Tools/String.cpp' './Tools/NSString.cpp'
mv './Tools/String.h' './Tools/NSString.h'

python - << EOF
myPyString = "Do something on python"
print myPyString

import os, sys, re

d='/home/tronty/IndieLibX/Samples'

def DXtoIL(outtext):

	for i in ['LPDIRECT3D9','LPDIRECT3DDEVICE9','m_pD3D','SAFE_DELETE','SAFE_DELETE_ARRAY','SAFE_RELEASE','#define SAFE_DELETE','#define SAFE_DELETE_ARRAY','#define SAFE_RELEASE','GameState\:\:m\_pd3dDevice\-\>SetTextureStageState','GameState\:\:m\_pd3dDevice\-\>SetSamplerState','GameState\:\:m\_pd3dDevice\-\>SetRenderState']: # prepend with //
		outtext=re.sub(r'\b'+i+'\b','//'+i, outtext)

	v=['LPDIRECT3DINDEXBUFFER9','IndexBufferID','LPDIRECT3DVERTEXBUFFER9','VertexBufferID','LPDIRECT3DTEXTURE9','TextureID','D3DMATERIAL9','D3DFROMWINEMATERIAL9','NULL','0','TCHAR','char','LPSTR','char*','LPCSTR','const char*','LPCTSTR','const char*','HINSTANCE','int','HRESULT','int','LRESULT','int','HWND','int','WPARAM','int','LPARAM','int','GameState\:\:m\_pd3dDevice\-\>','IRenderer::GetRendererInstance()->','TRUE','true','FALSE','false','BOOL','bool']

	for i in range(0,len(v)/2):
		what=r'\b%s\b' % v[2*i+0]
		to=v[2*i+1]
		outtext=re.sub(what, to, outtext)

	outtext=re.sub(r'\bPI\b','D3DXFROMWINE_PI', outtext)
	outtext=re.sub('\bD3DTS\_','TS_', outtext)
	outtext=re.sub('D3DX','D3DXFROMWINE', outtext)
	outtext=re.sub('D3DXFROMWINEFROMWINE','D3DXFROMWINE', outtext)

	outtext=re.sub('\bD3DPT\_TRIANGLELIST\b','PRIM_TRIANGLES', outtext)
	outtext=re.sub('\bD3DPT\_TRIANGLEFAN\b','PRIM_TRIANGLE_FAN', outtext)
	outtext=re.sub('\bD3DPT\_TRIANGLESTRIP\b','PRIM_TRIANGLE_STRIP', outtext)
	#outtext=re.sub('\bD3DPT\_\b','PRIM_QUADS', outtext)
	outtext=re.sub('\bD3DPT\_LINELIST\b','PRIM_LINES', outtext)
	outtext=re.sub('\bD3DPT\_LINESTRIP\b','PRIM_LINE_STRIP', outtext)
	#outtext=re.sub('\bD3DPT\_\b','PRIM_LINE_LOOP', outtext)	
	outtext=re.sub('\bD3DPT\_POINTLIST\b','PRIM_POINTS', outtext)
		
	outtext=re.sub('\b\s+->BeginScene\(.*?\)','IRenderer::GetRendererInstance()->BeginScene()', outtext)
	outtext=re.sub('\b\s+->Clear\(.*?\)','IRenderer::GetRendererInstance()->Clear(true, false, D3DXFROMWINEVECTOR4 (128.0f, 128.0f, 128.0f,1))', outtext)
	outtext=re.sub('\b\s+->EndScene\(.*?\)','IRenderer::GetRendererInstance()->EndScene()', outtext)
	outtext=re.sub('\b\s+->Present\(.*?\)','IRenderer::GetRendererInstance()->Present()', outtext)

	x='''
dev->CreateIndexBuffer(UINT Length,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DIndexBuffer9** ppIndexBuffer);
dev->CreateVertexBuffer(UINT Length,DWORD Usage,DWORD FVF,D3DPOOL Pool,IDirect3DVertexBuffer9** ppVertexBuffer
dev->DrawIndexedPrimitive(D3DPRIMITIVETYPE,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount);
dev->DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount);
dev->DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride);
dev->DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertices,UINT PrimitiveCount,CONST void* pIndexData,D3DFORMAT IndexDataFormat,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride);
dev->GetViewport(D3DVIEWPORT9* pViewport);
dev->SetFVF(DWORD FVF);
dev->SetIndices(IDirect3DIndexBuffer9* pIndexData);
dev->SetMaterial(CONST D3DMATERIAL9* pMaterial);
dev->SetRenderState(D3DRENDERSTATETYPE State,DWORD Value);
dev->SetSamplerState(DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD Value);
dev->SetStreamSource(UINT StreamNumber,IDirect3DVertexBuffer9* pStreamData,UINT OffsetInBytes,UINT Stride);
dev->SetTexture(DWORD Stage,IDirect3DBaseTexture9* pTexture);
dev->SetTextureStageState(DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value);
dev->SetTransform(TS State,CONST D3DMATRIX* pMatrix);
	'''
	return outtext

def GLtoIL(outtext):

	outtext=re.sub(r'\bNULL\b','int', outtext)
	for i in ['WINAPI','HINSTANCE']:
		cl=r'\b%s\b' % i
		outtext=re.sub(cl, 'int', outtext)
	v=['LPSTR','char*','LPCSTR','const char*','LPCTSTR','const char*']
	for i in range(0,len(v)/2):
		what=r'\b%s\b' % v[2*i+0]
		to=v[2*i+1]
		outtext=re.sub(what, to, outtext)

	outtext=re.sub(r'\#include \"\.\.\/Extensions\/GLee\.h\"','#include <Framework3/IRenderer.h>', outtext)

	outtext=re.sub(r'\.load2D\(\"','=IRenderer::GetRendererInstance()->addImageLibTexture("', outtext)

	outtext=re.sub(r'\bImage\b','Image3', outtext)
	outtext=re.sub(r'\bString\.h\"','NSString.h"', outtext)

	outtext=re.sub(r'\bTuple2f\b','D3DXFROMWINEVECTOR2', outtext)
	outtext=re.sub(r'\bTuple3f\b','D3DXFROMWINEVECTOR3', outtext)
	outtext=re.sub(r'\bTuple4f\b','D3DXFROMWINEVECTOR4', outtext)
	outtext=re.sub(r'\bMatrix4f\b','D3DXFROMWINEMATRIX', outtext)

	outtext=re.sub(r'\bTexture\b','TextureID', outtext)
	outtext=re.sub(r'\bShaders\b','ShaderID', outtext)

	outtext=re.sub(r'\bMouseEvent evt\b','IInput* evt', outtext)
	outtext=re.sub(r'\bKeyEvent evt\b','IInput* evt', outtext)
	outtext=re.sub(r'\bWindowEvent &evt\b','IInput* evt', outtext)
	outtext=re.sub(r'\bevt\.','evt->', outtext)

	outtext=re.sub(r'\bglColor3f','IRenderer::GetRendererInstance()->Color3f', outtext)
	outtext=re.sub(r'\bglColor3fv','IRenderer::GetRendererInstance()->Color3fv', outtext)
	outtext=re.sub(r'\bglColor4f','IRenderer::GetRendererInstance()->Color4f', outtext)
	outtext=re.sub(r'\bglColor4fv','IRenderer::GetRendererInstance()->Color4fv', outtext)
	outtext=re.sub(r'\bglTexCoord2f','IRenderer::GetRendererInstance()->TexCoord2f', outtext)
	outtext=re.sub(r'\bglVertex2i','IRenderer::GetRendererInstance()->Vertex2i', outtext)
	outtext=re.sub(r'\bglVertex2f','IRenderer::GetRendererInstance()->Vertex2f', outtext)
	outtext=re.sub(r'\bglVertex2fv','IRenderer::GetRendererInstance()->Vertex2fv', outtext)
	outtext=re.sub(r'\bglVertex3i','IRenderer::GetRendererInstance()->Vertex31', outtext)
	outtext=re.sub(r'\bglVertex3f','IRenderer::GetRendererInstance()->Vertex3f', outtext)
	outtext=re.sub(r'\bglVertex3fv','IRenderer::GetRendererInstance()->Vertex3fv', outtext)
	outtext=re.sub(r'\bglVertex4i','IRenderer::GetRendererInstance()->Vertex4i', outtext)
	outtext=re.sub(r'\bglVertex4f','IRenderer::GetRendererInstance()->Vertex4f', outtext)
	outtext=re.sub(r'\bglVertex4fv','IRenderer::GetRendererInstance()->Vertex4fv', outtext)

	outtext=re.sub(r'\"Data\/','"/home/tronty/IndieLib_resources/NatureScene/', outtext)

	outtext=re.sub(r'\#include \"\.\.\/Renderer\/','//include "../Renderer/', outtext)
	outtext=re.sub(r'\#include \"Terrain\/','#include "../Terrain/', outtext)

	outtext=re.sub('\bGL\_TRIANGLES\b','PRIM_TRIANGLES', outtext)
	outtext=re.sub('\bGL\_TRIANGLE\_FAN\b','PRIM_TRIANGLE_FAN', outtext)
	outtext=re.sub('\bGL\_TRIANGLE\_STRIP\b','PRIM_TRIANGLE_STRIP', outtext)
	#outtext=re.sub('\bGL\_QUADS\b','PRIM_QUADS', outtext)
	outtext=re.sub('\bGL\_LINES\b','PRIM_LINES', outtext)
	outtext=re.sub('\bGL\_LINE\_STRIP\b','PRIM_LINE_STRIP', outtext)
	#outtext=re.sub('\bGL\_LINE\_LOOP\b','PRIM_LINE_LOOP', outtext)	
	outtext=re.sub('\bGL\_POINTS\b','PRIM_POINTS', outtext)

	x='''

map.jpg")

#<string.h>','<string.h>'

#define deleteObject(A){ if(A){ delete   A; A = int; } }
#define deleteArray(A) { if(A){ delete[] A; A = int; } }

GLint
GLuint
GLenum

glActiveStencilFaceEXT(GL_BACK);
glActiveTextureARB(GL_TEXTURE0_ARB + unit);
glAlphaFunc(GL_GEQUAL, alphaReference);
glAttachObjectARB(programID, vertexProcessor.getShaderID());
glBegin(GL_TRIANGLE_STRIP);
glBeginQueryARB(GL_SAMPLES_PASSED_ARB, occlusionID);
glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, indicesVBOID);
glBindProgramARB(shaderTarget, shaderID);
glBindTexture(GL_TEXTURE_2D, id);
glBlendFunc(GL_ONE, GL_ONE);
glBufferDataARB(GL_ARRAY_BUFFER_ARB, TILE_COUNT * sizeof(TVertex),
glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, INDEX_COUNT * sizeof(unsigned short),
glCallLists(stringLength,GL_UNSIGNED_BYTE, string);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
glClientActiveTextureARB(GL_TEXTURE0_ARB + i);

glColorMask(0, 0, 0, 0);
glColorPointer(3, GL_FLOAT, geometry->getStrideSize(), OFFSET(geometry->getOffset(COLOR)));
glCompileShaderARB(shaderID);
glCompressedTexImage2DARB(target,
glCopyTexSubImage2D(newTarget, 0, 0, 0, 0, 0, width, height);
glCreateProgramObjectARB();
glCreateShaderObjectARB(shaderTrueType);
glCullFace(GL_BACK);
glDeleteBuffersARB(1, &geometryBufferID);
glDeleteLists(fontBaseRange, 256);
glDeleteObjectARB(programID);
glDeleteQueriesARB(1, &occlusionID);
glDeleteTextures(1, &textureID);
glDepthBoundsEXT(zMinZMax.x, zMinZMax.y);
glDepthFunc(GL_LEQUAL);
glDepthMask(GL_TRUE);
glDisable(shaderTarget);
glDisableClientState(GL_VERTEX_ARRAY);
glDrawArrays(geometry->getType(), 0, geometry->getIndicesCount());
glDrawElements(GL_TRIANGLES, INDEX_COUNT, GL_UNSIGNED_SHORT, indices);
glDrawRangeElements(GL_TRIANGLES, 0, indexCount, indexCount, GL_UNSIGNED_SHORT, indices);
glEnable();
glEnableClientState(GL_VERTEX_ARRAY);
glEnd();
glEndList();
glEndQueryARB(GL_SAMPLES_PASSED_ARB);
glFlush();
glFrontFace(GL_CCW);
glGenBuffersARB(1, &vboID);;
glGenLists(256);
glGenProgramsARB(1, &shaderID);
glGenQueriesARB(1, &occlusionID);
glGenTextures(1, &id);
glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
glGetInfoLogARB(ID, bufferSize, &dummy, logBuffer);
glGetIntegerv(GL_VIEWPORT, viewport);
glGetObjectParameterivARB(shaderID, GL_OBJECT_COMPILE_STATUS_ARB, &errorLog);
glGetQueryObjectivARB(occlusionID, GL_QUERY_RESULT_AVAILABLE_ARB, &available);
glGetQueryObjectuivARB(occlusionID, GL_QUERY_RESULT_ARB, &visiblePixelsCount);
glGetTexLevelParameteriv(target, 0, GL_TEXTURE_WIDTH , &info);
glGetUniformLocationARB(shaderID, name);
glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
glInitNames();
glIsEnabled(GL_LIGHTING);
glLightf(lightID, GL_CONSTANT_ATTENUATION , attenuation.x);
glLightfv(lightID, GL_AMBIENT,  getAmbient());
glLinkProgramARB(programID);
glListBase(fontBaseRange);
glLoadIdentity();
glLoadMatrixf(viewMatrix);
glLoadName(shapes[i]->getPickingIndex());
glMaterialf(GLenum face, GLenum pname, GLfloat param);
glMaterialfv(GLenum face, GLenum pname, const GLfloat *params);
glMatrixMode(GL_PROJECTION);
glMultMatrixf(worldTransform);
glNewList(fontBaseRange + i,GL_COMPILE);
glNormalPointer(GL_FLOAT, 24, OFFSET(0));
glPointSize(size*2);
glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
glPolygonOffset(0.3f,0.3f);
glPopAttrib();
glPopMatrix();
glProgramEnvParameter4fARB(GL_VERTEX_PROGRAM_ARB, 0, lightPosition.x,  
glProgramEnvParameter4fvARB(shaderTarget,   int(i), attributes4f[i]);
glProgramStringARB(shaderTrueType, GL_PROGRAM_FORMAT_ASCII_ARB,
glPushAttrib(GL_ALL_ATTRIB_BITS);
glPushMatrix();
glPushName(-1);
glRenderMode(GL_RENDER);
glRotatef(angle,0,0,1);
glScalef(width, height,1.0f);
glScissor(scissor[0], scissor[1], scissor[2] - scissor[0], scissor[3] - scissor[1]);
glSelectBuffer(100, selectionBuffer);
glShadeModel(GL_SMOOTH);
glShaderSourceARB(shaderID, 1, (const GLcharARB **)&shaderCode, &length);
glStencilFunc(GL_EQUAL, 0, ~0);
glStencilFuncSeparateATI(GL_ALWAYS, GL_ALWAYS, 0, ~0);
glStencilOp(GL_KEEP, GL_INCR, GL_KEEP);
glStencilOpSeparateATI(GL_BACK,  GL_KEEP, GL_INCR_WRAP_EXT, GL_KEEP);

glTexCoordPointer(GLint, GLenum, GLsizei, GLsizei, const GLvoid *);
glTexEnvf(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);
glTexGenfv(GL_T, plane, planes[1]);
glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, mode);
glTexImage1D   (GL_TEXTURE_1D, 0, inFormat, width, 0, format, GL_UNSIGNED_BYTE, NULL);
glTexImage2D(GL_TEXTURE_2D, 0, inFormat, width, height, 0, format, GL_UNSIGNED_BYTE, NULL);
glTexImage3D   (GL_TEXTURE_3D, 0, inFormat, width, height, depth, 0, format, GL_UNSIGNED_BYTE, NULL);
glTexImage3DEXT(target, 0, GL_RGBA,
glTexParameterf(GLenum target, GLenum pname, GLfloat param );
glTranslatef(GLfloat x, GLfloat y, GLfloat z);
glUniform1fARB(GLint, GLfloat);
glUniform1ivARB(GLint, GLsizei, const GLfloat *);
glUniform2fvARB(GLint, GLsizei, const GLfloat *);
glUniform2ivARB(GLint, GLsizei, const GLint *);
glUniform3fvARB(GLint, GLsizei, const GLfloat *);
glUniform3ivARB(GLint, GLsizei, const GLint *);
glUniform4fvARB(GLint, GLsizei, const GLfloat *);
glUniform4ivARB(GLint, GLsizei, const GLint *);
glUniformMatrix4fv(GLint, GLsizei, GLboolean, const GLfloat *);
glUseProgramObjectARB(GLhandleARB);

glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *ptr);
glViewport(GLint x, GLint y, GLsizei width, GLsizei height);
gluBuild2DMipmaps(...)
gluPerspective(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar)
gluPickMatrix(...);
	'''
	return outtext

def replaceStringInFile(filePath):
	"replaces all string by a regex substitution"
	tempName=filePath+'~~~'
	print filePath
	input = open(filePath)
	outtext=input.read()
	input.close()
	
	output = open(tempName,'w')

	patn1='''NULL'''
	replacemt1='<Texture>'
	patn2='''TextureNS'''
	replacemt2='TextureID'
	
	#outtext = outtext.replace(patn1,replacemt1)

	#outtext=re.sub(r'\bTARGET_IPHONE_SIMULATOR\b','IPHONE_SIMULATOR', outtext)
	#outtext=re.sub(r'\bTARGET_OS_IPHONE\b','OS_IPHONE', outtext)

	print filePath
	
	if 'NatureScene' in filePath.split('/'):
		outtext=GLtoIL(outtext)
	if 'RadeonTerrainDemo' in filePath.split('/'):
		outtext=DXtoIL(outtext)

	output.write(outtext)
	output.close()
	
	os.unlink(filePath)
	os.rename(tempName,filePath)
	
def myfun(dummy, dirr, filess):
	#print dummy, dirr, filess
	for child in filess:
	    print child
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
	    if '.cxx' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile(dirr+'/'+child)
		    print child
	    
	    if '.cpp' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile(dirr+'/'+child)
		    print child

def replaceinfiles1():
	rd=['NatureScene','RadeonTerrainDemo']
	for i in rd:
		os.path.walk(d+'/'+i+'/tmp', myfun, 13)

def main():
	print "In main method"
	replaceinfiles1()

if __name__=="__main__":
	main()

main()

EOF

echo "Back to bash"

