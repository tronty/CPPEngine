#include "Geometry.h"

#include "ShadowInfo.h"

#include "../Managers/GeometryManager.h"
#ifdef _INCLUDE_SCENEFRAME_
#include "../SceneFrame.h"
#endif

#include <Framework3/importgl.h>

Primitives toD3DPRIMITIVETYPE(int type0)

{

	Primitives type=(Primitives)0;
#if 0
???
	switch(type0)

	{

	case GL_TRIANGLE_STRIP: /*TRIANGLE_STRIP:*/ type=PRIM_TRIANGLE_STRIP;break;

	//case GL_QUAD_STRIP:	  /*QUAD_STRIP:*/     type=PRIM_;break;

	case GL_TRIANGLES:	/*TRIANGLES:*/	    type=PRIM_TRIANGLES;break;

	//case GL_POLYGON:	  /*POLYGON:*/	      type=PRIM_;break;

	case GL_POINTS: 	/*POINTS:*/	    type=PRIM_POINTS;break;

	//case GL_QUADS:	  /*QUADS:*/	      type=PRIM_;break;

	case GL_LINES:		/*LINES:*/	    type=PRIM_LINES;break;

	}
#endif
	return type;

}



Geometry::Geometry(const char* name_, int indicesC, int verticesC) : IOXMLObject("Geometry")

{

  bRESIDE_ON_GFX_DEVICE=false;

	initialize();

  verticesCount = verticesC;

  indicesCount	= indicesC;

  triangleCount = indicesC/3;

  name		= name_;

  FVF=-1;
  VERTEX_FORMATID=-1;

}



Geometry::Geometry(const Geometry &geom)  : IOXMLObject("Geometry")

{

  if(this != &geom)

    initialize();



  this->operator =(geom);

}



Geometry &Geometry::operator =(const Geometry &geom)

{

  if(this != &geom)

  {

    stx_memcpy(byReference, geom.byReference, 12);



    VERTEX_FORMAT     = geom.VERTEX_FORMAT;

    strideSize	      = geom.strideSize;



    infoShadowActive  = geom.infoShadowActive;

    elementsVBOID     = geom.elementsVBOID;

    indicesVBOID      = geom.indicesVBOID;

    verticesCount     = geom.verticesCount;

    indicesCount      = geom.indicesCount;

	triangleCount=geom.triangleCount;

    name	      = geom.name;

    type	      = geom.type;

	FVF=geom.FVF;



    for(int i = 0; i < 8; i++){

      byReference[i] = false;

      setTextureElements(i, geom.textureCoords[i], geom.texElementsCount[i], false);

    }



    setVertices(geom.vertices, false);

    setNormals (geom.normals,  false);

    setIndices (geom.indices,  false);

    setColors  (geom.colors,   false);



#ifdef INFOSHADOW

    deleteObject(infoShadow);

    if(geom.infoShadow)

      activeInfoShadow();

#endif

  }

  return *this;

}



BoundsDescriptor &Geometry::getBoundsDescriptor()

{

  return boundsDescriptor;

}

void   Geometry::setBoundsDescriptor(BoundsDescriptor &descriptor)

{

  boundsDescriptor = descriptor;

}

#ifdef INCLUDESHADOW

ShadowInfo *Geometry::getInfoShadow(){ return infoShadow; }

#endif

int MapToFVF(int element)
{
  #if 0
	int FVF=0;
  #else
  int FVF=element;
  #endif
	return FVF;
}

int GetFVFOffsetD3D(__DWORD__ FVF, __DWORD__ FVFComponent, int TEXComponent)
{
	int FVFOffset=0;
	// Mask position
	__DWORD__ dwPosition = FVF & D3DFVF_POSITION_MASK;

	// Position is always at the start
	if (FVFComponent != 0)
		if(dwPosition&FVFComponent) return FVFOffset;

	if( FVF & D3DFVF_XYZ )
		FVFOffset+=3*sizeof(float);
	else if( FVF & D3DFVF_XYZRHW )
		FVFOffset+=4*sizeof(float);
	else if( FVF & D3DFVF_XYZB1 )
		FVFOffset+=4*sizeof(float);
	else if( FVF & D3DFVF_XYZB2 )
		FVFOffset+=5*sizeof(float);
	else if( FVF & D3DFVF_XYZB3 )
		FVFOffset+=6*sizeof(float);
	else if( FVF & D3DFVF_XYZB4 )
		FVFOffset+=7*sizeof(float);
	else if( FVF & D3DFVF_XYZB5 )
		FVFOffset+=8*sizeof(float);
	if ((FVFComponent & D3DFVF_NORMAL)&&(FVF&D3DFVF_NORMAL)) return FVFOffset;
	if( FVF & D3DFVF_NORMAL )
		FVFOffset+=3*sizeof(float);
	if ((FVFComponent & D3DFVF_PSIZE)&&(FVF&D3DFVF_PSIZE)) return FVFOffset;
	if( FVF & D3DFVF_PSIZE )
		FVFOffset+=1*sizeof(float);
	if ((FVFComponent & D3DFVF_DIFFUSE)&&(FVF&D3DFVF_DIFFUSE)) return FVFOffset;
	if( FVF & D3DFVF_DIFFUSE )
		FVFOffset+=1*sizeof(__WORD__);
	if ((FVFComponent & D3DFVF_SPECULAR)&&(FVF&D3DFVF_SPECULAR)) return FVFOffset;
	if( FVF & D3DFVF_SPECULAR )
		FVFOffset+=1*sizeof(__WORD__);
	__DWORD__ texture = FVF >> 16;
	for (__DWORD__ i=0;i<((FVF&D3DFVF_TEXCOUNT_MASK) >> D3DFVF_TEXCOUNT_SHIFT);i++)
	{
		if (TEXComponent == i) return FVFOffset;
		switch (texture & 3)
		{
			case D3DFVF_TEXTUREFORMAT1:
				FVFOffset+=1*sizeof(float);
				break;
			case D3DFVF_TEXTUREFORMAT2:
				FVFOffset+=2*sizeof(float);
				break;
			case D3DFVF_TEXTUREFORMAT3:
				FVFOffset+=3*sizeof(float);
				break;
			case D3DFVF_TEXTUREFORMAT4:
				FVFOffset+=4*sizeof(float);
				break;
		}
		texture = texture >>2;
	}
	return -1;
}

int GetFVFOffset(int FVF, int FVFComponent, int TEXComponent)
{
	int FVFOffset=0;

	if ((FVFComponent & VERTICES)&&(FVF&VERTICES)) return FVFOffset;
	if( FVF & VERTICES )
		FVFOffset+=3*sizeof(float);
	if ((FVFComponent & NORMALS)&&(FVF&NORMALS)) return FVFOffset;
	if( FVF & NORMALS )
		FVFOffset+=3*sizeof(float);
	if ((FVFComponent & COLOR)&&(FVF&COLOR)) return FVFOffset;
	if( FVF & COLOR )
		FVFOffset+=4*sizeof(float);


	if ((FVFComponent & TEXTURE0)&&(FVF&TEXTURE0)) return FVFOffset;
	if( FVF & TEXTURE0 )
		FVFOffset+=2*sizeof(float);
	if ((FVFComponent & TEXTURE1)&&(FVF&TEXTURE1)) return FVFOffset;
	if( FVF & TEXTURE1 )
		FVFOffset+=2*sizeof(float);
	if ((FVFComponent & TEXTURE2)&&(FVF&TEXTURE2)) return FVFOffset;
	if( FVF & TEXTURE2 )
		FVFOffset+=2*sizeof(float);
	if ((FVFComponent & TEXTURE3)&&(FVF&TEXTURE3)) return FVFOffset;
	if( FVF & TEXTURE3 )
		FVFOffset+=2*sizeof(float);
	if ((FVFComponent & TEXTURE4)&&(FVF&TEXTURE4)) return FVFOffset;
	if( FVF & TEXTURE4 )
		FVFOffset+=2*sizeof(float);
	if ((FVFComponent & TEXTURE5)&&(FVF&TEXTURE5)) return FVFOffset;
	if( FVF & TEXTURE5 )
		FVFOffset+=2*sizeof(float);
	if ((FVFComponent & TEXTURE6)&&(FVF&TEXTURE6)) return FVFOffset;
	if( FVF & TEXTURE6 )
		FVFOffset+=2*sizeof(float);
	if ((FVFComponent & TEXTURE7)&&(FVF&TEXTURE7)) return FVFOffset;
	if( FVF & TEXTURE7 )
		FVFOffset+=2*sizeof(float);

	return -1;
}


void   Geometry::setVertexFormat(int format){ VERTEX_FORMAT = MapToFVF(format); }

VertexFormatID	 Geometry::getVertexFormat(){
if(  VERTEX_FORMATID==-1)
{
int n=0;
int ov=getOffset(VERTICES);
if(ov!=-1) n++;

int on=getOffset(NORMALS);
if(on!=-1) n++;
//int oc=getOffset(COLOR);
//if(oc!=-1) n++;
int o0=getOffset(TEXTURE0);
if(o0!=-1) n++;
int o1=getOffset(TEXTURE1);
if(o1!=-1) n++;
int o2=getOffset(TEXTURE2);
if(o2!=-1) n++;
int o3=getOffset(TEXTURE3);
if(o3!=-1) n++;
int o4=getOffset(TEXTURE4);
if(o4!=-1) n++;
int o5=getOffset(TEXTURE5);
if(o5!=-1) n++;
int o6=getOffset(TEXTURE6);
if(o6!=-1) n++;
int o7=getOffset(TEXTURE7);
if(o7!=-1) n++;




	int i=0;
	FormatDesc* fmt=new FormatDesc[n];/*[] = {
		{ 0,  TYPE_VERTEX, FORMAT_FLOAT, 3},
		{ 0, TYPE_NORMAL, FORMAT_FLOAT, 3},
	       { 0, TYPE_TEXCOORD, FORMAT_FLOAT, 3},
		{ 0, TYPE_TEXCOORD, FORMAT_FLOAT, 3},
		{ 0, TYPE_TEXCOORD, FORMAT_FLOAT, 3},
		{ 0, TYPE_TEXCOORD, FORMAT_FLOAT, 3},
	};*/


if(ov!=-1) 
{
	fmt[i].stream=0;
	fmt[i].format=FORMAT_FLOAT;
	fmt[i].size=3;
	fmt[i++].type=TYPE_VERTEX;
}
if(on!=-1) 
{
	fmt[i].stream=0;
	fmt[i].format=FORMAT_FLOAT;
	fmt[i].size=3;
	fmt[i++].type=TYPE_NORMAL;
}
if(0)//oc!=-1) 
{
	fmt[i].stream=0;
	fmt[i].format=FORMAT_FLOAT;
	fmt[i++].size=4;
	//fmt[i++].type=0;//???TYPE_COLOR;
	DBG_HALT;
}
if(o0!=-1) 
{
	fmt[i].stream=0;
	fmt[i].format=FORMAT_FLOAT;
	fmt[i].size=2;
	fmt[i++].type=TYPE_TEXCOORD;
}
if(o1!=-1) 
{
	fmt[i].stream=0;
	fmt[i].format=FORMAT_FLOAT;
	fmt[i].size=2;
	fmt[i++].type=TYPE_TEXCOORD;
}
if(o2!=-1) 
{
	fmt[i].stream=0;
	fmt[i].format=FORMAT_FLOAT;
	fmt[i].size=2;
	fmt[i++].type=TYPE_TEXCOORD;
}
if(o3!=-1) 
{
	fmt[i].stream=0;
	fmt[i].format=FORMAT_FLOAT;
	fmt[i].size=2;
	fmt[i++].type=TYPE_TEXCOORD;
}
if(o4!=-1) 
{
	fmt[i].stream=0;
	fmt[i].format=FORMAT_FLOAT;
	fmt[i].size=2;
	fmt[i++].type=TYPE_TEXCOORD;
}
if(o5!=-1) 
{
	fmt[i].stream=0;
	fmt[i].format=FORMAT_FLOAT;
	fmt[i].size=2;
	fmt[i++].type=TYPE_TEXCOORD;
}
if(o6!=-1) 
{
	fmt[i].stream=0;
	fmt[i].format=FORMAT_FLOAT;
	fmt[i].size=2;
	fmt[i++].type=TYPE_TEXCOORD;
}
if(o7!=-1) 
{
	fmt[i].stream=0;
	fmt[i].format=FORMAT_FLOAT;
	fmt[i].size=2;
	fmt[i++].type=TYPE_TEXCOORD;
}

	ShaderID id=-1;
#ifdef _INCLUDE_SCENEFRAME_
	if(1)
		id=g_SceneFrame->getSkyShader();
	//else id=g_SceneFrame->getWaterShader();
#endif
	
	VERTEX_FORMATID = IRenderer::GetRendererInstance()->addVertexFormat(fmt, n, id);
}
	int r=VERTEX_FORMATID;
	return r;
}



void   Geometry::setVerticesCount(uint count){ verticesCount = count; }

uint    Geometry::getVerticesCount()	    { return verticesCount;  }



void   Geometry::setIndicesCount(uint count){ indicesCount = count;triangleCount=count/3; }

__DWORD__	 Geometry::getIndicesCount()	     { return indicesCount;  }



void   Geometry::setStrideSize(uint i){ strideSize = i;	  }

uint    Geometry::getStrideSize()     { return strideSize; }



bool   Geometry::setType(Primitives geomType)

{

  switch(geomType)

  {

    case PRIM_TRIANGLE_STRIP:

    case PRIM_TRIANGLES:

    case PRIM_POINTS:

    case PRIM_LINES:

      type = geomType;

    return true;

/*

    case GL_QUAD_STRIP:

    case GL_POLYGON:

    case GL_QUADS:

*/

	default:

	LOG_PRINT("unsupported primitive type %d\n",geomType);

	return false;

  }

  return false;

}



Primitives    Geometry::getType()	   { return type;     }



void   Geometry::setElementsVBOID(VertexBufferID id){ elementsVBOID  = id;  }

VertexBufferID	  Geometry::getElementsVBOID()	    { return elementsVBOID; }



void   Geometry::setIndicesVBOID(IndexBufferID id){ indicesVBOID  = id;  }

IndexBufferID	  Geometry::getIndicesVBOID()	   { return indicesVBOID; }



float* Geometry::getVertices()	 { return vertices;	     }

float* Geometry::getNormals()	 { return normals;	     }

float* Geometry::getColors()	 { return colors;	     }

__DWORD__*	 Geometry::getIndices()    { return indices;	       }



int Geometry::getOffset(int geomElement)

{

  int offset = 0;



  switch(geomElement)

  {

    case TEXTURE0: offset  = GetFVFOffset(geomElement, TEXTURE0, 0);

    break;

    case TEXTURE1: offset  = GetFVFOffset(geomElement, TEXTURE1, 1);

    break;

    case TEXTURE2: offset  = GetFVFOffset(geomElement, TEXTURE2, 2);

    break;

    case TEXTURE3: offset  = GetFVFOffset(geomElement, TEXTURE3, 3);

    break;

    case TEXTURE4: offset  = GetFVFOffset(geomElement, TEXTURE4, 4);

    break;

    case TEXTURE5: offset  = GetFVFOffset(geomElement, TEXTURE5, 5);

    break;

    case TEXTURE6: offset  = GetFVFOffset(geomElement, TEXTURE6, 6);

    break;

    case TEXTURE7: offset  = GetFVFOffset(geomElement, TEXTURE7, 7);

    break;

    case COLOR:   offset   = GetFVFOffset(geomElement, COLOR, 0);

    break;

    case NORMALS:  offset  = GetFVFOffset(geomElement, NORMALS, 0);

    break;

    case VERTICES: offset  = GetFVFOffset(geomElement, VERTICES, 0);

    break;

  }



  return offset;

}



int  Geometry::getTriangleCount()

{

  triangleCount = (type == PRIM_TRIANGLE_STRIP) ? indicesCount - 2:

		  (type == PRIM_TRIANGLE_FAN  ) ? indicesCount - 2:

		  /*(type == PRIM_TRIANGLES	) ? */indicesCount/3;//  :

		  //???(type == GL_QUADS	 ) ? indicesCount/2  : 0;

  return triangleCount;

}

#ifdef INFOSHADOW

bool Geometry::activeInfoShadow()

{

  if(infoShadow)

    return true;



  if(!indicesCount   ||

     !verticesCount  ||

     !indices	     ||

     type != PRIM_TRIANGLES)

  {LOG_PRINT("Couldn't activate the shadow info");return false;}



  #ifdef INCLUDESHADOW

  infoShadow = new ShadowInfo(this);

  infoShadow->constructEdges();

#endif

  return true;

}



#endif

void Geometry::initialize(){

  stx_memset(byReference, false, 12);



  for(int i = 0; i < 8; i++)

  {

    texElementsCount[i] = 0;

    textureCoords[i]	 = 0;

  }



  VERTEX_FORMAT    = D3DFVF_XYZ;

#ifdef INFOSHADOW

  infoShadow	   = 0;

#endif

  vertices	   = 0;

  normals	   = 0;

  indices	   = 0;

  colors	   = 0;



  elementsVBOID    = -1;

  indicesVBOID	   = -1;

  verticesCount    = 0;

  indicesCount	   = 0;

  strideSize	   = 0;

  type		   = PRIM_TRIANGLES;

}



void Geometry::setTextureElementsCount(int index, int elementSize){

  if(index < 0 || index > MAX_TEX_UNITS)

    return;

  texElementsCount[index]	= elementSize;

}



uint Geometry::getTextureElementsCount(uint index)

{

  return  index >= 0 && index < MAX_TEX_UNITS ? texElementsCount[index]: 0;

}



float* Geometry::getTextureCoords(int index)

{

  return index < 0 || index > MAX_TEX_UNITS ? 0 : textureCoords[index];

}



void Geometry::setTextureElements(int index	 , float* pointer,

				  int elementSize, bool byRef)

{



  if(index < 0 || index > MAX_TEX_UNITS) return;

  if(!pointer)	return;



  if(!byReference[index])

    delete[] textureCoords[index];



  VERTEX_FORMAT 	 |= (TEXTURE0 <<index);

  byReference[index]	  = byRef;

  texElementsCount[index] = elementSize;



  if(byRef)

    textureCoords[index] = pointer;

  else{

    textureCoords[index]     = new float[verticesCount*elementSize];

    stx_memcpy(textureCoords[index], pointer, verticesCount*sizeof(float)*elementSize);

  }

}



void Geometry::setColors(float* crs, bool byRef){

  if(!crs)	      return;

  if(!byReference[8]) delete[] colors;



  byReference[8] = byRef;

  VERTEX_FORMAT |= D3DFVF_DIFFUSE;



  if(byReference[8])

    colors = crs;

  else{

    colors = new float[verticesCount*3];

    stx_memcpy(colors, crs, verticesCount*sizeof(float)*3);

  }

}



void Geometry::setNormals(float* normalsCoords, bool byRef)

{

  if(!normalsCoords)  return;

  if(!byReference[9]) delete[] normals;



  byReference[9] = byRef;

  VERTEX_FORMAT |= D3DFVF_NORMAL;



  if(byRef)

    normals = normalsCoords;

  else{

    normals = new float[verticesCount*3];

    stx_memcpy(normals, normalsCoords, verticesCount*sizeof(float)*3);

  }

}



void Geometry::setVertices(float* verticesCoords, bool byRef)

{

  if(!verticesCoords)

    return;



  if(!byReference[10]) delete[] vertices;



  byReference[10] = byRef;



  if(byRef)

    vertices = verticesCoords;

  else{

    vertices = new float[verticesCount*3];

    stx_memcpy(vertices, verticesCoords, verticesCount*sizeof(float)*3);

  }



   float minX =  1000000.0f,

	 minY =  1000000.0f,

	 minZ =  1000000.0f,

	 maxX = -1000000.0f,

	 maxY = -1000000.0f,

	 maxZ = -1000000.0f;



  D3DXFROMWINEVECTOR3* indexedVectors = (D3DXFROMWINEVECTOR3*)vertices,

	 * temp 	  = 0;



  //if(computeBounds)

  {

    for(int i = 0; i < verticesCount; i++)

    {

      temp = &indexedVectors[i];

      if(temp->x < minX) minX = temp->x;

      if(temp->x > maxX) maxX = temp->x;



      if(temp->y < minY) minY = temp->y;

      if(temp->y > maxY) maxY = temp->y;



      if(temp->z < minZ) minZ = temp->z;

      if(temp->z > maxZ) maxZ = temp->z;

    }

    boundsDescriptor.computeBounds(minX, minY, minZ, maxX, maxY, maxZ);

  }

}


void Geometry::setIndices(int * ind, bool byRef){

  if(!ind)

   return;



  if(!byReference[11]) delete[] indices;



  byReference[11] = byRef;


/*
  if(byRef)

  {

   indices = ind;

  }

  else
  */
  {

    indices   = new __DWORD__[indicesCount];



      for(int i=0;i<indicesCount;i++)

      {

	indices[i]=ind[i];//???

      }

      //setIndices(ind, true);





  }

}





#if 0
void Geometry::setIndices(__DWORD__ * ind, bool byRef){

  if(!ind)

   return;



  if(!byReference[11]) delete[] indices;



  byReference[11] = byRef;



  if(byRef)

  {

   indices = ind;

  }

  else{

    indices   = new __DWORD__[indicesCount];



      for(int i=0;i<indicesCount;i++)

      {

	indices[i]=ind[i];//???

      }

      //setIndices(ind, true);





  }

}
#endif




void Geometry::setIndices(__DWORD__ * ind, bool byRef){

  if(!ind)

   return;



  if(!byReference[11]) delete[] indices;



  byReference[11] = byRef;

/*

  if(byRef)

  {

   //??? indices = ind;

  }

  else{*/

    indices   = new __DWORD__[indicesCount];



      for(int i=0;i<indicesCount;i++)

      {

	indices[i]=ind[i];//???

      }

      //setIndices(ind, true);

	  //delete[] ind;





  //}

}



void Geometry::overrideByReferenceFlag(int element, bool val)

{

  if(element & TEXTURE0) byReference[0] = val;

  if(element & TEXTURE1) byReference[1] = val;

  if(element & TEXTURE2) byReference[2] = val;

  if(element & TEXTURE3) byReference[3] = val;

  if(element & TEXTURE4) byReference[4] = val;

  if(element & TEXTURE5) byReference[5] = val;

  if(element & TEXTURE6) byReference[6] = val;

  if(element & TEXTURE6) byReference[7] = val;



  if(element & COLOR	) byReference[ 8] = val;

  if(element & NORMALS	) byReference[ 9] = val;

  if(element & VERTICES ) byReference[10] = val;

  if(element & INDICES	) byReference[11] = val;

}



bool Geometry::isByReference(int element)

{

  switch(element)

  {

    case TEXTURE0: return byReference[ 0]; break;

    case TEXTURE1: return byReference[ 1]; break;

    case TEXTURE2: return byReference[ 2]; break;

    case TEXTURE3: return byReference[ 3]; break;

    case TEXTURE4: return byReference[ 4]; break;

    case TEXTURE5: return byReference[ 5]; break;

    case TEXTURE6: return byReference[ 6]; break;

    case TEXTURE7: return byReference[ 7]; break;

    case COLOR:    return byReference[ 8]; break;

    case NORMALS:  return byReference[ 9]; break;

    case VERTICES: return byReference[10]; break;

    case INDICES:  return byReference[11]; break;

  }

  return false;

}



void Geometry::cleanUpAfterCompilation()

{

  if(!byReference[8])  delete[] colors;

  if(!byReference[9])  delete[] normals;

  if(!byReference[10]) delete[] vertices;

  if(!byReference[11]) delete[] indices;



  for(int i = 0; i < 8; i++)

    if(!byReference[i])

      delete[] textureCoords[i];



  stx_memset(byReference, false, 12);

  LOG_PRINT("Cleaning up geometry after compilation -> %s\n",name.c_str());

}



bool Geometry::compile()

{



  if(bRESIDE_ON_GFX_DEVICE)

    return true;



  bool bD=((this->getVerticesCount()==4)&&(this->getIndicesCount()==6));

	if(bD) LOG_PRINT("Compiling geometry -> %s\n",name.c_str());

  LOG_PRINT("Compiling geometry -> %s\n",name.c_str());



//  if(!GLEE_ARB_vertex_buffer_object)

//    {LOG_PRINT("Failed: VBO extension not supported");return;}





  if(!getVertices())

    {LOG_PRINT("Failed: 0 vertices");return false;}





  float      *GPUInterleavedArray   = 0;

  int	      interleavedBufferSize = 0,

	     *GPUIndices	    = 0,

	      supported 	    = 0;



  VertexBufferID      vboID		     =	  -1;

  IndexBufferID indicesVBOID	       =    -1;





   for(int i = 0; i < 8; i++)

   {

     supported = VERTEX_FORMAT & (TEXTURE0 << i);

     if(supported)

       interleavedBufferSize += getTextureElementsCount(i);

     else

       break;

  }





  if((VERTEX_FORMAT & D3DFVF_XYZ) && getVertices()) interleavedBufferSize += 3;





	if((VERTEX_FORMAT & D3DFVF_NORMAL ) && getNormals() ) interleavedBufferSize += 3;





	if((VERTEX_FORMAT & D3DFVF_DIFFUSE   ) && getColors()  ) interleavedBufferSize += 1;//3;





  setStrideSize(interleavedBufferSize*4);



//if(bD)LOG_PRINT("SV0=%d\n",interleavedBufferSize*4);

  interleavedBufferSize *= getVerticesCount();

//if(bD)LOG_PRINT("getVerticesCount()=%d\n",getVerticesCount());





/*

  glGenBuffersARB(1, &vboID);

  glBindBufferARB(GL_ARRAY_BUFFER_ARB, vboID);

  glBufferDataARB(GL_ARRAY_BUFFER_ARB, interleavedBufferSize * sizeof(float),

		  0, GL_STATIC_DRAW_ARB);



*/

  //if(glGetError() == GL_NO_ERROR)

    GPUInterleavedArray = new float[interleavedBufferSize];



	//glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);

/*  else

   {LOG_PRINT("Not enough memory for the interleaved geometry arrays");return;}

*/

  float  *texCoorArray	= 0,

	 *vertexArray	= getVertices(),

	 *normalArray	= getNormals(),

	 *colorArray	= getColors();

  int	  texElemSize	= 0,

	  s;



	int i = 0;

	int t = 0;

	int sss=0;

	this->FVF=0;

	__WORD__ ct=0;

	for(i = 0, t = 0; i < interleavedBufferSize; t++)

  {

	  sss=0;

	  if((VERTEX_FORMAT & D3DFVF_XYZ) && vertexArray)

    {

		sss+=3*sizeof(float);

		GPUInterleavedArray[i++] = vertexArray[t*3 + 0];

      GPUInterleavedArray[i++] = vertexArray[t*3 + 1];

      GPUInterleavedArray[i++] = vertexArray[t*3 + 2];

	  this->FVF|=D3DFVF_XYZ;

	  //if(bD)LOG_PRINT("D3DFVF_XYZ\n");

    }



    if((VERTEX_FORMAT & D3DFVF_NORMAL) && normalArray)

    {

		sss+=3*sizeof(float);

		GPUInterleavedArray[i++] = normalArray[t*3 + 0];

      GPUInterleavedArray[i++] = normalArray[t*3 + 1];

      GPUInterleavedArray[i++] = normalArray[t*3 + 2];

			  this->FVF|=D3DFVF_NORMAL;

	  //if(bD)LOG_PRINT("D3DFVF_NORMAL\n");

	}

    if((VERTEX_FORMAT & D3DFVF_DIFFUSE) && colorArray)

    {

	sss+=1*sizeof(D3DXFROMWINEVECTOR4);//???

      GPUInterleavedArray[i++] = colorArray[t*3 + 0];
      GPUInterleavedArray[i++] = colorArray[t*3 + 1];
      GPUInterleavedArray[i++] = colorArray[t*3 + 2];
      GPUInterleavedArray[i++] = 1.0f;

	this->FVF|=D3DFVF_DIFFUSE;

		  //if(bD)LOG_PRINT("D3DFVF_DIFFUSE\n");

    }

    for(s = 0; s < 8; s++)

    {

		ct=0;

      texElemSize  = getTextureElementsCount(s);

      texCoorArray = getTextureCoords(s);

	if(VERTEX_FORMAT & (TEXTURE0 << s))

      {

		  ct++;

	switch(texElemSize)

	{



	  case 1:

		  {

			  GPUInterleavedArray[i++] = texCoorArray[t*1 + 0];

			  sss+=1*sizeof(float);

			  this->FVF|=D3DFVF_TEXCOORDSIZE1(s);

				  //if(bD)LOG_PRINT("D3DFVF_TEXCOORDSIZE%d(%d)\n",1,s);

				break;

		  }

	  case 2:

		  {

			  GPUInterleavedArray[i++] = texCoorArray[t*2 + 0];

			  GPUInterleavedArray[i++] = texCoorArray[t*2 + 1];

			  sss+=2*sizeof(float);

			  this->FVF|=D3DFVF_TEXCOORDSIZE2(s);

				  //if(bD)LOG_PRINT("D3DFVF_TEXCOORDSIZE%d(%d)\n",2,s);

			  break;

		  }

	  case 3:

		  {

			  GPUInterleavedArray[i++] = texCoorArray[t*3 + 0];

		  GPUInterleavedArray[i++] = texCoorArray[t*3 + 1];

		  GPUInterleavedArray[i++] = texCoorArray[t*3 + 2];

		  sss+=3*sizeof(float);

		  this->FVF|=D3DFVF_TEXCOORDSIZE3(s);

				  //if(bD)LOG_PRINT("D3DFVF_TEXCOORDSIZE%d(%d)\n",3,s);

		  break;

		  }

	}

      }

      else

	break;

    }



  }



	switch(ct)

	{

		case 1:

			FVF|=D3DFVF_TEX1;

			//if(bD)LOG_PRINT("D3DFVF_TEX1\n");

			break;

		case 2:

			FVF|=D3DFVF_TEX2;

			//if(bD)LOG_PRINT("D3DFVF_TEX2\n");

			break;

		case 3:

			FVF|=D3DFVF_TEX3;

			//if(bD)LOG_PRINT("D3DFVF_TEX3\n");

			break;

		case 4:

			FVF|=D3DFVF_TEX4;

			//if(bD)LOG_PRINT("D3DFVF_TEX4\n");

			break;

		case 5:

			FVF|=D3DFVF_TEX5;

			//if(bD)LOG_PRINT("D3DFVF_TEX5\n");

			break;

		case 6:

			FVF|=D3DFVF_TEX6;

			//if(bD)LOG_PRINT("D3DFVF_TEX6\n");

			break;

		case 7:

			FVF|=D3DFVF_TEX7;

			//if(bD)LOG_PRINT("D3DFVF_TEX7\n");

			break;

		case 8:

			FVF|=D3DFVF_TEX8;

			//if(bD)LOG_PRINT("D3DFVF_TEX8\n");

			break;

	}



/*

	glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

*/

		vboID = IRenderer::GetRendererInstance()->addVertexBuffer(interleavedBufferSize * sizeof(float), STATIC, GPUInterleavedArray);

	LOG_FNLN;
	LOG_PRINT("sss=%d\n",sss);
	LOG_PRINT("interleavedBufferSize=%d\n",interleavedBufferSize);

  if(indices)

  {

/*

    glGenBuffersARB(1, &indicesVBOID);

    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, indicesVBOID);

    glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, getIndicesCount()*sizeof(int),

		    getIndices(), GL_STATIC_DRAW_ARB);



    if(glGetError() != GL_NO_ERROR)

    {

      glDeleteBuffersARB(1, &indicesVBOID);

      indicesVBOID = 0;

      {LOG_PRINT("Error creating index buffer");return;}

    }



    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

*/

		__DWORD__* i0=getIndices();
/*
  int *Ptr=new int[getIndicesCount()];

  int i=0;


	for( i = 0; i < getIndicesCount(); i++)

  {

	  Ptr[i]=i0[i];

	}*/

	indicesVBOID=IRenderer::GetRendererInstance()->addIndexBuffer(getIndicesCount(),sizeof(__WORD__), STATIC,i0);
	setIndicesVBOID(indicesVBOID);

	LOG_FNLN;
	LOG_PRINT("getIndicesCount()=%d\n",getIndicesCount());

	//for(__DWORD__ i = 0; i < getIndicesCount(); i++)
	//	LOG_PRINT("sky i0=%d\n",i0[i]);


	//delete[] Ptr;

  }

  setElementsVBOID(vboID);





  bRESIDE_ON_GFX_DEVICE=true;



  return true;

}



bool Geometry::loadXMLSettings(XMLElement *element)

{

  XMLElement *node	 = 0,

	     *child	 = 0,

	     *subChild	 = 0;



  if(!isSuitable(element))

    return false;



  if(child = element->getChildByName("description"))

    return IOXMLObject::loadXMLSettings(child->getValuec());



  destroy();



  for(size_t i = 0; i < element->getChildrenCount(); i++)

  {

    node = element->getChild(i);

    if(!node) continue;

    const NSString &name = node->getName();



    if(name == "indicesCount")

    {

      setIndicesCount(node->getValuei());

      continue;

    }

#if 0
    if(name == "type")

    {

      int type0 = /*(Primitives)???*/getXMLType(node->getValue());

      if(type0 == -1)

      {

	destroy();

	{LOG_PRINT("Unknown Geometry type -> ") + node->getValue());return false;}

      }

	  type=(Primitives)type0;//???toD3DPRIMITIVETYPE(type0);

      continue;

    }
#endif

    if(name == "TexCoords")

    {

      int id	       = 0,

	  size	       = 0;

      float coeffX     = 1.0f,

	    coeffY     = 1.0f;



      if(!(child = node->getChildByName("id")))

      {

	LOG_PRINT("XML node <TextureID> must have an <id> child\n");

	return 0;

      }



      id = child->getValuei();



      if(!(child = node->getChildByName("size")))

      {

	LOG_PRINT("XML node <TextureID> must have a <size> child\n");

	return 0;

      }



      size = child->getValuei();



      if(child = node->getChildByName("coeffX"))

	coeffX = clampNS(child->getValuef(), 0.1f, 20.0f);



      if(child = node->getChildByName("coeffY"))

	coeffY = clampNS(child->getValuef(), 0.1f, 20.0f);



      if(!(child = node->getChildByName("RawData")))

      {

	LOG_PRINT("XML node <TextureID> must have a <RawData> child\n");

	return 0;

      }



      child->rawData.skipDelete = true;

      setVerticesCount(child->rawData.byteCount/(size*sizeof(float)));



      if(coeffX != 1.0f || coeffY != 1.0f)

      {

	float *texCoords  = (float*)child->rawData.data;

	int    floatCount =  child->rawData.byteCount/sizeof(float);

	for(int t = 0; t < floatCount; t+=2)

	{

	  texCoords[t	 ] *=  coeffX;

	  texCoords[t + 1] *=  coeffY;

	}

      }



      setTextureElements(id, (float*)child->rawData.data, size, true);

      overrideByReferenceFlag((0x1 << id), false);

      continue;

    }



    if(name == "Vertices")

    {

      if(!(child = node->getChildByName("RawData")))

      {

	LOG_PRINT("XML node <Vertices> must have a <RawData> child\n");

	return 0;

      }

      child->rawData.skipDelete = true;

      setVerticesCount(child->rawData.byteCount/(3*sizeof(float)));

      setVertices((float*)child->rawData.data, true);

      overrideByReferenceFlag(VERTICES, false);

      continue;

    }



    if(name == "Normals")

    {

      if(!(child = node->getChildByName("RawData")))

      {

	LOG_PRINT("XML node <Normals> must have a <RawData> child\n");

	return 0;

      }

      child->rawData.skipDelete = true;

      setVerticesCount(child->rawData.byteCount/(3*sizeof(float)));

      setNormals((float*)child->rawData.data, true);

      overrideByReferenceFlag(NORMALS, false);

      continue;

    }



    if(name == "Indices")

    {

      if(!(child = node->getChildByName("RawData")))

      {

	LOG_PRINT("XML node <Indices> must have a <RawData> child\n");

	return 0;

      }

      child->rawData.skipDelete = true;

      setIndicesCount(child->rawData.byteCount/sizeof(int));

      #if 1

      setIndices((int*)child->rawData.data, true);//???

      #else

      int ni=child->rawData.byteCount/sizeof(int);

      int* ind=new int[ni];

      for(int i=0;i<ni;i++)

      {

	ind[i]=child->rawData.data[i];//???

      }

      setIndices(ind, true);

      delete[] ind;

      #endif

      overrideByReferenceFlag(INDICES, false);

      continue;

    }

  }

  return true;

}



bool Geometry::exportXMLSettings(ofstream &xmlFile)

{

  if(!xmlFile.is_open())

    {LOG_PRINT("Cannot export Geometry to XML: file not ready.\n");return false;}



  if(bRESIDE_ON_GFX_DEVICE)

    {LOG_PRINT("Cannot export Geometry to XML: compiled.\n");return false;}



  char *rawData = 0;

  int	count	= 0;



  xmlFile << "<Geometry type	     = \"";



  switch(type)

  {

    case PRIM_TRIANGLE_STRIP: xmlFile << "TRIANGLE_STRIP\"\n"; break;

    //case GL_QUAD_STRIP:     xmlFile << "QUAD_STRIP\"\n";     break;

    case PRIM_TRIANGLES:	  xmlFile << "TRIANGLES\"\n";	   break;

    //case GL_POLYGON:	      xmlFile << "POLYGON\"\n";        break;

    case PRIM_POINTS:	  xmlFile << "POINTS\"\n";	   break;

    //case GL_QUADS:	      xmlFile << "QUADS\"\n";	       break;

    case PRIM_LINES:	  xmlFile << "LINES\"\n";	   break;

    default:

     {LOG_PRINT("Cannot export Geometry to XML: unknown type.\n");return false;}

  }



  xmlFile << "		indicesCount = \"" << getIndicesCount() << "\">\n";



  for(int i = 0; i < 10; i++)

  {

    rawData = (char*)getTextureCoords(i);

    if(!rawData)

      break;



    count = getTextureElementsCount(i) *

	    getVerticesCount()	       *

	    sizeof(float);



    xmlFile << "  <TexCoords id   = \"" << i << "\"\n";

    xmlFile << "	     size = \"" << getTextureElementsCount(i) << "\">\n";

    xmlFile << "    <RawData length = \"" << count << "\">";

    xmlFile.write(rawData, count);

    xmlFile << "    </RawData>\n";

    xmlFile << "  </TexCoords>\n";

  }



  rawData = (char*)getVertices();



  if(rawData)

  {

    count =  getVerticesCount() *  sizeof(float) * 3;

    xmlFile << "  <Vertices>\n";

    xmlFile << "    <RawData length = \"" << count << "\">";

    xmlFile.write(rawData, count);

    xmlFile << "    </RawData>\n";

    xmlFile << "  </Vertices>\n";

  }



  rawData = (char*)getIndices();



  if(rawData)

  {

    count =  getIndicesCount() *  sizeof(int);

    xmlFile << "  <Indices>\n";

    xmlFile << "    <RawData length = \"" << count << "\">";

    xmlFile.write(rawData, count);

    xmlFile << "    </RawData>\n";

    xmlFile << "  </Indices>\n";

  }



  rawData = (char*)getNormals();



  if(rawData)

  {

    count =  getVerticesCount() *  sizeof(float) * 3;

    xmlFile << "  <Normals>\n";

    xmlFile << "    <RawData length = \"" << count << "\">";

    xmlFile.write(rawData, count);

    xmlFile << "    </RawData>\n";

    xmlFile << "  </Normals>\n";

  }

  xmlFile << "</Geometry>\n";

  return true;

}



int Geometry::getXMLType(const NSString &value)

{

  if(value.getLength())

  {

	  if(value == "TRIANGLE_STRIP") {return int(PRIM_TRIANGLE_STRIP);}

    if(value == "QUAD_STRIP")	  {LOG_PRINT("unsupported primitive GL_QUAD_STRIP\n");return -1;}//GL_QUAD_STRIP;

    if(value == "TRIANGLES")	  {return int(PRIM_TRIANGLES);}

    if(value == "POLYGON")	  {LOG_PRINT("unsupported primitive GL_POLYGON\n");return -1;}//GL_POLYGON;

    if(value == "POINTS")	  {return int(PRIM_POINTS);}

    if(value == "QUADS")	  {LOG_PRINT("unsupported primitive GL_QUADS\n");return -1;}//GL_QUADS;

	if(value == "LINES")	      {return int(PRIM_LINES);}

  }

  return -1;

}



bool Geometry::destroy()

{

  GeometryInfo *geomtryInfo = GeometryManager::getGeometryInfo(name.c_str());



  if(geomtryInfo)

  {

    geomtryInfo->decreaseUserCount();

    if(geomtryInfo->getUserCount() > 0)

      return false;

  }



  if(!byReference[8])  delete[] colors;

  if(!byReference[9])  delete[] normals;



  if(!byReference[10]) delete[] vertices;

  if(!byReference[11]) delete[] indices;



  for(int i = 0; i < MAX_TEX_UNITS; i++)

    if(!byReference[i])

      delete[] textureCoords[i];

#ifdef INFOSHADOW

  deleteObject(infoShadow);

#endif


  GeometryManager::flushUnusedGeometry();

  return true;

}

