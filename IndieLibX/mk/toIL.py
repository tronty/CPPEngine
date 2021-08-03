#!/bin/bash

MYSTRING="Do something in bash"
echo $MYSTRING

python - << EOF
myPyString = "Do something on python"
print myPyString

import os, sys, re

def DXtoIL():
	os.system('cd ../Samples/RadeonTerrainDemo')
	os.system('rm -fr tmp')
	os.system('unzip -d tmp RadeonTerrainDemoDX9.zip')
	os.system('cd tmp')
	os.system('rm -f ATI.ICO sky.dat ground.dat path.dat RadeonTerrainDemo.sln RadeonTerrainDemo.suo terrain.dat RadeonTerrainDemo.vcproj TerrainDemo.rc material.dat')
	os.system('rm -fr texture')

	x='''

['LPDIRECT3D9','LPDIRECT3DDEVICE9','m_pD3D','SAFE_DELETE','SAFE_DELETE_ARRAY','SAFE_RELEASE','#define SAFE_DELETE','#define SAFE_DELETE_ARRAY','#define SAFE_RELEASE'] # prepend with //
['LPDIRECT3DINDEXBUFFER9','IndexBufferID','LPDIRECT3DVERTEXBUFFER9','VertexBufferID','LPDIRECT3DTEXTURE9','TextureID']

'D3DMATERIAL9','D3DFROMWINEMATERIAL9'

'NULL','0','D3DX','D3DXFROMWINE'

'TCHAR','char','LPSTR','char*','LPCSTR','const char*','LPCTSTR','const char*','HINSTANCE','int','HRESULT','int','LRESULT','int','HWND','int','WPARAM','int','LPARAM','int'

'GameState::m_pd3dDevice->','IRenderer::GetRendererInstance()->'




dev->BeginScene()
dev->Clear(0, NULL, /*D3DCLEAR_TARGET |*/ D3DCLEAR_ZBUFFER,
dev->CreateIndexBuffer(m_dwIndNum*sizeof(DWORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, /*D3DPOOL_MANAGED*/D3DPOOL_DEFAULT, &m_pIndicesIB, NULL);
dev->CreateVertexBuffer( 4*sizeof(D3DVERTEX),D3DUSAGE_WRITEONLY,
dev->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0,0,size, 0, m_dwIndNum/3);
dev->DrawPrimitive(D3DPT_TRIANGLELIST,  0, m_dwGroundNumVert/3 );
dev->EndScene();
dev->GetViewport(&vp);
dev->Present( NULL, NULL, NULL, NULL );
dev->SetFVF(D3DFVF_LVERTEX1);
dev->SetIndices(m_pIndicesIB);
dev->SetMaterial(&m_Mtrl);
dev->SetRenderState(D3DRS_ZENABLE, TRUE);
dev->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
dev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
dev->SetStreamSource( 0, m_pGroundVB, 0, sizeof(LITVERTEX) );
dev->SetTexture(0, pTexture->GetSurface());
dev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
dev->SetTransform(D3DTS_VIEW, &m_matView);
	'''

def GLtoIL():
	os.system('cd ../Samples/NatureScene')
	os.system('rm -fr tmp')
	os.system('unzip -d tmp NatureScene.zip')
	os.system('cd tmp')
	os.system('rm -fr Extensions Icons Image Renderer lib Window')
	os.system('mv ./Application Nature/*.* .')
	os.system('rm -fr Application\ Nature')

	x='''
'.load2D("','=IRenderer::GetRendererInstance()->addImageLibTexture("'

'Image','Image3'
'String\.h\"','NSString.h"'
#<string.h>','<string.h>'

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
glColor3f(1,1,1);
glColor3fv(label.getColor());
glColor4f(1.0f, 1.0f, 1.0f, alphaMark);
glColor4fv(appearance->getColor());
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
glMaterialf(GL_FRONT, GL_SHININESS, shininess);
glMaterialfv(GL_FRONT, GL_AMBIENT , ambient);
glMaterialfv(GL_FRONT, GL_DIFFUSE , diffuse);
glMaterialfv(GL_FRONT, GL_EMISSION, emissive);
glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
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
glTexCoord2f(0.0f,0.0f);
glTexCoordPointer(3, GL_FLOAT, sizeof(GVertex), OFFSET(offset3));
glTexEnvf(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);
glTexGenfv(GL_T, plane, planes[1]);
glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, mode);
glTexImage1D   (GL_TEXTURE_1D, 0, inFormat, width, 0, format, GL_UNSIGNED_BYTE, NULL);
glTexImage2D(GL_TEXTURE_2D, 0, inFormat, width, height, 0, format, GL_UNSIGNED_BYTE, NULL);
glTexImage3D   (GL_TEXTURE_3D, 0, inFormat, width, height, depth, 0, format, GL_UNSIGNED_BYTE, NULL);
glTexImage3DEXT(target, 0, GL_RGBA,
glTexParameterf(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTranslatef(-0.5,-0.5,0.0);
glUniform1fARB(location, attributes1f[i].getElements()[0]);
glUniform1ivARB(location, 1, attributes1i[i].getElements());
glUniform2fvARB(location, 1, attributes2f[i].getElements());
glUniform2ivARB(location, 1, attributes2i[i].getElements());
glUniform3fvARB(location, 1, attributes3f[i].getElements());
glUniform3ivARB(location, 1, attributes3i[i].getElements());
glUniform4fvARB(location, 1, attributes4f[i].getElements());
glUniform4ivARB(location, 1, attributes4i[i].getElements());
glUniformMatrix4fv(location, 1, false, attributes16f[i].getElements());
glUseProgramObjectARB(0);
glUseProgramObjectARB(shaderID);
glVertex2i(windowBounds.x, discYBounds.y);
glVertex3f(position.x + (matrixBuffer[0] - matrixBuffer[4])*size,
glVertex3fv(corners[i]);
glVertexPointer(3, GL_FLOAT, sizeof(TVertex), OFFSET(offset2));
glViewport(0, 0, evt.getWidth(), evt.getHeight());
gluBuild2DMipmaps(target,
gluPerspective(90.0f, (float)window.getWidth()/window.getHeight(), nearClip, farClip);
gluPerspective(fov.x, fov.y, fov.z, fov.w);
gluPickMatrix(mouse.x, mouse.y, rect.x, rect.y, viewport);
	'''

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

	outtext=re.sub(r'\bTARGET_IPHONE_SIMULATOR\b','IPHONE_SIMULATOR', outtext)
	outtext=re.sub(r'\bTARGET_OS_IPHONE\b','OS_IPHONE', outtext)

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

def replaceinfiles():
	rd=['NatureScene','RadeonTerrainDemo']
	d='/home/tronty/IndieLibX/Samples'
	for i in rd:
		os.path.walk(d+'/'+i+'/tmp', myfun, 13)

def main():
	DXtoIL()
	GLtoIL()

if __name__=="__main__":
	main()

EOF

echo "Back to bash"

