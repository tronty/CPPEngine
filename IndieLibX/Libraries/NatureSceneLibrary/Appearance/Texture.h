#ifndef TEXTURE_H

#define TEXTURE_H



#include "../Managers/TexturesManager.h"

//include "TextureCoordGenerator.h"

//include "../Renderer/GLPBuffer.h"

#include "../Nodes/Transform3D.h"



typedef enum _TextureType

{

	TEXTURE1D,

	TEXTURE2D,

	TEXTURE3D,

	TEXTURECUBE

} TextureType;

class NVTexture : public IOXMLObject

{

  public:
    TextureID id;

	SamplerStateID samplerState;
    NVTexture(TextureType target = TEXTURE2D);

    NVTexture(const NVTexture &copy);

    NVTexture &operator = (const NVTexture &);

   ~NVTexture();



    virtual bool loadXMLSettings(XMLElement *element);

    virtual bool exportXMLSettings(ofstream &xmlFile  );

/*

typedef enum _D3DTEXTUREADDRESS {

    D3DTADDRESS_WRAP		= 1,

    D3DTADDRESS_MIRROR		= 2,

    D3DTADDRESS_CLAMP		= 3,

    D3DTADDRESS_BORDER		= 4,

    D3DTADDRESS_MIRRORONCE	= 5,

    D3DTADDRESS_FORCE_DWORD	= 0x7fffffff, // force 32-bit size enum 

} D3DTEXTUREADDRESS;

typedef enum _D3DTEXTUREFILTERTYPE

{

    D3DTEXF_NONE	    = 0,    // filtering disabled (valid for mip filter only)

    D3DTEXF_POINT	    = 1,    // nearest

    D3DTEXF_LINEAR	    = 2,    // linear interpolation

    D3DTEXF_ANISOTROPIC     = 3,    // anisotropic

    D3DTEXF_PYRAMIDALQUAD   = 6,    // 4-sample tent

    D3DTEXF_GAUSSIANQUAD    = 7,    // 4-sample gaussian

    D3DTEXF_FORCE_DWORD     = 0x7fffffff,   // force 32-bit size enum

} D3DTEXTUREFILTERTYPE;

*/

#if 1

    bool load2D(const char* infoEndOrPath,

		unsigned int clampS    = 0,//D3DTADDRESS_WRAP,

		unsigned int clampT    = 0,//D3DTADDRESS_WRAP,

		unsigned int magFilter = 0,//D3DTEXF_LINEAR,

		unsigned int minFilter = 0,//D3DTEXF_LINEAR,

		bool   mipmap	 = true);



    bool load2DImage3(const Image3 &image,

		     unsigned int clampS    = 0,//D3DTADDRESS_WRAP,

		     unsigned int clampT    = 0,//D3DTADDRESS_WRAP,

		     unsigned int magFilter = 0,//D3DTEXF_LINEAR,

		     unsigned int minFilter = 0,//D3DTEXF_LINEAR,

		     bool  mipmap    = true);



    bool loadCube(const char* infoEndOrPath,

		  unsigned int clampS	 = 0,//D3DTADDRESS_CLAMP,

		  unsigned int clampT	 = 0,//D3DTADDRESS_CLAMP,

		  unsigned int magFilter = 0,//D3DTEXF_LINEAR,

		  unsigned int minFilter = 0,//D3DTEXF_LINEAR,

		  bool	mipmap	  = true);



    bool create1DShell(const char* name,

		       unsigned int width7     = 128,

		       FORMAT inFormat  = FORMAT_RGBA8,

		       FORMAT format    = FORMAT_RGBA8, 

		       unsigned int clampS    = 0,//D3DTADDRESS_WRAP, 

		       unsigned int clampT    = 0,//D3DTADDRESS_WRAP, 

		       unsigned int magFilter = 0,//D3DTEXF_LINEAR,

		       unsigned int minFilter = 0//D3DTEXF_LINEAR
);



    bool create2DShell(const char* name,

		       unsigned int width7     = 128,

		       unsigned int height7    = 128, 

		       FORMAT inFormat  = FORMAT_RGBA8,

		       FORMAT format    = FORMAT_RGBA8, 

		       unsigned int clampS    = 0,//D3DTADDRESS_WRAP, 

		       unsigned int clampT    = 0,//D3DTADDRESS_WRAP, 

		       unsigned int magFilter = 0,//D3DTEXF_LINEAR,

		       unsigned int minFilter = 0//D3DTEXF_LINEAR
);



    bool createRectShell(const char* name,

			 unsigned int width7	 = 128,

			 unsigned int height7	 = 128, 

			 FORMAT inFormat  = FORMAT_RGBA8,

			 FORMAT format    = FORMAT_RGBA8,  

			 unsigned int clampS	= 0,//D3DTADDRESS_CLAMP, 

			 unsigned int clampT	= 0,//D3DTADDRESS_CLAMP, 

			 unsigned int magFilter = 0,//D3DTEXF_LINEAR,

			 unsigned int minFilter = 0//D3DTEXF_LINEAR
);



    bool create3DShell(const char* name,

		       unsigned int width7     = 64,

		       unsigned int height7    = 64, 

		       unsigned int depth     = 64,

		       FORMAT inFormat  = FORMAT_RGBA8,

		       FORMAT format	  = FORMAT_RGBA8, 

		       unsigned int clampS    = 0,//D3DTADDRESS_WRAP, 

		       unsigned int clampT    = 0,//D3DTADDRESS_WRAP, 

		       unsigned int clampR    = 0,//D3DTADDRESS_WRAP, 

		       unsigned int magFilter = 0,//D3DTEXF_LINEAR,

		       unsigned int minFilter = 0//D3DTEXF_LINEAR
);



    bool createCubeShell(const char* name,

			 unsigned int size	= 128,

			 FORMAT inFormat  = FORMAT_RGBA8,

			 FORMAT format    = FORMAT_RGBA8, 

			 unsigned int clampS	= 0,//D3DTADDRESS_CLAMP, 

			 unsigned int clampT	= 0,//D3DTADDRESS_CLAMP, 

			 unsigned int magFilter = 0,//D3DTEXF_LINEAR,

			 unsigned int minFilter = 0//D3DTEXF_LINEAR
);



    bool createNoise3D(const  char* name,

		       unsigned int size = 128);



    bool createNormalizingCube(const char* name,

			       unsigned int size	 = 128, 

			       bool   buildMipmaps = true);

#endif

    void copyCurrentBuffer(TextureID newTarget = -1);

/*

    void setTextureCoordinatesGenerator(TextureCoordGenerator &texGen);

    TextureCoordGenerator &getTextureCoordinatesGenerator();

*/

    void	 setTransform(const Transform3D &);

    Transform3D &getTransform();



    //void doTransform();

    //void undoTransform();



    const bool activate(int unit = -1);

    const bool deactivate();



    //void associateWithPBuffer(GLPBuffer *buffer);



    void   setID(TextureID textureID);

    const  TextureID  getID() const;

   // const  LPDIRECT3DSURFACE9  getSurf() const;

    void   setTarget(TextureType target);



    const unsigned int	getTarget() const;

    const unsigned int	getHeight() const;

    const unsigned int	getWidth()  const;

    const unsigned int	getDepth()  const;



    //void    setAnisoLevel(int level);

    //unsigned int  getAnisoLevel();

    void    destroy();



  protected:

    unsigned int		 height7,

			   width7,

			   depth;

  private:



    //TextureCoordGenerator  textureCoordGenerator;

    Transform3D 	   transform;

//    GLPBuffer 	    *pBuffer;

  

    TextureType 	   target;

	//TextureID target;

    unsigned int  aniso;


    //LPDIRECT3DSURFACE9	surf;

    int 		   currUnit;



    bool loadTextureFace(const Image3 &image, TextureType target, bool	mipmap);



    bool finalizeLoading(const char* string);

    bool checkForRepeat (const char* string);

 

    int getXMLMagFilter(const NSString &value);

    int getXMLMinFilter(const NSString &value);

    int getXMLWrapMode (const NSString &value);

    int getXMLType     (const NSString &value);



    int getValidWrapMode (int clampNS);

    int getValidMagFilter(int filter);

    int getValidMinFilter(int filter);



    unsigned int m_clampS;

	unsigned int m_clampT;

    unsigned int m_magFilter;

	unsigned int m_minFilter;

    bool  m_mipmap;

};

#endif

