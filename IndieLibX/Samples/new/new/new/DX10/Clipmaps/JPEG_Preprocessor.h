#include <Framework3/IRenderer.h>

#define MAX_CHANELS 3

class JPEG_GPU
{

public:
    
    TextureID                 pTextureDCT;
    TextureID        pTextureDCTSRV;
    
    TextureID                 pTextureQ;
    TextureID        pTextureQSRV;

    // Row textures
    TextureID                 pTexture1Row;
    TextureID        pTexture1RowSRV;
    TextureID          pTexture1RowRTV;
    
    TextureID                 pTexture2Row;
    TextureID        pTexture2RowSRV;
    TextureID          pTexture2RowRTV;

    // Column textures
    TextureID                 pTexture1Col;
    TextureID        pTexture1ColSRV;
    TextureID          pTexture1ColRTV;
    
    TextureID                 pTexture2Col;
    TextureID        pTexture2ColSRV;
    TextureID          pTexture2ColRTV;
    
    // Final texture
    TextureID                 pTextureTarget;
    TextureID        pTextureTargetSRV;
    TextureID          pTextureTargetRTV;
    
    JPEG_GPU();
    int Intitialize( int width, int height, BYTE *pQuantTable );
    void Release();
};


class JPEG_Data
{
    int     imageWidth;
    int     imageHeight;
    int     componentsNum;
    BYTE    quantTables[MAX_CHANELS][64];
    
    int     MCU_per_row;
    int     MCU_blocks_num;
    
    HANDLE  fileHandle;
    HANDLE  mappingHandle;
    char    *pFileData;
    
    // D3D10 Textures are stored here and are used for IDCT on GPU
    JPEG_GPU        TextureData[MAX_CHANELS];
    
public:
    
    JPEG_Data();
    int LoadFile( char* fileName );
    void    Release();
    int     GetImageWidth() { return imageWidth; }
    int     GetImageHeight() { return imageHeight; }
    int AllocateTextures( int stackSize, int border );
    int UpdateTextureData( int blocksNum, int blockSize, int *pSrcCorners );
    TextureID GetFinalTarget( int componentID ) { return TextureData[componentID].pTextureTargetSRV; }
    void    UncompressTextureData();
};


class UpdateInfo
{
    int     blocksNum;      // A total number of blocks reserved for a single update step.
                            // This should be at least StackSize / UpdateBorderSize
    int     curBlock;
    int     blocksAdded;
    int     *pSrcCorners;   // Values used to map data during UpdateSubresource calls
                            // when we prepare data for IDCT pass.
                            // Here we store only the positions of a left upper corner,
                            // because update region size depends on the resource being used.
    int     *pDstCorners;   // Values used to map data during CopySubresourceRegion calls
                            // when we update our clipmap stack texture with uncompressed data.
    
public:
    
    UpdateInfo();
    ~UpdateInfo();
    void    Initialize( int blocks );
    void    AddBlock( int DstBlock[2], int SrcBlock[2] );
    int     GetNewBlocksNumber() { return blocksAdded; }
    int*    GetSrcBlocksPointer() { return pSrcCorners; }
    int*    GetDstBlocksPointer() { return pDstCorners; }
    void    Reset();
    void    Release();
};


class JPEG_Manager
{
    bool        isInitialized;
    int         levelsNum;      // Preprocessed levels number usde for a clipmap stack update.
                                // This should be at least as much as a stack depth
    int         textureSize;    // The biggest size of 0 level.
    int         blockSize;      // Updatable region size for 0 level.
    JPEG_Data   *pJpegData;
    JPEG_Data   *pJpegDataHM;   // Used to store additional height maps values
    UpdateInfo  *pUpdateBlocks;
    TextureID  ppBakedTextures;  // This textures are used at a final pass and they hold
                                        // an uncomprssed data ready for a clipmap stack texture update
    TextureID  ppBakedTexturesRTV;
    
public:
    
    JPEG_Manager();
    ~JPEG_Manager();
    int     Intitialize( int levels, char fileNames[][MAX_PATH], char fileNamesHM[][MAX_PATH] );
    void        AddBlock( int level, int srcBlock[2], int dstBlock[2] );
    void        AllocateBlocks( int blocksNum );
    int     AllocateTextures( int stackSize, int borderSize );
    void        Update( int level );
    int         GetImageWidth() { return pJpegData[0].GetImageWidth(); }
    int         GetImageHeight() { return pJpegData[0].GetImageHeight(); }
    void        Release();
};

extern JPEG_Manager g_JPEG_Manager;
