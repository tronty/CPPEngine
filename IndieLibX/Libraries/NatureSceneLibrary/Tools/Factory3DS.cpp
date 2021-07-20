#include <Framework3/IRenderer.h>
#include "Factory3DS.h"

#include <Managers/GeometryManager.h>

#include <Framework3/IRenderer.h>

#include "Index.h"



#include <stdio.h>

#ifndef __APPLE__
#include <malloc.h>
#endif

#include <memory.h>


//using namespace NatureScene;
#ifndef FREE

#define FREE(p) { if(p) { free(p); p=0; } }

#endif

#define fastSquareRoot sqrt

typedef void (*cb)(Chunk3DS_t *, void *);



static FILE *File3DS=0;

static char szString[64];



int modelID   = 0;

int sizeOfT3F = sizeof(D3DXFROMWINEVECTOR3);



int comparePosition(float *a, float *b)

{

	return memcmp(a, b, sizeOfT3F);

}



void computeNormals(Mesh3D *mesh)

{

  IndexT vertexIndex;



  float *faceNormals = 0,

	*normalVecs  = 0,

	*texCoords   = 0,

	*vertices    = 0,

	 magnitude   = 0,

	 normal[3],

	 v0[3],

	 v1[3];



  int (*TriangleRefs)[32];



  int  i, j, k,

       count  = 0,

       vertex = 0,

       result = 0;



  unsigned short *pFaces  = 0;



  mesh->normalVecs  = new D3DXFROMWINEVECTOR3[mesh->NumVertex];

  normalVecs	    = (float*)mesh->normalVecs;

  vertices	    = (float*)mesh->vertices;

  pFaces	    = (unsigned short*)mesh->Face;



  if(!mesh->smoothGroup)

  {

    for(i=0;i<mesh->NumFace;i++)

    {



      v0[0] = vertices[3*pFaces[3*i+1]+0] - vertices[3*pFaces[3*i+0]+0];

      v0[1] = vertices[3*pFaces[3*i+1]+1] - vertices[3*pFaces[3*i+0]+1];

      v0[2] = vertices[3*pFaces[3*i+1]+2] - vertices[3*pFaces[3*i+0]+2];



      v1[0] = vertices[3*pFaces[3*i+2]+0] - vertices[3*pFaces[3*i+0]+0];

      v1[1] = vertices[3*pFaces[3*i+2]+1] - vertices[3*pFaces[3*i+0]+1];

      v1[2] = vertices[3*pFaces[3*i+2]+2] - vertices[3*pFaces[3*i+0]+2];



      normal[0] = (v0[1]*v1[2]-v0[2]*v1[1]);

      normal[1] = (v0[2]*v1[0]-v0[0]*v1[2]);

      normal[2] = (v0[0]*v1[1]-v0[1]*v1[0]);



      magnitude = fastSquareRoot(normal[0]*normal[0] +

				 normal[1]*normal[1] +

				 normal[2]*normal[2]);



      if(magnitude > .00001f){

	magnitude  = 1.0f/magnitude;

	normal[0] *= magnitude;

	normal[1] *= magnitude;

	normal[2] *= magnitude;

      }



      for(j=0;j<3;j++){

	normalVecs[3*pFaces[3*i+j]+0] += normal[0];

	normalVecs[3*pFaces[3*i+j]+1] += normal[1];

	normalVecs[3*pFaces[3*i+j]+2] += normal[2];

      }

    }

    return;

  }



  indexArray(&vertexIndex, (char *)vertices, sizeof(float)*3, mesh->NumVertex, (sortFunction)comparePosition);



  TriangleRefs = (int (*)[32])malloc(sizeof(int)*32*vertexIndex.count);



  if(TriangleRefs==0)

    return;



  stx_memset(TriangleRefs, 0, sizeof(int)*32*vertexIndex.count);



  for(i=0;i<mesh->NumFace;i++)

  {

    for(j=0;j<3;j++){

     vertex = indexFind(&vertexIndex, &vertices[3*pFaces[3*i+j]], &result);



      if(TriangleRefs[vertex][0]<48){

	TriangleRefs[vertex][0]++;

	TriangleRefs[vertex][TriangleRefs[vertex][0]]=i;

      }

    }

  }



  faceNormals = new float[3*mesh->NumFace];



  for(i=0;i<mesh->NumFace;i++)

  {

    v0[0] = vertices[3*pFaces[3*i+1]+0] - vertices[3*pFaces[3*i+0]+0];

    v0[1] = vertices[3*pFaces[3*i+1]+1] - vertices[3*pFaces[3*i+0]+1];

    v0[2] = vertices[3*pFaces[3*i+1]+2] - vertices[3*pFaces[3*i+0]+2];



    v1[0] = vertices[3*pFaces[3*i+2]+0] - vertices[3*pFaces[3*i+0]+0];

    v1[1] = vertices[3*pFaces[3*i+2]+1] - vertices[3*pFaces[3*i+0]+1];

    v1[2] = vertices[3*pFaces[3*i+2]+2] - vertices[3*pFaces[3*i+0]+2];



    faceNormals[3*i+0] = (v0[1]*v1[2]-v0[2]*v1[1]);

    faceNormals[3*i+1] = (v0[2]*v1[0]-v0[0]*v1[2]);

    faceNormals[3*i+2] = (v0[0]*v1[1]-v0[1]*v1[0]);



    magnitude = fastSquareRoot(faceNormals[3*i+0]*faceNormals[3*i+0] +

			       faceNormals[3*i+1]*faceNormals[3*i+1] +

			       faceNormals[3*i+2]*faceNormals[3*i+2]);



    if(magnitude)

    {

      magnitude 	  = 1.0f/magnitude;

      faceNormals[3*i+0] *= magnitude;

      faceNormals[3*i+1] *= magnitude;

      faceNormals[3*i+2] *= magnitude;

    }

  }



  for(i=0;i<mesh->NumFace;i++)

  {

    for(j=0;j<3;j++){



      vertex = indexFind(&vertexIndex, (void *)&vertices[3*pFaces[3*i+j]], &result);

      count  = 0;



      for(k=1; k <= TriangleRefs[vertex][0];k++)

      {

	if(mesh->smoothGroup[i] == mesh->smoothGroup[TriangleRefs[vertex][k]])

	{

	  normalVecs[3*pFaces[3*i+j]+0] += faceNormals[3*TriangleRefs[vertex][k]+0];

	  normalVecs[3*pFaces[3*i+j]+1] += faceNormals[3*TriangleRefs[vertex][k]+1];

	  normalVecs[3*pFaces[3*i+j]+2] += faceNormals[3*TriangleRefs[vertex][k]+2];

	  count++;

	}

      }



      normalVecs[3*pFaces[3*i+j]+0] /= (float)count;

      normalVecs[3*pFaces[3*i+j]+1] /= (float)count;

      normalVecs[3*pFaces[3*i+j]+2] /= (float)count;

    }

  }

  indexFree(&vertexIndex);



  FREE(TriangleRefs);

  deleteArray(faceNormals);

  deleteArray(mesh->smoothGroup);

}



void computeTangentSpace(Mesh3D *Mesh)

{

  if(!Mesh->texCoords){

    computeNormals(Mesh);

    return;

  }



	int i, j, k, Count, Vertex=0, Result=0;

	IndexT	VertexIndex;

	int (*TriangleRefs)[32];

	float *FaceTangent;

	float *FaceBinormal;

	float *FaceNormal;

	float v0[3], v1[3], cross[3];

  float *pTexCoords = 0,

	*pBiNormals = 0,

	*pVertices  = 0,

	*pTangents  = 0,

	*pNormals   = 0,

	Mag;



	D3DXFROMWINEVECTOR3 Tangent,

	  Binormal,

	  Normal;



  unsigned short   *pFaces = 0;



  Mesh->binormalVecs = new D3DXFROMWINEVECTOR3[Mesh->NumVertex];

  Mesh->tangentVecs  = new D3DXFROMWINEVECTOR3[Mesh->NumVertex];

  Mesh->normalVecs   = new D3DXFROMWINEVECTOR3[Mesh->NumVertex];

  pBiNormals	     = (float*)Mesh->binormalVecs ;

  pTexCoords	     = (float*)Mesh->texCoords;

  pTangents	     = (float*)Mesh->tangentVecs;

  pVertices	     = (float*)Mesh->vertices;

  pNormals	     = (float*)Mesh->normalVecs;

  pFaces	     = (unsigned short *)Mesh->Face;



  if(Mesh->smoothGroup==0)

	{

		for(i=0;i<Mesh->NumFace;i++)

		{

			v0[0]=pVertices[3*pFaces[3*i+1]+0]-pVertices[3*pFaces[3*i+0]+0];

			v0[1]=pTexCoords[2*pFaces[3*i+1]+0]-pTexCoords[2*pFaces[3*i+0]+0];

			v0[2]=pTexCoords[2*pFaces[3*i+1]+1]-pTexCoords[2*pFaces[3*i+0]+1];



			v1[0]=pVertices[3*pFaces[3*i+2]+0]-pVertices[3*pFaces[3*i+0]+0];

			v1[1]=pTexCoords[2*pFaces[3*i+2]+0]-pTexCoords[2*pFaces[3*i+0]+0];

			v1[2]=pTexCoords[2*pFaces[3*i+2]+1]-pTexCoords[2*pFaces[3*i+0]+1];



			cross[0]=(v0[1]*v1[2]-v0[2]*v1[1]);

			cross[1]=(v0[2]*v1[0]-v0[0]*v1[2]);

			cross[2]=(v0[0]*v1[1]-v0[1]*v1[0]);



			if(fabs(cross[0])>0.000001f)

			{

				Tangent[0]  =-cross[1]/cross[0];

				Binormal[0] =cross[2]/cross[0];

			}



			v0[0]=pVertices[3*pFaces[3*i+1]+1]-pVertices[3*pFaces[3*i+0]+1];

			v0[1]=pTexCoords[2*pFaces[3*i+1]+0]-pTexCoords[2*pFaces[3*i+0]+0];

			v0[2]=pTexCoords[2*pFaces[3*i+1]+1]-pTexCoords[2*pFaces[3*i+0]+1];



			v1[0]=pVertices[3*pFaces[3*i+2]+1]-pVertices[3*pFaces[3*i+0]+1];

			v1[1]=pTexCoords[2*pFaces[3*i+2]+0]-pTexCoords[2*pFaces[3*i+0]+0];

			v1[2]=pTexCoords[2*pFaces[3*i+2]+1]-pTexCoords[2*pFaces[3*i+0]+1];



			cross[0]=(v0[1]*v1[2]-v0[2]*v1[1]);

			cross[1]=(v0[2]*v1[0]-v0[0]*v1[2]);

			cross[2]=(v0[0]*v1[1]-v0[1]*v1[0]);



			if(fabs(cross[0])>0.000001f)

			{

				Tangent[1]=-cross[1]/cross[0];

				Binormal[1]=cross[2]/cross[0];

			}



			v0[0]=pVertices[3*pFaces[3*i+1]+2]-pVertices[3*pFaces[3*i+0]+2];

			v0[1]=pTexCoords[2*pFaces[3*i+1]+0]-pTexCoords[2*pFaces[3*i+0]+0];

			v0[2]=pTexCoords[2*pFaces[3*i+1]+1]-pTexCoords[2*pFaces[3*i+0]+1];



			v1[0]=pVertices[3*pFaces[3*i+2]+2]-pVertices[3*pFaces[3*i+0]+2];

			v1[1]=pTexCoords[2*pFaces[3*i+2]+0]-pTexCoords[2*pFaces[3*i+0]+0];

			v1[2]=pTexCoords[2*pFaces[3*i+2]+1]-pTexCoords[2*pFaces[3*i+0]+1];



			cross[0]=(v0[1]*v1[2]-v0[2]*v1[1]);

			cross[1]=(v0[2]*v1[0]-v0[0]*v1[2]);

			cross[2]=(v0[0]*v1[1]-v0[1]*v1[0]);



			if(fabs(cross[0])>0.000001f)

			{

				Tangent[2]=-cross[1]/cross[0];

				Binormal[2]=cross[2]/cross[0];

			}



			Mag = fastSquareRoot(Tangent[0]*Tangent[0]+Tangent[1]*Tangent[1]+Tangent[2]*Tangent[2]);



			if(Mag)

			{

				Mag=1.0f/Mag;

				Tangent[0]*=Mag;

				Tangent[1]*=Mag;

				Tangent[2]*=Mag;

			}



			Mag= fastSquareRoot(Binormal[0]*Binormal[0]+Binormal[1]*Binormal[1]+Binormal[2]*Binormal[2]);



			if(Mag)

			{

				Mag=1.0f/Mag;

				Binormal[0]*=Mag;

				Binormal[1]*=Mag;

				Binormal[2]*=Mag;

			}



			v0[0]=pVertices[3*pFaces[3*i+1]+0]-pVertices[3*pFaces[3*i+0]+0];

			v0[1]=pVertices[3*pFaces[3*i+1]+1]-pVertices[3*pFaces[3*i+0]+1];

			v0[2]=pVertices[3*pFaces[3*i+1]+2]-pVertices[3*pFaces[3*i+0]+2];



			v1[0]=pVertices[3*pFaces[3*i+2]+0]-pVertices[3*pFaces[3*i+0]+0];

			v1[1]=pVertices[3*pFaces[3*i+2]+1]-pVertices[3*pFaces[3*i+0]+1];

			v1[2]=pVertices[3*pFaces[3*i+2]+2]-pVertices[3*pFaces[3*i+0]+2];



			Normal[0]=(v0[1]*v1[2]-v0[2]*v1[1]);

			Normal[1]=(v0[2]*v1[0]-v0[0]*v1[2]);

			Normal[2]=(v0[0]*v1[1]-v0[1]*v1[0]);



			Mag = fastSquareRoot(Normal[0]*Normal[0]+Normal[1]*Normal[1]+Normal[2]*Normal[2]);



			if(Mag)

			{

				Mag=1.0f/Mag;

				Normal[0]*=Mag;

				Normal[1]*=Mag;

				Normal[2]*=Mag;

			}



			for(j=0;j<3;j++)

			{

				pTangents[3*pFaces[3*i+j]+0]+=Tangent[0];

				pTangents[3*pFaces[3*i+j]+1]+=Tangent[1];

				pTangents[3*pFaces[3*i+j]+2]+=Tangent[2];



				pBiNormals[3*pFaces[3*i+j]+0]+=Binormal[0];

				pBiNormals[3*pFaces[3*i+j]+1]+=Binormal[1];

				pBiNormals[3*pFaces[3*i+j]+2]+=Binormal[2];



				pNormals[3*pFaces[3*i+j]+0]+=Normal[0];

				pNormals[3*pFaces[3*i+j]+1]+=Normal[1];

				pNormals[3*pFaces[3*i+j]+2]+=Normal[2];

			}

		}



		return;

	}

	indexArray(&VertexIndex, (char *)pVertices, sizeof(float)*3, Mesh->NumVertex, (sortFunction)comparePosition);



	TriangleRefs=(int (*)[32])malloc(sizeof(int)*32*VertexIndex.count);



	if(TriangleRefs==0)

		return;



	stx_memset(TriangleRefs, 0, sizeof(int)*32*VertexIndex.count);



	for(i=0;i<Mesh->NumFace;i++)

	{

		for(j=0;j<3;j++)

		{

			Vertex=indexFind(&VertexIndex, &pVertices[3*pFaces[3*i+j]], &Result);



			if(TriangleRefs[Vertex][0]<48)

			{

				TriangleRefs[Vertex][0]++;

				TriangleRefs[Vertex][TriangleRefs[Vertex][0]]=i;

			}

		}

	}



	FaceTangent=(float *)malloc(sizeof(float)*3*Mesh->NumFace);



	if(FaceTangent==0)

		return;



	stx_memset(FaceTangent, 0, sizeof(float)*3*Mesh->NumFace);



	FaceBinormal=(float *)malloc(sizeof(float)*3*Mesh->NumFace);



	if(FaceBinormal==0)

		return;



	stx_memset(FaceBinormal, 0, sizeof(float)*3*Mesh->NumFace);



	FaceNormal=(float *)malloc(sizeof(float)*3*Mesh->NumFace);



	if(FaceNormal==0)

		return;



	stx_memset(FaceNormal, 0, sizeof(float)*3*Mesh->NumFace);



	for(i=0;i<Mesh->NumFace;i++)

	{

		v0[0]=pVertices[3*pFaces[3*i+1]+0]-pVertices[3*pFaces[3*i+0]+0];

		v0[1]=pTexCoords[2*pFaces[3*i+1]+0]-pTexCoords[2*pFaces[3*i+0]+0];

		v0[2]=pTexCoords[2*pFaces[3*i+1]+1]-pTexCoords[2*pFaces[3*i+0]+1];



		v1[0]=pVertices[3*pFaces[3*i+2]+0]-pVertices[3*pFaces[3*i+0]+0];

		v1[1]=pTexCoords[2*pFaces[3*i+2]+0]-pTexCoords[2*pFaces[3*i+0]+0];

		v1[2]=pTexCoords[2*pFaces[3*i+2]+1]-pTexCoords[2*pFaces[3*i+0]+1];



		cross[0]=(v0[1]*v1[2]-v0[2]*v1[1]);

		cross[1]=(v0[2]*v1[0]-v0[0]*v1[2]);

		cross[2]=(v0[0]*v1[1]-v0[1]*v1[0]);



		if(fabs(cross[0])>0.000001f)

		{

			FaceTangent[3*i+0]=-cross[1]/cross[0];

			FaceBinormal[3*i+0]=cross[2]/cross[0];

		}



		v0[0]=pVertices[3*pFaces[3*i+1]+1]-pVertices[3*pFaces[3*i+0]+1];

		v0[1]=pTexCoords[2*pFaces[3*i+1]+0]-pTexCoords[2*pFaces[3*i+0]+0];

		v0[2]=pTexCoords[2*pFaces[3*i+1]+1]-pTexCoords[2*pFaces[3*i+0]+1];



		v1[0]=pVertices[3*pFaces[3*i+2]+1]-pVertices[3*pFaces[3*i+0]+1];

		v1[1]=pTexCoords[2*pFaces[3*i+2]+0]-pTexCoords[2*pFaces[3*i+0]+0];

		v1[2]=pTexCoords[2*pFaces[3*i+2]+1]-pTexCoords[2*pFaces[3*i+0]+1];



		cross[0]=(v0[1]*v1[2]-v0[2]*v1[1]);

		cross[1]=(v0[2]*v1[0]-v0[0]*v1[2]);

		cross[2]=(v0[0]*v1[1]-v0[1]*v1[0]);



		if(fabs(cross[0])>0.000001f)

		{

			FaceTangent[3*i+1]=-cross[1]/cross[0];

			FaceBinormal[3*i+1]=cross[2]/cross[0];

		}



		v0[0]=pVertices[3*pFaces[3*i+1]+2]-pVertices[3*pFaces[3*i+0]+2];

		v0[1]=pTexCoords[2*pFaces[3*i+1]+0]-pTexCoords[2*pFaces[3*i+0]+0];

		v0[2]=pTexCoords[2*pFaces[3*i+1]+1]-pTexCoords[2*pFaces[3*i+0]+1];



		v1[0]=pVertices[3*pFaces[3*i+2]+2]-pVertices[3*pFaces[3*i+0]+2];

		v1[1]=pTexCoords[2*pFaces[3*i+2]+0]-pTexCoords[2*pFaces[3*i+0]+0];

		v1[2]=pTexCoords[2*pFaces[3*i+2]+1]-pTexCoords[2*pFaces[3*i+0]+1];



		cross[0]=(v0[1]*v1[2]-v0[2]*v1[1]);

		cross[1]=(v0[2]*v1[0]-v0[0]*v1[2]);

		cross[2]=(v0[0]*v1[1]-v0[1]*v1[0]);



		if(fabs(cross[0])>0.000001f)

		{

			FaceTangent[3*i+2]=-cross[1]/cross[0];

			FaceBinormal[3*i+2]=cross[2]/cross[0];

		}



		Mag=(float)sqrt(FaceTangent[3*i+0]*FaceTangent[3*i+0]+FaceTangent[3*i+1]*FaceTangent[3*i+1]+FaceTangent[3*i+2]*FaceTangent[3*i+2]);



		if(Mag)

		{

			Mag=1.0f/Mag;

			FaceTangent[3*i+0]*=Mag;

			FaceTangent[3*i+1]*=Mag;

			FaceTangent[3*i+2]*=Mag;

		}



		Mag=(float)sqrt(FaceBinormal[3*i+0]*FaceBinormal[3*i+0]+FaceBinormal[3*i+1]*FaceBinormal[3*i+1]+FaceBinormal[3*i+2]*FaceBinormal[3*i+2]);



		if(Mag)

		{

			Mag=1.0f/Mag;

			FaceBinormal[3*i+0]*=Mag;

			FaceBinormal[3*i+1]*=Mag;

			FaceBinormal[3*i+2]*=Mag;

		}



		v0[0]=pVertices[3*pFaces[3*i+1]+0]-pVertices[3*pFaces[3*i+0]+0];

		v0[1]=pVertices[3*pFaces[3*i+1]+1]-pVertices[3*pFaces[3*i+0]+1];

		v0[2]=pVertices[3*pFaces[3*i+1]+2]-pVertices[3*pFaces[3*i+0]+2];



		v1[0]=pVertices[3*pFaces[3*i+2]+0]-pVertices[3*pFaces[3*i+0]+0];

		v1[1]=pVertices[3*pFaces[3*i+2]+1]-pVertices[3*pFaces[3*i+0]+1];

		v1[2]=pVertices[3*pFaces[3*i+2]+2]-pVertices[3*pFaces[3*i+0]+2];



		FaceNormal[3*i+0]=(v0[1]*v1[2]-v0[2]*v1[1]);

		FaceNormal[3*i+1]=(v0[2]*v1[0]-v0[0]*v1[2]);

		FaceNormal[3*i+2]=(v0[0]*v1[1]-v0[1]*v1[0]);



		Mag=(float)sqrt(FaceNormal[3*i+0]*FaceNormal[3*i+0]+FaceNormal[3*i+1]*FaceNormal[3*i+1]+FaceNormal[3*i+2]*FaceNormal[3*i+2]);



		if(Mag)

		{

			Mag=1.0f/Mag;

			FaceNormal[3*i+0]*=Mag;

			FaceNormal[3*i+1]*=Mag;

			FaceNormal[3*i+2]*=Mag;

		}

	}



	for(i=0;i<Mesh->NumFace;i++)

	{

		for(j=0;j<3;j++)

		{

			Vertex=indexFind(&VertexIndex, (void *)&pVertices[3*pFaces[3*i+j]], &Result);



			Count=0;



			for(k=1;k<=TriangleRefs[Vertex][0];k++)

			{

				if(Mesh->smoothGroup[i]==Mesh->smoothGroup[TriangleRefs[Vertex][k]])

				{

					pTangents[3*pFaces[3*i+j]+0]+=FaceTangent[3*TriangleRefs[Vertex][k]+0];

					pTangents[3*pFaces[3*i+j]+1]+=FaceTangent[3*TriangleRefs[Vertex][k]+1];

					pTangents[3*pFaces[3*i+j]+2]+=FaceTangent[3*TriangleRefs[Vertex][k]+2];



					pBiNormals[3*pFaces[3*i+j]+0]+=FaceBinormal[3*TriangleRefs[Vertex][k]+0];

					pBiNormals[3*pFaces[3*i+j]+1]+=FaceBinormal[3*TriangleRefs[Vertex][k]+1];

					pBiNormals[3*pFaces[3*i+j]+2]+=FaceBinormal[3*TriangleRefs[Vertex][k]+2];



					pNormals[3*pFaces[3*i+j]+0]+=FaceNormal[3*TriangleRefs[Vertex][k]+0];

					pNormals[3*pFaces[3*i+j]+1]+=FaceNormal[3*TriangleRefs[Vertex][k]+1];

					pNormals[3*pFaces[3*i+j]+2]+=FaceNormal[3*TriangleRefs[Vertex][k]+2];



					Count++;

				}

			}



			pTangents[3*pFaces[3*i+j]+0]/=(float)Count;

			pTangents[3*pFaces[3*i+j]+1]/=(float)Count;

			pTangents[3*pFaces[3*i+j]+2]/=(float)Count;



			pBiNormals[3*pFaces[3*i+j]+0]/=(float)Count;

			pBiNormals[3*pFaces[3*i+j]+1]/=(float)Count;

			pBiNormals[3*pFaces[3*i+j]+2]/=(float)Count;



			pNormals[3*pFaces[3*i+j]+0]/=(float)Count;

			pNormals[3*pFaces[3*i+j]+1]/=(float)Count;

			pNormals[3*pFaces[3*i+j]+2]/=(float)Count;

		}

	}

	indexFree(&VertexIndex);



	FREE(TriangleRefs);

	FREE(FaceTangent);

	FREE(FaceBinormal);

	FREE(FaceNormal);

}



void ReadString(char *NSString)

{

  int i=0;



  while(1)

  {

    fread(&NSString[i], sizeof(char), 1, File3DS);



    if(NSString[i++]=='\0')

      break;

  }

}



void ReadChunkArray(int Length, void (*Callback)(Chunk3DS_t *, void *), void *Data)

{

  Chunk3DS_t Chunk;

  int Start, Position=0;



  do

  {

    Start=ftell(File3DS);

    fread(&Chunk, sizeof(Chunk3DS_t), 1, File3DS);



    Callback(&Chunk, Data);



    fseek(File3DS, Start+Chunk.Length, STX_SEEK_SET);



    Position+=Chunk.Length;

  }

  while(Position<Length);

}



void InspectChunkArray(int Length, void (*Callback)(Chunk3DS_t *, void *), void *Data)

{

  Chunk3DS_t Chunk;

  int Start, ChunkStart, Position=0;



  ChunkStart=ftell(File3DS);



  do

  {

    Start=ftell(File3DS);



    fread(&Chunk, sizeof(Chunk3DS_t), 1, File3DS);



    Callback(&Chunk, Data);



    fseek(File3DS, Start+Chunk.Length, STX_SEEK_SET);



    Position+=Chunk.Length;

  }

  while(Position<Length);



  fseek(File3DS, ChunkStart, STX_SEEK_SET);

}



void ReadColorChunk(Chunk3DS_t *Chunk, float *Color)

{

  unsigned char RGB[3];



  switch(Chunk->ID)

  {

    case 0x0010:

      fread(Color, sizeof(float), 3, File3DS);

      break;



    case 0x0011:

      fread(RGB, sizeof(unsigned char), 3, File3DS);



      Color[0]=(float)RGB[0]/255.0f;

      Color[1]=(float)RGB[1]/255.0f;

      Color[2]=(float)RGB[2]/255.0f;

      break;

  }

}



void ReadPercentageChunk(Chunk3DS_t *Chunk, float *Value)

{

  short Percent;



  switch(Chunk->ID)

  {

    case 0x0030:

      fread(&Percent, sizeof(short), 1, File3DS);

      *Value=(float)Percent;

      break;



    case 0x0031:

      fread(Value, sizeof(float), 1, File3DS);

      break;

  }



  *Value/=100.0f;

}



void InspectEditObject(Chunk3DS_t *Chunk, Model3DS_t *Model)

{

  switch(Chunk->ID)

  {

    case 0x4100:

      Model->NumMesh++;

      break;

  }

}



void InspectEdit3DS(Chunk3DS_t *Chunk, Model3DS_t *Model)

{

  switch(Chunk->ID)

  {

    case 0xAFFF:

      Model->NumMaterial++;

      break;



    case 0x4000:

      ReadString(szString);

      InspectChunkArray(Chunk->Length-6+stx_strlen(szString), (cb)InspectEditObject, Model);

      break;

  }

}



void ReadVertexList(Mesh3D  *Mesh)

{

  unsigned short i, NumVertex;



  fread(&NumVertex, sizeof(unsigned short), 1, File3DS);



  Mesh->NumVertex = NumVertex;

  Mesh->vertices  = new D3DXFROMWINEVECTOR3[NumVertex];

  float Vertex[3];



  for(i=0;i<NumVertex;i++)

  {

    fread(Vertex, sizeof(float), 3, File3DS);

    Mesh->vertices[i]=D3DXFROMWINEVECTOR3( Vertex[0], Vertex[2], -Vertex[1]);

  }

}



void ReadMaterialGroup(Group3DS *group)

{

  unsigned short NumFace;



  ReadString(group->Name);



  fread(&NumFace, sizeof(unsigned short), 1, File3DS);

  group->Face = (unsigned short *)malloc(NumFace*sizeof(unsigned short));



  if(group->Face)

  {

    group->Size = NumFace;

    group->Material = 0;

    fread(group->Face, sizeof(unsigned short), NumFace, File3DS);

  }

}



void InspectFaceSubs(Chunk3DS_t *Chunk, Mesh3D	*mesh)

{

  switch(Chunk->ID)

  {

    case 0x4130:

      mesh->NumGroup++;

      break;

  }

}



void ReadFaceSubs(Chunk3DS_t *Chunk, Mesh3D  *mesh)

{

  switch(Chunk->ID)

  {

   case 0x4130:

      ReadMaterialGroup(&mesh->group[mesh->NumGroup++]);

      break;



    case 0x4150:

      mesh->smoothGroup = new int[mesh->NumFace];

      if(mesh->smoothGroup)

	fread(mesh->smoothGroup, sizeof(int), mesh->NumFace, File3DS);

      break;

  }

}



void ReadFaceList(Mesh3D *mesh, unsigned int Length)

{

  unsigned int Position=6;

  unsigned short NumFace;

  int i;



  fread(&NumFace, sizeof(unsigned short), 1, File3DS);

  Position+=sizeof(unsigned short);



  mesh->NumFace = NumFace;

  mesh->Face = (unsigned short *)malloc(sizeof(unsigned short)*3*mesh->NumFace);



  for(i=0;i<NumFace;i++)

  {

    unsigned short FaceFlag;



    fread(&mesh->Face[3*i], sizeof(unsigned short), 3, File3DS);

    fread(&FaceFlag, sizeof(unsigned short), 1, File3DS);

  }



  Position+=sizeof(unsigned short)*4*NumFace;



  if(Position<Length)

  {

    InspectChunkArray(Position-6, (cb)InspectFaceSubs, mesh);



    if(mesh->NumGroup)

    {

      mesh->group=(Group3DS *)malloc(sizeof(Group3DS)*mesh->NumGroup);

      stx_memset(mesh->group, 0, sizeof(Group3DS)*mesh->NumGroup);



      if(mesh->group)

      {

	mesh->NumGroup=0;

	ReadChunkArray(Position-6, (cb)ReadFaceSubs, mesh);

      }

    }

  }

}



void ReadTextureCoordinates(Mesh3D *mesh)

{

  unsigned short NumUV;



  fread(&NumUV, sizeof(unsigned short), 1, File3DS);

  mesh->NumUV = NumUV;



  mesh->texCoords = new D3DXFROMWINEVECTOR2[NumUV];

  fread(mesh->texCoords, sizeof(D3DXFROMWINEVECTOR2),  mesh->NumUV, File3DS);

}



void ReadTriangleObject(Chunk3DS_t *Chunk, Mesh3D *mesh)

{

  switch(Chunk->ID)

  {

    case 0x4110:

      ReadVertexList(mesh);

      break;



    case 0x4120:

      ReadFaceList(mesh, Chunk->Length-6);

      break;



    case 0x4140:

      ReadTextureCoordinates(mesh);

      break;

  }

}



void ReadEditObject(Chunk3DS_t *Chunk, Model3DS_t *Model)

{

  switch(Chunk->ID)

  {

    case 0x4100:

      stx_strlcpy(Model->Mesh[Model->NumMesh].Name, szString,stx_strlen(szString)+1);

      ReadChunkArray(Chunk->Length-6, (cb)ReadTriangleObject, &Model->Mesh[Model->NumMesh++]);

      break;

  }

}



void ReadTexture(Chunk3DS_t *Chunk, Material3DS_t *Material)

{

  switch(Chunk->ID)

  {

    case 0xA300:

      ReadString(Material->TextureID);

      break;

  }

}



void ReadMaterial(Chunk3DS_t *Chunk, Material3DS_t *Material)

{

  switch(Chunk->ID)

  {

    case 0xA000:

      ReadString(Material->Name);

      break;



    case 0xA010:

      ReadChunkArray(Chunk->Length-6, (cb)ReadColorChunk, Material->Ambient);

      Material->Ambient[3]=1.0f;

      break;



    case 0xA020:

      ReadChunkArray(Chunk->Length-6, (cb)ReadColorChunk, Material->Diffuse);

      Material->Diffuse[3]=1.0f;

      break;



    case 0xA030:

      ReadChunkArray(Chunk->Length-6, (cb)ReadColorChunk, Material->Specular);

      Material->Specular[3]=1.0f;

      break;



    case 0xA040:

      ReadChunkArray(Chunk->Length-6, (cb)ReadPercentageChunk, &Material->Shininess);

      Material->Shininess*=140.0f;

      break;



    case 0xA080:

      ReadChunkArray(Chunk->Length-6, (cb)ReadColorChunk, Material->Emission);

      Material->Emission[3]=1.0f;

      break;



    case 0xA200:

      ReadChunkArray(Chunk->Length-6, (cb)ReadTexture, Material);

      break;

  }

}



void ReadEdit3DS(Chunk3DS_t *Chunk, Model3DS_t *Model)

{

  switch(Chunk->ID)

  {

    case 0x4000:

      ReadString(szString);

      ReadChunkArray(Chunk->Length-6+stx_strlen(szString), (cb)ReadEditObject, Model);

      break;



    case 0xAFFF:

      ReadChunkArray(Chunk->Length-6, (cb)ReadMaterial, &Model->Material[Model->NumMaterial++]);

      break;

  }

}



void ReadMain3DS(Chunk3DS_t *Chunk, Model3DS_t *Model)

{

  int i, j, k;



  switch(Chunk->ID)

  {

    case 0x3D3D:

      InspectChunkArray(Chunk->Length-6, (cb)InspectEdit3DS, Model);



      if(Model->NumMesh)

      {

	Model->Mesh=(Mesh3D *)malloc(sizeof(Mesh3D)*Model->NumMesh);

	stx_memset(Model->Mesh, 0, sizeof(Mesh3D)*Model->NumMesh);

      }



      if(Model->NumMaterial)

      {

	Model->Material=(Material3DS_t *)malloc(sizeof(Material3DS_t)*Model->NumMaterial);

	stx_memset(Model->Material, 0, sizeof(Material3DS_t)*Model->NumMaterial);

      }



      Model->NumMesh=0;

      Model->NumMaterial=0;



      ReadChunkArray(Chunk->Length-6, (cb)ReadEdit3DS, Model);



      for(i=0;i<Model->NumMesh;i++)

      {

	for(j=0;j<Model->Mesh[i].NumGroup;j++)

	{

	  for(k=0;k<Model->NumMaterial;k++)

	  {

	    if(!strcmp(Model->Mesh[i].group[j].Name, Model->Material[k].Name))

	    {

	      Model->Mesh[i].group[j].Material=k;

	      break;

	    }

	  }

	}

      }

      break;



    default:

      break;

  }

}



bool load3DS(TransformGroup *parent, bool tbn, char *strFileName)

{

	LOG_PRINT("Factory3DS.cpp:load3DS:strFileName=%s\n", strFileName);


  GeometryInfo *geomtryInfo    = 0;

  Geometry     *geometry       = 0;

  const char   *filePath       = MediaPathManager::lookUpMediaPath(strFileName);

  bool		attachGeometry = true;

  if(!filePath){
    LOG_PRINT("Failed to load the 3DS model -> %s\n", strFileName);
	return;
}





  Model3DS_t *Model = (Model3DS_t*)malloc(sizeof(Model3DS_t));

  int i;

  Chunk3DS_t Chunk;

  char	     stringBuffer[256];



  stx_memset(stringBuffer, 0, 256);

  stx_memset(Model, 0, sizeof(Model3DS_t));

  std::string fn=stx_convertpath(filePath);
  LOG_PRINT("load3DS:fopen=%s\n",fn.c_str());
    File3DS = fopen(fn.c_str(), "rb");



  fread(&Chunk, sizeof(Chunk3DS_t), 1, File3DS);



  if(Chunk.ID==0x4D4D)

    ReadChunkArray(Chunk.Length-6, (cb)ReadMain3DS, Model);

  else

  {

    FREE(Model);

    LOG_PRINT("Failed to load the 3DS model -> %s\n", strFileName);
	return;

  }



  fclose(File3DS);

  modelID++;



  for(i = 0; i < Model->NumMesh; i++)

  {

    stx_snprintf(stringBuffer, 256, "%s_%d", filePath, i);



    Mesh3D *objectPointer = &Model->Mesh[i];

    Shape  *shape	  = new Shape(0, new Appearance());



    geomtryInfo = GeometryManager::getGeometryInfo(stringBuffer);



    if(!geomtryInfo)

    {

		LOG_PRINT("3DS NF=%d\n",objectPointer->NumFace);

		LOG_PRINT("3DS NV=%d\n",objectPointer->NumVertex);

      geometry	  = new Geometry(stringBuffer,

				 objectPointer->NumFace*3,

				 objectPointer->NumVertex);

      geomtryInfo = new GeometryInfo(geometry->getName(), geometry);



      GeometryManager::addGeometryInfo(geomtryInfo);

      geomtryInfo->decreaseUserCount();

      shape->setGeometry(geometry);

      attachGeometry = true;

    }

    else

    {

      attachGeometry = false;

      shape->setGeometry(geomtryInfo->getMedia());

    }



    if(Model->NumMaterial > 0){

      if(objectPointer->group){

	if(Model->Material[objectPointer->group[0].Material].TextureID[0])

	{
	  TextureID texture;
//	  texture.load2D(Model->Material[objectPointer->group[0].Material].TextureID);


	  	  char buf[1024];
	  bool c=false;
	  char* str=Model->Material[objectPointer->group[0].Material].TextureID;
	  	  stx_snprintf(buf,256, "%s\0",strFileName);//filePath);
#if 0
	  int l=stx_strlen(str);
	  if((str[l-4]=='.')&&(str[l-3]=='p')&&(str[l-2]=='n')&&(str[l-1]=='g'))
	  {
		  str[l-3]='t';
		  str[l-2]='g';
		  str[l-1]='a';
	  }
#endif
	  if(stx_strchr(buf,stx_getOsSeparator()))
	  {
		  *stx_strrchr(buf,stx_getOsSeparator())='\0';
	  stx_strlcat(buf,"/\0",1024);

	  }
	  else if(stx_strchr(buf,stx_getOsSeparator()))
	  {
		  *stx_strrchr(buf,stx_getOsSeparator())='\0';
	  stx_strlcat(buf,"/\0",1024);

	  }



	  stx_strlcat(buf,str,1024);

		  texture = IRenderer::GetRendererInstance()->addImageLibTexture(buf, false, IRenderer::GetRendererInstance()->Getlinear());
  	LOG_PRINT("addImageLibTexture:%s:%x\n", buf, texture);

	  shape->getAppearance()->setTexture(0, texture);
	//IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
	//IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	//IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendOneOne());

	}

#if 0
	Material material;

	material.setShininess(Model->Material[objectPointer->group[0].Material].Shininess);

	material.setEmissive(Model->Material[objectPointer->group[0].Material].Emission);

	material.setSpecular(Model->Material[objectPointer->group[0].Material].Specular);

	material.setAmbient(Model->Material[objectPointer->group[0].Material].Ambient);

	material.setDiffuse(Model->Material[objectPointer->group[0].Material].Diffuse);

	shape->getAppearance()->setMaterial(material);
#endif
      }

    }



    if(attachGeometry)

    {

      if(!tbn)

	computeNormals(&Model->Mesh[i]);

      else

	computeTangentSpace(&Model->Mesh[i]);

/*

	v4.x=-1257.257202

(3108): v4.y=0.124995

(3108): v4.z=-638.826721

(3108): v4.x=-1257.257935

(3108): v4.y=0.124994

(3108): v4.z=638.576782

(3108): v4.x=1257.007202

(3108): v4.y=0.124995

(3108): v4.z=-638.826660

(3108): v4.x=1257.007446

(3108): v4.y=0.125005

(3108): v4.z=638.576538

(3108): i6=0

(3108): i6=1

(3108): i6=2

(3108): i6=1

(3108): i6=3

(3108): i6=2



	{D3DXFROMWINEVECTOR3(-1.0f, 1.0f, -1.0f),D3DXFROMWINEVECTOR2(	0.0f,0.0f) },

	{D3DXFROMWINEVECTOR3( 1.0f, 1.0f, -1.0f),D3DXFROMWINEVECTOR2(	1.0f,0.0f) },

	{D3DXFROMWINEVECTOR3(-1.0f,-1.0f, -1.0f),D3DXFROMWINEVECTOR2(	1.0f,1.0f) },

	{D3DXFROMWINEVECTOR3( 1.0f,-1.0f, -1.0f),D3DXFROMWINEVECTOR2(	0.0f,1.0f) }



	 1

	 3

	 0

	 2

	((__DWORD__*)pIndices2)[0]=2;

	((__DWORD__*)pIndices2)[1]=3;

	((__DWORD__*)pIndices2)[2]=1;

	((__DWORD__*)pIndices2)[3]=3;

	((__DWORD__*)pIndices2)[4]=0;

	((__DWORD__*)pIndices2)[5]=1;

*/

       bool bb=strcmp(strFileName,(std::string("/NatureScene/Water/watermesh.3ds")).c_str())==0;

       //LOG_PRINT("strFileName=%s\n",strFileName);

       int sv    = objectPointer->NumVertex;

       int si    = objectPointer->NumFace*3;

	if(bb&&(sv==4)&&(si==6))

      {

	D3DXFROMWINEVECTOR3 vertices2[4];

	 vertices2[0]=objectPointer->vertices[1];

	 vertices2[1]=objectPointer->vertices[3];

	 vertices2[2]=objectPointer->vertices[0];

	 vertices2[3]=objectPointer->vertices[2];

      shape->getGeometry()->setVertices(&vertices2[0].x, false);

      }

      else

      {

      shape->getGeometry()->setVertices(&objectPointer->vertices[0].x, true);

      }

       /*

      if((sv==4)&&(si==6))

      {

      for(int s = 0; s < sv; s++)

      {

			LOG_PRINT("v4.x=%f\n",objectPointer->vertices[s].x);

			LOG_PRINT("v4.y=%f\n",objectPointer->vertices[s].y);

			LOG_PRINT("v4.z=%f\n",objectPointer->vertices[s].z);

      }

      }

	 */

      shape->getGeometry()->setNormals(&objectPointer->normalVecs[0].x, true);



	if(bb&&(sv==4)&&(si==6))

      {

	int indices2[6];
#if 1
	indices2[0]=2;

	indices2[1]=3;

	indices2[2]=1;

	indices2[3]=3;

	indices2[4]=1;

	indices2[5]=0;
#else
#endif
	shape->getGeometry()->setIndices((int*)indices2, false);

      }

      else

      {

	shape->getGeometry()->setIndices((int*)objectPointer->Face, true);

      }



	if(objectPointer->texCoords)

	  shape->getGeometry()->setTextureElements(0, &objectPointer->texCoords[0].x, 2, true);



	if(tbn && objectPointer->texCoords){

	  shape->getGeometry()->setTextureElements(1, &objectPointer->tangentVecs[0].x, 3, true);

	  shape->getGeometry()->setTextureElements(2, &objectPointer->binormalVecs[0].x, 3, true);

	  shape->getGeometry()->overrideByReferenceFlag(TEXTURE1 | TEXTURE2, false);

	}



      shape->getGeometry()->overrideByReferenceFlag(TEXTURE0 | VERTICES | NORMALS | INDICES, false);

      FREE(objectPointer->Face);

    }

    else

    {

      deleteArray(objectPointer->binormalVecs);

      deleteArray(objectPointer->smoothGroup);

		  deleteArray(objectPointer->tangentVecs);

	    deleteArray(objectPointer->normalVecs);

      deleteArray(objectPointer->texCoords);

      deleteArray(objectPointer->vertices);

    }

    parent->addChild(shape);

  }

  for(int i = 0; i < Model->NumMesh; i++)

    if(Model->Mesh[i].group)

    {

      for(int j=0;j<Model->Mesh[i].NumGroup;j++)

	FREE(Model->Mesh[i].group[j].Face);



      FREE(Model->Mesh[i].group);

    }



  LOG_PRINT("Loaded 3DS model -> %s\n", filePath);



  return true;

}





