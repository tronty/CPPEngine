#include <Nodes/TransformGroup.h>

//include "MathUtils.h"



#ifndef FACTORY3DS_H

#define FACTORY3DS_H



#pragma pack(push)

#pragma pack(1)



typedef struct

{

	unsigned short ID;

	unsigned int Length;

} Chunk3DS_t;



typedef struct

{

	char  Name[64];

	D3DXFROMWINEVECTOR4 Ambient,

	        Diffuse,

          Specular,

          Emission;

	float Shininess;

	char  TextureID[32];

} Material3DS_t;



typedef struct

{

	char Name[64];

	int  Material,

	     Start,

	     Size;

	unsigned short *Face;



} Group3DS;



typedef struct

{

  D3DXFROMWINEVECTOR3 *binormalVecs,

		      *tangentVecs,

	        *normalVecs,

          *vertices;



  D3DXFROMWINEVECTOR2 *texCoords;



  char     Name[64];



  int      NumUV,

           NumFace,

           NumGroup,

           NumVertex,

          *smoothGroup;



  unsigned short *Face;

  Group3DS *group;

} Mesh3D;



typedef struct

{

	int NumMaterial;

	Material3DS_t *Material;



	int NumMesh;

	Mesh3D *Mesh;

} Model3DS_t;



#pragma pack(pop)



bool load3DS(TransformGroup *group, bool tbn, char *strFileName);



#endif

