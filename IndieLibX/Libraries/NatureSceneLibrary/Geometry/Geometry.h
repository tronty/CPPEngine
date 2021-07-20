#ifndef GEOMETRY_H

#define GEOMETRY_H



#include "BoundsDescriptor.h"
#include <Parsers/IOXMLObject.h>



#ifdef INCLUDESHADOW

//???friend class ShadowInfo;

class ShadowInfo;

#endif

//\geometry.h(6) : error C2255: 'friend' : not allowed outside of a class definition

class Geometry	: //public NamedObject
		public IOXMLObject

{

public:
    const char* getName(){return name.c_str();}

    uint     texElementsCount[MAX_TEX_UNITS];

private:
	std::string name;

	uint

	    VERTEX_FORMAT;
	VertexFormatID

	    VERTEX_FORMATID;

public:

   uint	 triangleCount,

	    verticesCount;

	__DWORD__    indicesCount;

	    VertexBufferID elementsVBOID;

	    IndexBufferID indicesVBOID;

	uint strideSize;

	   __DWORD__ *indices;

	   Primitives type;

	   VertexFormatID FVF;

  private:

    bool    infoShadowActive,

	    byReference[12];



    float  *textureCoords[MAX_TEX_UNITS],

	   *vertices,

	   *normals,

	   *colors;



    BoundsDescriptor boundsDescriptor;

#ifdef INCLUDESHADOW

    ShadowInfo*      infoShadow;

#endif

    void	     initialize();

    int 	     getXMLType(const NSString &);



  public:

    Geometry(const Geometry &geom);

    Geometry(const char* name, int indicesCount = 0, int verticesCount = 0);

    Geometry &operator =(const Geometry &geom);



    virtual bool loadXMLSettings(XMLElement *element);

    virtual bool exportXMLSettings(ofstream &xmlFile  );



    bool   destroy();



    BoundsDescriptor &getBoundsDescriptor();

    void   setBoundsDescriptor(BoundsDescriptor &descriptor);



    bool   compile();



    void   setVertexFormat(int format);

    VertexFormatID   getVertexFormat();

	bool   bRESIDE_ON_GFX_DEVICE;

	bool   RESIDE_ON_GFX_DEVICE(){return bRESIDE_ON_GFX_DEVICE;};



    void   setTextureElements(int, float*, int, bool);

    void   setTextureElementsCount(int, int);

    uint    getTextureElementsCount(uint index);



    void   setElementsVBOID(VertexBufferID id);

    VertexBufferID    getElementsVBOID();



    void   setIndicesVBOID(IndexBufferID id);

    IndexBufferID    getIndicesVBOID();



    bool   setType(Primitives type = PRIM_TRIANGLES);

    Primitives	getType();



    void   setStrideSize(uint i);

    uint    getStrideSize();



    void   setIndicesCount(uint count);

    void   setIndices(int*, bool);
    //void   setIndices(uint*, bool);
    void   setIndices(__DWORD__*, bool);

    __DWORD__*	 getIndices();

    __DWORD__    getIndicesCount();



    void   setVerticesCount(uint count);

    void   setVertices(float *vertices, bool byReference);

    float* getVertices();

    uint    getVerticesCount();



    void   setNormals(float* normals, bool byReference);

    float* getNormals();



    void   setColors(float* colors, bool byReference);

    float* getColors();

#ifdef INCLUDESHADOW

    ShadowInfo *getInfoShadow();

    bool	activeInfoShadow();

#endif

    void	deleteBuffers();



    void   overrideByReferenceFlag(int element, bool val);

    bool   isByReference(int element);



    void   cleanUpAfterCompilation();



    float* getTextureCoords(int index);

    int    getTriangleCount();

    int    getOffset(int geomElement);

};

#endif



