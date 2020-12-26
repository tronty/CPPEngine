/*
 Copyright (C) 2015 Apple Inc. All Rights Reserved.
 See LICENSE.txt for this sample’s licensing information
 
 Abstract:
 Functions for loading a model file for vertex arrays.  The model file
  format used is a simple "binary blob" invented for the purpose of this sample code.
 */

#include "modelUtil.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <fstream>
using namespace std;

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <limits>

#include <assimp/postprocess.h>
#include <assimp/version.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/DefaultLogger.hpp>

#include <assimp/Exporter.hpp>

#include <zlib.h>

#include <Framework3/IRenderer.h>
std::vector<stx_Mesh> mesh_;


typedef struct modelHeaderRec
{
	char fileIdentifier[30];
	unsigned int majorVersion;
	unsigned int minorVersion;
} modelHeader;

typedef struct modelTOCRec
{
	unsigned int attribHeaderSize;
	unsigned int byteElementOffset;
	unsigned int bytePositionOffset;
	unsigned int byteTexcoordOffset;
	unsigned int byteNormalOffset;
} modelTOC;

typedef struct modelAttribRec
{
	unsigned int byteSize;
	GLenum datatype;
	GLenum primType; //If index data
	unsigned int sizePerElement;
	unsigned int numElements;
} modelAttrib;

demoModel* mdlLoadModel(const char* filepathname)
{
	if(NULL == filepathname)
	{
		return NULL;
	}
	
	demoModel* model = (demoModel*) calloc(sizeof(demoModel), 1);
	
	if(NULL == model)
	{
		return NULL;
	}
						
	
	size_t sizeRead;
	int error;
	FILE* curFile = fopen(filepathname, "r");
	
	if(!curFile)
	{	
		mdlDestroyModel(model);	
		return NULL;
	}
	
	modelHeader header;
	
	sizeRead = fread(&header, 1, sizeof(modelHeader), curFile);
	
	if(sizeRead != sizeof(modelHeader))
	{
		fclose(curFile);
		mdlDestroyModel(model);		
		return NULL;
	}
	
	if(strncmp(header.fileIdentifier, "AppleOpenGLDemoModelWWDC2010", sizeof(header.fileIdentifier)))
	{
		fclose(curFile);
		mdlDestroyModel(model);		
		return NULL;
	}
	
	if(header.majorVersion != 0 && header.minorVersion != 1)
	{
		fclose(curFile);
		mdlDestroyModel(model);		
		return NULL;
	}
	
	modelTOC toc;
	
	sizeRead = fread(&toc, 1, sizeof(modelTOC), curFile);
	
	if(sizeRead != sizeof(modelTOC))
	{
		fclose(curFile);
		mdlDestroyModel(model);		
		return NULL;
	}
	
	if(toc.attribHeaderSize > sizeof(modelAttrib))
	{
		fclose(curFile);
		mdlDestroyModel(model);		
		return NULL;
	}
	
	modelAttrib attrib;
	
	error = fseek(curFile, toc.byteElementOffset, SEEK_SET);
	
	if(error < 0)
	{
		fclose(curFile);
		mdlDestroyModel(model);		
		return NULL;
	}
	
	sizeRead = fread(&attrib, 1, toc.attribHeaderSize, curFile);
	
	if(sizeRead != toc.attribHeaderSize)
	{
		fclose(curFile);
		mdlDestroyModel(model);		
		return NULL;
	}
	
	model->elementArraySize = attrib.byteSize;
	model->elementType = attrib.datatype;
	model->numElements = attrib.numElements;

	LOG_PRINT("model->elementArraySize=%d\n", model->elementArraySize);
	LOG_PRINT("model->elementType=%d\n", model->elementType);
	LOG_PRINT("model->numElements=%d\n", model->numElements);
	
	// OpenGL ES cannot use UNSIGNED_INT elements
	// So if the model has UI element...
	if(GL_UNSIGNED_INT == model->elementType)
	{
		LOG_PRINT("GL_UNSIGNED_INT == model->elementType\n");
		//...Load the UI elements and convert to UNSIGNED_SHORT
		
		GLubyte* uiElements = (GLubyte*) malloc(model->elementArraySize);
		size_t ushortElementArraySize = model->numElements * sizeof(GLushort);
		model->elements = (GLubyte*)malloc(ushortElementArraySize); 
		
		sizeRead = fread(uiElements, 1, model->elementArraySize, curFile);
		
		if(sizeRead != model->elementArraySize)
		{
			fclose(curFile);
			mdlDestroyModel(model);		
			return NULL;
		}
		
		GLuint elemNum = 0;
		for(elemNum = 0; elemNum < model->numElements; elemNum++)
		{
			//We can't handle this model if an element is out of the UNSIGNED_INT range
			if(((GLuint*)uiElements)[elemNum] >= 0xFFFF)
			{
				fclose(curFile);
				mdlDestroyModel(model);		
				return NULL;
			}
			
			((GLushort*)model->elements)[elemNum] = ((GLuint*)uiElements)[elemNum];
		}
		
		free(uiElements);
	
		
		model->elementType = GL_UNSIGNED_SHORT;
		model->elementArraySize = model->numElements * sizeof(GLushort);
	}
	else 
	{	
		LOG_PRINT("GL_UNSIGNED_INT != model->elementType\n");
		model->elements = (GLubyte*)malloc(model->elementArraySize);
		
		sizeRead = fread(model->elements, 1, model->elementArraySize, curFile);
		
		if(sizeRead != model->elementArraySize)
		{
			fclose(curFile);
			mdlDestroyModel(model);		
			return NULL;
		}
	}
	LOG_PRINT("model->numElements=%d\n", model->numElements);
	LOG_PRINT("model->elementArraySize=%d\n", model->elementArraySize);

	fseek(curFile, toc.bytePositionOffset, SEEK_SET);
	
	sizeRead = fread(&attrib, 1, toc.attribHeaderSize, curFile);
	
	if(sizeRead != toc.attribHeaderSize)
	{
		fclose(curFile);
		mdlDestroyModel(model);		
		return NULL;
	}
	
	model->positionArraySize = attrib.byteSize;
	model->positionType = attrib.datatype;
	model->positionSize = attrib.sizePerElement;
	model->numVertcies = attrib.numElements;
	model->positions = (GLubyte*) malloc(model->positionArraySize);

	LOG_PRINT("model->positionArraySize=%d\n", model->positionArraySize);
	LOG_PRINT("model->positionType=%d\n", model->positionType);
	LOG_PRINT("model->positionSize=%d\n", model->positionSize);
	LOG_PRINT("model->numVertcies=%d\n", model->numVertcies);
	
	sizeRead = fread(model->positions, 1, model->positionArraySize, curFile);
	
	if(sizeRead != model->positionArraySize)
	{
		fclose(curFile);
		mdlDestroyModel(model);		
		return NULL;
	}
	
	error = fseek(curFile, toc.byteTexcoordOffset, SEEK_SET);
	
	if(error < 0)
	{
		fclose(curFile);
		mdlDestroyModel(model);		
		return NULL;
	}
	
	sizeRead = fread(&attrib, 1, toc.attribHeaderSize, curFile);
	
	if(sizeRead != toc.attribHeaderSize)
	{	
		fclose(curFile);
		mdlDestroyModel(model);		
		return NULL;
	}
	
	model->texcoordArraySize = attrib.byteSize;
	model->texcoordType = attrib.datatype;
	model->texcoordSize = attrib.sizePerElement;

	LOG_PRINT("model->texcoordArraySize=%d\n", model->texcoordArraySize);
	LOG_PRINT("model->texcoordType=%d\n", model->texcoordType);
	LOG_PRINT("model->texcoordSize=%d\n", model->texcoordSize);
	
	//Must have the same number of texcoords as positions
	if(model->numVertcies != attrib.numElements)
	{
		fclose(curFile);
		mdlDestroyModel(model);		
		return NULL;
	}
	
	model->texcoords = (GLubyte*) malloc(model->texcoordArraySize);
	
	sizeRead = fread(model->texcoords, 1, model->texcoordArraySize, curFile);
	
	if(sizeRead != model->texcoordArraySize)
	{
		fclose(curFile);
		mdlDestroyModel(model);		
		return NULL;
	}
	
	error = fseek(curFile, toc.byteNormalOffset, SEEK_SET);
	
	if(error < 0)
	{
		fclose(curFile);
		mdlDestroyModel(model);		
		return NULL;
	}
	
	sizeRead = fread(&attrib, 1, toc.attribHeaderSize, curFile);
	
	if(sizeRead !=  toc.attribHeaderSize)
	{
		fclose(curFile);
		mdlDestroyModel(model);		
		return NULL;
	}
	
	model->normalArraySize = attrib.byteSize;
	model->normalType = attrib.datatype;
	model->normalSize = attrib.sizePerElement;

	LOG_PRINT("model->normalArraySize=%d\n", model->normalArraySize);
	LOG_PRINT("model->normalType=%d\n", model->normalType);
	LOG_PRINT("model->normalSize=%d\n", model->normalSize);

	//Must have the same number of normals as positions
	if(model->numVertcies != attrib.numElements)
	{
		fclose(curFile);
		mdlDestroyModel(model);		
		return NULL;
	}
		
	model->normals = (GLubyte*) malloc(model->normalArraySize );
	
	sizeRead =  fread(model->normals, 1, model->normalArraySize , curFile);
	
	if(sizeRead != model->normalArraySize)
	{
		fclose(curFile);
		mdlDestroyModel(model);		
		return NULL;
	}
	
	
	fclose(curFile);
	
	return model;
	
}

demoModel* mdlLoadQuadModel()
{
	GLfloat posArray[] = {
		-200.0f, 0.0f, -200.0f,
		 200.0f, 0.0f, -200.0f,
		 200.0f, 0.0f,  200.0f,
		-200.0f, 0.0f,  200.0f
	};
		
	GLfloat texcoordArray[] = { 
		0.0f,  1.0f,
		1.0f,  1.0f,
		1.0f,  0.0f,
		0.0f,  0.0f
	};
	
	GLfloat normalArray[] = {
		0.0f, 0.0f, 1.0,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
	};
	
	GLushort elementArray[] =
	{
		0, 2, 1,
		0, 3, 2
	};
	
	demoModel* model = (demoModel*) calloc(sizeof(demoModel), 1);
	
	if(NULL == model)
	{
		return NULL;
	}
	
	model->positionType = GL_FLOAT;
	model->positionSize = 3;
	model->positionArraySize = sizeof(posArray);
	model->positions = (GLubyte*)malloc(model->positionArraySize);
	stx_memcpy(model->positions, posArray, model->positionArraySize);
	
	model->texcoordType = GL_FLOAT;
	model->texcoordSize = 2;
	model->texcoordArraySize = sizeof(texcoordArray);
	model->texcoords = (GLubyte*)malloc(model->texcoordArraySize);
	stx_memcpy(model->texcoords, texcoordArray, model->texcoordArraySize );

	model->normalType = GL_FLOAT;
	model->normalSize = 3;
	model->normalArraySize = sizeof(normalArray);
	model->normals = (GLubyte*)malloc(model->normalArraySize);
	stx_memcpy(model->normals, normalArray, model->normalArraySize);
	
	model->elementArraySize = sizeof(elementArray);
	model->elements	= (GLubyte*)malloc(model->elementArraySize);
	stx_memcpy(model->elements, elementArray, model->elementArraySize);
	
	model->primType = GL_TRIANGLES;
	
	
	model->numElements = sizeof(elementArray) / sizeof(GLushort);
	model->elementType = GL_UNSIGNED_SHORT;
	model->numVertcies = model->positionArraySize / (model->positionSize * sizeof(GLfloat));
	
	return model;
}

void mdlDestroyModel(demoModel* model)
{
	if(NULL == model)
	{
		return;
	}
	
	free(model->elements);
	free(model->positions);
	free(model->normals);
	free(model->texcoords);
	
	free(model);
}

int ApplicationLogic()
{
	std::string fn=stx_convertpath("/GLEssentials/Assets/demon.mdl");
	demoModel* model=mdlLoadModel(fn.c_str());

	std::vector<D3DXFROMWINEVECTOR3> vertices3;
	std::vector<D3DXFROMWINEVECTOR3> normals3;
	std::vector<D3DXFROMWINEVECTOR2> texcoords2;
	std::vector<stx_Texture> Textures;

	float* positions=(float*)model->positions;
	float* normals=(float*)model->normals;
	float* texcoords=(float*)model->texcoords;
	//((GLushort*)model->elements)[elemNum] = ((GLuint*)uiElements)[elemNum];
	unsigned short* elements=(unsigned short*)model->elements;

/*
stx_convertpath:174
../../IndieLib_resources/GLEssentials/Assets/demon.mdl:
model->positionArraySize=140320:8770*16
model->positionSize=4
model->numVertcies=8770
model->texcoordArraySize=70160:8770*8
model->texcoordSize=2
model->normalArraySize=105240:8770*12
model->normalSize=3
../../../Libraries/STX/src/fileUtilCPP.cpp:
stx_convertpath:174
../../IndieLib_resources/GLEssentials/Assets/demon.x:
*/
	for(unsigned int i=0;i<model->numVertcies;i++)
	{
		stx_VertexPositionTexture v_;
#if 1
		vertices3.push_back(D3DXFROMWINEVECTOR3(positions[4*i+0], positions[4*i+2], positions[4*i+1]));
		normals3.push_back(D3DXFROMWINEVECTOR3(normals[3*i+0], normals[3*i+2], normals[3*i+1]));
		texcoords2.push_back(D3DXFROMWINEVECTOR2(texcoords[2*i+0], texcoords[2*i+1]));
#else
		vertices3.push_back(D3DXFROMWINEVECTOR3(positions[4*i+0], positions[4*i+1], positions[4*i+2]));
		normals3.push_back(D3DXFROMWINEVECTOR3(normals[3*i+0], normals[3*i+1], normals[3*i+2]));
		texcoords2.push_back(D3DXFROMWINEVECTOR2(texcoords[2*i+0], texcoords[2*i+1]));
#endif
		v_.Position=vertices3[vertices3.size()-1];
		v_.Normal=normals3[vertices3.size()-1];
		v_.Tex=texcoords2[vertices3.size()-1];
		mesh_.vertices.push_back(v_);
		mesh_.indices.push_back(vertices3.size()-1);
	}
	mesh_.textureNames.push_back("demon.png");
	std::vector<__WORD__> inds1;
	std::vector<__WORD__> inds2;

	__WORD__ elemNum = 0;
#if 0
	for(elemNum = 0; elemNum < model->numElements/4; elemNum++)
	{
		inds1.push_back(((GLuint*)elements)[4*elemNum]);
		inds1.push_back(((GLuint*)elements)[4*elemNum+2]);
		inds1.push_back(((GLuint*)elements)[4*elemNum+1]);
		inds2.push_back(0);
		inds1.push_back(((GLuint*)elements)[4*elemNum]);
		inds1.push_back(((GLuint*)elements)[4*elemNum+3]);
		inds1.push_back(((GLuint*)elements)[4*elemNum+2]);
		inds2.push_back(0);
	}
#elif 1
	for(elemNum = 0; elemNum < model->numElements; elemNum++)
	{
		inds1.push_back(elements[elemNum]);
		if((elemNum % 3) ==0)
			inds2.push_back(0);
	}
#elif 0
	for(unsigned int i=0;i<model->numVertcies;i++)
		inds1.push_back(i);
	for(unsigned int i=0;i<model->numVertcies/3;i++)
		inds2.push_back(0);
#endif

	LOG_PRINT("vertices3.size()=%d\n", vertices3.size());
	LOG_PRINT("normals3.size()=%d\n", normals3.size());
	LOG_PRINT("texcoords2.size()=%d\n", texcoords2.size());
	LOG_PRINT("inds1.size()=%d\n", inds1.size());
	LOG_PRINT("inds2.size()=%d\n", inds2.size());

	stx_Texture texture(vt, inds2);
	stx_writeXFile("/GLEssentials/Assets/demon.x", mesh_);//, vertices3, normals3, texcoords2, inds1, texture);
	return 0;
}

