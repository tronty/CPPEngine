#include <Framework3/IRenderer.h>
#include "Clipmaps.h"
#include "JPEG_Preprocessor.h"
#include "JPEG_Decoder.h"
#include <string>

JPEG_Manager    g_JPEG_Manager;
stx_Effect*    g_pEffectJPEG = 0;

////////////////////////////////////////////////////////////////////////////////////
// JPEG_Data class
////////////////////////////////////////////////////////////////////////////////////

JPEG_Data::JPEG_Data()
{
    fileHandle = 0;
    mappingHandle = 0;
}

int JPEG_Data::LoadFile( char* fileName )
{
#if 0 // ???
    Pjpeg_decoder_file_stream Pinput_stream = new jpeg_decoder_file_stream();

    if( !Pinput_stream->open(fileName) )
    {
        return 1;
    }
    
    Pjpeg_decoder Pd = new jpeg_decoder(Pinput_stream);
    Pd->begin();

    std::wstring temporaryName = std::wstring(fileName) + "_UNCOMPRESSED";
    
    fileHandle = CreateFile( temporaryName.c_str(),
                            GENERIC_ALL,
                            0,
                            0,
                            CREATE_ALWAYS,
                            FILE_FLAG_RANDOM_ACCESS | FILE_FLAG_DELETE_ON_CLOSE,
                            0 );
                            
    if( !fileHandle )
    {
        return 1;
    }
    
    Pd->dump_DCT_data( fileHandle );
    
    mappingHandle = CreateFileMapping(
                    fileHandle,
                    0,
                    PAGE_READONLY,
                    0,
                    0,
                    0 );

    pFileData = (char*)MapViewOfFile(
                    mappingHandle,
                    FILE_MAP_READ,
                    0,
                    0,
                    0 );
    
    imageWidth = Pd->get_width();
    imageHeight = Pd->get_height();
    
    componentsNum = Pd->getScanComponentsNum();
    
    static float scaleFactor[8] = { 1.0f, 1.387039845f, 1.306562965f, 1.175875602f,
                                    1.0f, 0.785694958f, 0.541196100f, 0.275899379f };
    
    for( int i = 0; i < componentsNum; i++ )
    {
        QUANT_TYPE *pTable = Pd->getQuantizationTable( i );

        for( int row = 0; row < 8; row++ )
        {
            for( int col = 0; col < 8; col++ )
            {
                quantTables[i][row * 8 + col] = BYTE(pTable[row * 8 + col] * scaleFactor[row] * scaleFactor[col]);
            }
        } 
    
        MCU_per_row = Pd->get_mcus_per_row();
    }
        
    MCU_blocks_num = Pd->get_blocks_per_mcu();
    
    delete Pd;
    delete Pinput_stream;
#endif   
    return 0;
}


int JPEG_Data::AllocateTextures( int stackSize, int border )
{
    int hr;
    
    if( componentsNum == 1 )
    {
        TextureData[0].Intitialize( stackSize, border, quantTables[0] );
            
        return 0;
    }
    else if( componentsNum == 3 )
    {   
        TextureData[0].Intitialize( stackSize, border, quantTables[0] );
        TextureData[1].Intitialize( stackSize / 2, border / 2, quantTables[1] );
        TextureData[2].Intitialize( stackSize / 2, border / 2, quantTables[2] );
        
        return 0;
    }
    
    return 1;
}


// Prepare texture data for IDCT pass
int JPEG_Data::UpdateTextureData( int blocksNum, int blockSize, int *pSrcCorners )
{
    int MCU_SIZE = 8;
    int MCU_BLOCK_SIZE = 64;
    
    int MCU_ROW;
    int MCU_COLUMN;

    if( componentsNum == 3 )
    {
        MCU_SIZE = 16;
        MCU_BLOCK_SIZE = 384;
    }
        
    int MCU_NUM_X = blockSize / MCU_SIZE;
    int MCU_NUM_Y = blockSize / MCU_SIZE;
    
    int updatesNum = 0;
    
    if( componentsNum == 1 )
    {
        D3DFROMWINE_BOX box;

        box.front = 0;
        box.back = 1;
                
        for( int i = 0; i < blocksNum; i++ )
        {
            MCU_ROW = pSrcCorners[ i * 2 + 1 ] / MCU_SIZE;
            MCU_COLUMN = pSrcCorners[ i * 2 ] / MCU_SIZE;
            
            box.top = 0;
            box.bottom = 8;
            
            for( int j = MCU_ROW; j < MCU_ROW + MCU_NUM_Y; j++ )
            {
                box.left = ( i * MCU_NUM_X ) * 8;
                box.right = box.left + 8;
                
                for( int k = MCU_COLUMN; k < MCU_COLUMN + MCU_NUM_X; k++ )
                {
                    int offset = MCU_BLOCK_SIZE * ( j * MCU_per_row  + k );
                    IRenderer::GetRendererInstance()->UpdateSubresource( TextureData[0].pTextureDCT, 0, &box, pFileData + offset, 8, 0 );
                    
                    box.left += 8;
                    box.right += 8;
                }
                
                box.top += 8;
                box.bottom += 8;
            }
        }   
    }
    else if( componentsNum == 3 )
    {
        D3DFROMWINE_BOX box[2];

        box[0].front = box[1].front = 0;
        box[0].back = box[1].back = 1;
        
        int corner[2];
        
        for( int i = 0; i < blocksNum; i++ )
        {
            MCU_ROW = pSrcCorners[ i * 2 + 1 ] / MCU_SIZE;
            MCU_COLUMN = pSrcCorners[ i * 2 ] / MCU_SIZE;
            
            box[1].top = 0;
            box[1].bottom = 8;
            
            corner[1] = 0;
            
            for( int j = MCU_ROW; j < MCU_ROW + MCU_NUM_Y; j++ )
            {
                corner[0] = i * MCU_NUM_X * 16;
                
                box[1].left = i * MCU_NUM_X * 8;
                box[1].right = box[1].left + 8;
                                
                for( int k = MCU_COLUMN; k < MCU_COLUMN + MCU_NUM_X; k++ )
                {
                    int offset = MCU_BLOCK_SIZE * ( j * MCU_per_row + k );
                    
                    box[0].top = corner[1];
                    box[0].bottom = corner[1] + 8;
                    
                    box[0].left = corner[0];
                    box[0].right = corner[0] + 8;
                    
                    IRenderer::GetRendererInstance()->UpdateSubresource( TextureData[0].pTextureDCT, 0, &box[0], pFileData + offset +   0, 8, 0 );
                    
                    box[0].left += 8;
                    box[0].right += 8;
                   
                    IRenderer::GetRendererInstance()->UpdateSubresource( TextureData[0].pTextureDCT, 0, &box[0], pFileData + offset +  64, 8, 0 );
                    
                    box[0].top += 8;
                    box[0].bottom += 8;
                    
                    box[0].left = corner[0];
                    box[0].right = corner[0] + 8;
                    
                    IRenderer::GetRendererInstance()->UpdateSubresource( TextureData[0].pTextureDCT, 0, &box[0], pFileData + offset + 128, 8, 0 );
                    
                    box[0].left += 8;
                    box[0].right += 8;
                    
                    IRenderer::GetRendererInstance()->UpdateSubresource( TextureData[0].pTextureDCT, 0, &box[0], pFileData + offset + 192, 8, 0 );
                    
                    IRenderer::GetRendererInstance()->UpdateSubresource( TextureData[1].pTextureDCT, 0, &box[1], pFileData + offset + 256, 8, 0 );
                    IRenderer::GetRendererInstance()->UpdateSubresource( TextureData[2].pTextureDCT, 0, &box[1], pFileData + offset + 320, 8, 0 );
                    updatesNum++;
                    
                    box[1].left += 8;
                    box[1].right += 8;
                    
                    corner[0] += 16;
                }
                
                box[1].top += 8;
                box[1].bottom += 8;
                
                corner[1] += 16;
            }
        }   
    }
    
    return 0;
}


// Perform IDCT on a single texture chanel 
void JPEG_Data::UncompressTextureData()
{
    D3DFROMWINEVIEWPORT9 customViewports[2];
    
    customViewports[0].TopLeftX = 0;
    customViewports[0].TopLeftY = 0;
    customViewports[0].MinDepth = 0.0f;
    customViewports[0].MaxDepth = 1.0f;
    
    TextureID Views[2];
    
    stx_Effect::stx_Technique *technique = g_pEffectJPEG->GetTechniqueByName( "JPEG_Decompression" );
    
    static exBufferID pBuffers[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    static unsigned pStrides[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    static unsigned pOffsets[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        
    IRenderer::GetRendererInstance()->setVertexFormat(0,  0 );
    IRenderer::GetRendererInstance()->setIndexBuffer( 0, FORMAT_R32_UINT, 0 );
    IRenderer::GetRendererInstance()->setVertexBuffer(0, D3D10_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT, pBuffers, pStrides, pOffsets );
    IRenderer::GetRendererInstance()->IASetPrimitiveTopology( PRIM_POINTS );
    
    SURFACE_DESC desc;
    
    stx_Effect::stx_Variable *pRowScale = g_pEffectJPEG->GetVariableByName( "g_RowScale" )->AsScalar();
    stx_Effect::stx_Variable *pColScale = g_pEffectJPEG->GetVariableByName( "g_ColScale" )->AsScalar();
    
    TextureID ppSRV[2];
    
    for( int i = 0; i < componentsNum; i++ )
    {
        TextureData[i].pTextureTarget->GetDesc( &desc );
        pRowScale->SetFloat( desc.Width / 8.0f );
        pColScale->SetFloat( desc.Height / 8.0f );
                
        /////////////////////////////////////////////////////////////////////////////
        // Pass1
        /////////////////////////////////////////////////////////////////////////////
        
        technique->GetPassByIndex( 0 )->Apply( 0 );
        
        Views[0] = TextureData[i].pTexture1RowRTV;
        Views[1] = TextureData[i].pTexture2RowRTV;
        
        TextureData[i].pTexture1Row->GetDesc( &desc );
        customViewports[0].Width = desc.Width;
        customViewports[0].Height = desc.Height;
        customViewports[1] = customViewports[0];
        
        IRenderer::GetRendererInstance()->RSSetViewports( 2, customViewports );
        IRenderer::GetRendererInstance()->OMSetRenderTargets( 2, Views, 0 );

        SURFACE_DESC desc;
        TextureData[i].pTextureQ->GetDesc( &desc );
                
        ppSRV[0] =  TextureData[i].pTextureDCTSRV;
        ppSRV[1] =  TextureData[i].pTextureQSRV;
        IRenderer::GetRendererInstance()->PSSetShaderResources( 0, 2, ppSRV );
        
        IRenderer::GetRendererInstance()->Draw( 1, 0 );
        
        /////////////////////////////////////////////////////////////////////////////
        // Pass2
        /////////////////////////////////////////////////////////////////////////////
        
        technique->GetPassByIndex( 1 )->Apply( 0 );
        
        Views[0] = TextureData[i].pTextureTargetRTV;
        Views[1] = 0;
        
        TextureData[i].pTextureTarget->GetDesc( &desc );
        customViewports[0].Width = desc.Width;
        customViewports[0].Height = desc.Height;
        
        IRenderer::GetRendererInstance()->RSSetViewports( 1, customViewports );
        IRenderer::GetRendererInstance()->OMSetRenderTargets( 2, Views, 0 );
                
        ppSRV[0] =  TextureData[i].pTexture1RowSRV;
        ppSRV[1] =  TextureData[i].pTexture2RowSRV;
        IRenderer::GetRendererInstance()->PSSetShaderResources( 0, 2, ppSRV );
        
        IRenderer::GetRendererInstance()->Draw( 1, 0 );
        
        /////////////////////////////////////////////////////////////////////////////
        // Pass3
        /////////////////////////////////////////////////////////////////////////////
        
        technique->GetPassByIndex( 2 )->Apply( 0 );
        
        Views[0] = TextureData[i].pTexture1ColRTV;
        Views[1] = TextureData[i].pTexture2ColRTV;
        
        TextureData[i].pTexture1Col->GetDesc( &desc );
        customViewports[0].Width = desc.Width;
        customViewports[0].Height = desc.Height;
        customViewports[1] = customViewports[0];
                
        IRenderer::GetRendererInstance()->RSSetViewports( 2, customViewports );
        IRenderer::GetRendererInstance()->OMSetRenderTargets( 2, Views, 0 );
                
        ppSRV[0] =  TextureData[i].pTextureTargetSRV;
        ppSRV[1] =  0;
        IRenderer::GetRendererInstance()->PSSetShaderResources( 0, 2, ppSRV );
        
        IRenderer::GetRendererInstance()->Draw( 1, 0 );
         
        /////////////////////////////////////////////////////////////////////////////
        // Pass4
        /////////////////////////////////////////////////////////////////////////////

        technique->GetPassByIndex( 3 )->Apply( 0 );
        
        Views[0] = TextureData[i].pTextureTargetRTV;
        Views[1] = 0;
        
        TextureData[i].pTextureTarget->GetDesc( &desc );
        customViewports[0].Width = desc.Width;
        customViewports[0].Height = desc.Height;

        IRenderer::GetRendererInstance()->RSSetViewports( 1, customViewports );
        IRenderer::GetRendererInstance()->OMSetRenderTargets( 2, Views, 0 );
        
        ppSRV[0] =  TextureData[i].pTexture1ColSRV;
        ppSRV[1] =  TextureData[i].pTexture2ColSRV;
        IRenderer::GetRendererInstance()->PSSetShaderResources( 0, 2, ppSRV );
                
        IRenderer::GetRendererInstance()->Draw( 1, 0 );
    }
}


void JPEG_Data::Release()
{
    if( mappingHandle )
    {
        CloseHandle( mappingHandle );
        mappingHandle = 0;
    }
    
    if( fileHandle )
    {
        CloseHandle( fileHandle );
        fileHandle = 0;
    }
    
    pFileData = 0;
    
    for( int i =0; i < componentsNum; i++ )
    {
        TextureData[i].Release();
    }
}


////////////////////////////////////////////////////////////////////////////////////
// UpdateInfo class
////////////////////////////////////////////////////////////////////////////////////
UpdateInfo::UpdateInfo()
{
    blocksNum = 0;
    curBlock = 0;
    blocksAdded = 0;
    pSrcCorners = 0;
    pDstCorners = 0;
}

UpdateInfo::~UpdateInfo()
{
    Release();
}

void UpdateInfo::Initialize( int blocks )
{
    Release();
    
    blocksNum = blocks;
    
    pSrcCorners = new int[ blocksNum * 2 ];
    pDstCorners = new int[ blocksNum * 2 ];
    
    Reset();
}

void UpdateInfo::Release()
{
    if( pSrcCorners )
    {
        delete [] pSrcCorners;
        pSrcCorners = 0;   
    }
    
    if( pDstCorners )
    {
        delete [] pDstCorners;
        pDstCorners = 0;   
    }
    
    blocksNum = 0;
    curBlock = 0;
    blocksAdded = 0;
}

void UpdateInfo::Reset()
{
    curBlock = 0;
    blocksAdded = 0;
}

void UpdateInfo::AddBlock( int srcBlock[2], int dstBlock[2] )
{
    pSrcCorners[ 2 * curBlock ] = srcBlock[0];
    pSrcCorners[ 2 * curBlock + 1 ] = srcBlock[1];
    
    pDstCorners[ 2 * curBlock ] = dstBlock[0];
    pDstCorners[ 2 * curBlock + 1 ] = dstBlock[1];
    
    blocksAdded++;
    curBlock++;
    
    curBlock %= blocksNum;
}


////////////////////////////////////////////////////////////////////////////////////
// JPEG_Manager class
////////////////////////////////////////////////////////////////////////////////////
JPEG_Manager::JPEG_Manager()
{
    isInitialized = false;
    levelsNum = 0;
    pJpegData = 0;
    pUpdateBlocks = 0;
    ppBakedTextures = 0;
}
    
JPEG_Manager::~JPEG_Manager()
{
    Release();
}

int JPEG_Manager::Intitialize( int levels, char fileNames[][MAX_PATH], char fileNamesHM[][MAX_PATH] )
{   
    int hr = 0;
    
    if( !isInitialized )
    {
        if( !levels )
            return 1;
            
        levelsNum = levels;
        
        pJpegData = new JPEG_Data[levelsNum];
        pUpdateBlocks = new UpdateInfo[levelsNum];
        
        if( fileNamesHM )
        {
            pJpegDataHM = new JPEG_Data[levelsNum];
        }
        
        for( int i = 0; i < levelsNum; i++ )
        {
            ( pJpegData[i].LoadFile( fileNames[i] ) )
            
            if( fileNamesHM )
                ( pJpegDataHM[i].LoadFile( fileNamesHM[i] ) )
        }
        
        isInitialized = true;
    }
    
    return hr;
}


void JPEG_Manager::AllocateBlocks( int blocksNum )
{
    for( int i = 0; i < levelsNum; i++ )
    {
        pUpdateBlocks[i].Initialize( int( blocksNum * pow( 2.0, i ) ) );
    }    
}


int JPEG_Manager::AllocateTextures( int stackSize, int borderSize )
{
    int hr;
    int border = borderSize;
    
    LoadEffect( "JPEG_Preprocessor.fx", &g_pEffectJPEG );
    
    textureSize = stackSize;
    blockSize = borderSize;
    
    ppBakedTextures = new TextureID [levelsNum];
    ppBakedTexturesRTV = new TextureID [levelsNum];
    
    SURFACE_DESC texDesc;
    ZeroMemory( &texDesc, sizeof( texDesc ) );
    texDesc.ArraySize = 1;
    texDesc.Usage = USAGE_DEFAULT;
    texDesc.BindFlags = BIND_RENDER_TARGET;
    texDesc.Format = FORMAT_RGBA8_UNORM;
    texDesc.MipLevels = 1;
    texDesc.SampleDesc.Count = 1;
    
    SURFACE_DESC RTVDesc;
    ZeroMemory( &RTVDesc, sizeof( RTVDesc ) );
    RTVDesc.Format = FORMAT_RGBA8_UNORM;
    RTVDesc.ViewDimension = RTV_DIMENSION_TEXTURE2D;
    RTVDesc.Texture2D.MipSlice = 0;
    
    for( int i = 0; i < levelsNum; i++ )
    {
        ( ( pJpegData[i].AllocateTextures( stackSize, border ) ) )
        ( ( pJpegDataHM[i].AllocateTextures( stackSize, border ) ) )
        
        {Image3 img;
	img.create(FORMAT_RGBA8, stackSize, border, 1, 1);
	ppBakedTextures[i]=IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear());}

        border >>= 1;
    }
    
    return 0;
}


void JPEG_Manager::Release()
{
    //SAFE_RELEASE( g_pEffectJPEG );
    
    if( pJpegData )
    {
        for( int i = 0; i < levelsNum; i++ )
            pJpegData[i].Release();
            
        delete [] pJpegData;
        pJpegData = 0;
    }
    
    if( pUpdateBlocks )
    {
        for( int i = 0; i < levelsNum; i++ )
            pUpdateBlocks[i].Release();
            
        delete [] pUpdateBlocks;
        pUpdateBlocks = 0;
    }
    
    if( pJpegDataHM )
    {
        for( int i = 0; i < levelsNum; i++ )
            pJpegDataHM[i].Release();
            
        delete [] pJpegData;
        pJpegDataHM = 0;
    }
    
    if( ppBakedTextures )
    {
        for( int i = 0; i < levelsNum; i++ )
        {
            //SAFE_RELEASE( ppBakedTextures[i] );
            //SAFE_RELEASE( ppBakedTexturesRTV[i] );
        }
        
        delete [] ppBakedTextures;
        ppBakedTextures = 0;
        
        delete [] ppBakedTexturesRTV;
        ppBakedTexturesRTV = 0;
    }
    
    isInitialized = false;
    levelsNum = 0;
}


void JPEG_Manager::AddBlock( int level, int srcBlock[2], int dstBlock[2] )
{
    if( level < levelsNum )
    {
        pUpdateBlocks[level].AddBlock( srcBlock, dstBlock );
    }
}


// Update one level of a clipmap stack texture with a new data covered by update border size
// We update a single ( StackSize * ( UpdateBorderSize >> level ) ) block at once
void JPEG_Manager::Update( int level )
{
    TextureID cashedRTV;
    TextureID cashedDSV;
    D3DFROMWINEVIEWPORT9 cashedViewport;
    unsigned int numViewports = 1;
        
    // Read current states to restore them after image block processing 
    IRenderer::GetRendererInstance()->OMGetRenderTargets( 1, &cashedRTV, &cashedDSV );
    IRenderer::GetRendererInstance()->RSGetViewports( &numViewports, &cashedViewport );
	
	if( level < levelsNum )
    {
        int blocksNum = pUpdateBlocks[level].GetNewBlocksNumber();
        
        // Process diffuse texture
        pJpegData[level].UpdateTextureData( blocksNum, blockSize >> level, pUpdateBlocks[level].GetSrcBlocksPointer() );
        pJpegData[level].UncompressTextureData();
                
        // Process heightmap texture
        // Here we use update blocks from the diffuse texture because they should be the same
        pJpegDataHM[level].UpdateTextureData( blocksNum, blockSize >> level, pUpdateBlocks[level].GetSrcBlocksPointer() );
        pJpegDataHM[level].UncompressTextureData();
        pUpdateBlocks[level].Reset();
                
        D3DFROMWINEVIEWPORT9 customViewport;
    
        customViewport.TopLeftX = 0;
        customViewport.TopLeftY = 0;
        customViewport.MinDepth = 0.0f;
        customViewport.MaxDepth = 1.0f;
                
        SURFACE_DESC desc;
        ppBakedTextures[level]->GetDesc( &desc );
        
        customViewport.Width = desc.Width;
        customViewport.Height = desc.Height;
        
        stx_Effect::stx_Technique *technique = g_pEffectJPEG->GetTechniqueByName( "JPEG_Decompression" );
        
        IRenderer::GetRendererInstance()->changeRenderTarget(&ppBakedTexturesRTV[level], 0 );
        IRenderer::GetRendererInstance()->RSSetViewports( 1, &customViewport );
        technique->GetPassByIndex( 4 )->Apply( 0 );

        TextureID ppSRV[4];
        
        ppSRV[0] = pJpegData[level].GetFinalTarget( 0 );
        ppSRV[1] = pJpegData[level].GetFinalTarget( 1 );
        ppSRV[2] = pJpegData[level].GetFinalTarget( 2 );
        
        if( pJpegDataHM )
            ppSRV[3] = pJpegDataHM[level].GetFinalTarget( 0 );
        else
            ppSRV[3] = 0; 
        
        IRenderer::GetRendererInstance()->PSSetShaderResources( 0, 4, ppSRV );
        // Combine decoded components into a single R8G8B8A8 texture
        IRenderer::GetRendererInstance()->Draw( 1, 0 );

        int *pDstBlocks = pUpdateBlocks[level].GetDstBlocksPointer();
        int levelBlockSize = blockSize >> level;
        
        D3DFROMWINE_BOX srcBox;
		srcBox.front = 0;
        srcBox.back = 1;
        srcBox.top = 0;
        srcBox.bottom = levelBlockSize;
        srcBox.left = 0;
        srcBox.right = levelBlockSize;

        // Copy fully unpacked data to the clipmap stack texture
        for( int i = 0; i < blocksNum; i++ )
        {
            IRenderer::GetRendererInstance()->CopySubresourceRegion( g_pStackTexture, level, pDstBlocks[i * 2], pDstBlocks[i * 2 + 1], 0, ppBakedTextures[level], 0, &srcBox );
            
            srcBox.left += levelBlockSize;
            srcBox.right += levelBlockSize;
        }
    }

	TextureID ppRTV[2];
    ppRTV[0] = cashedRTV;
    ppRTV[1] = 0;
        
    IRenderer::GetRendererInstance()->changeRenderTarget(ppRTV, cashedDSV );
    IRenderer::GetRendererInstance()->RSSetViewports( 1, &cashedViewport );

	//SAFE_RELEASE( cashedRTV );
    //SAFE_RELEASE( cashedDSV );
}


////////////////////////////////////////////////////////////////////////////////////
// JPEG_GPU class
////////////////////////////////////////////////////////////////////////////////////

JPEG_GPU::JPEG_GPU()
{
    pTextureDCT = 0;
    pTextureDCTSRV = 0;
    pTextureQ = 0;
    pTextureQSRV = 0;
    pTexture1Row = 0;
    pTexture1RowSRV = 0;
    pTexture1RowRTV = 0;
    pTexture2Row = 0;
    pTexture2RowSRV = 0;
    pTexture2RowRTV = 0;
    pTexture1Col = 0;
    pTexture1ColSRV = 0;
    pTexture1ColRTV = 0;
    pTexture2Col = 0;
    pTexture2ColSRV = 0;
    pTexture2ColRTV = 0;
    pTextureTarget = 0;
    pTextureTargetSRV = 0;
    pTextureTargetRTV = 0;
}


int JPEG_GPU::Intitialize( int width, int height, BYTE *pQuantTable )
{
    int hr; 
	{Image3 img;
	img.create(FORMAT_R8, width, height, 1, 1);
	pTextureDCT=IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear());}
	{Image3 img;
	img.create(FORMAT_R8, 8, 8, 1, 1);
	pTextureQ=IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear());}
	{Image3 img;
	img.create(FORMAT_RGBA16f, width / 8, height, 1, 1);
	pTexture1Row=IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear());}
	{Image3 img;
	img.create(FORMAT_RGBA16f, width / 8, height, 1, 1);
	pTexture2Row=IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear());}
	{Image3 img;
	img.create(FORMAT_RGBA16f, width, height / 8, 1, 1);
	pTexture1Col=IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear());}
	{Image3 img;
	img.create(FORMAT_RGBA16f, width, height / 8, 1, 1);
	pTexture2Col=IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear());}
	{Image3 img;
	img.create(FORMAT_R16f, width, height, 1, 1);
	pTextureTarget=IRenderer::GetRendererInstance()->addTexture(&img, false, IRenderer::GetRendererInstance()->Getlinear());}

    return 0;
}


void JPEG_GPU::Release()
{
    //SAFE_RELEASE( pTextureDCT );
    //SAFE_RELEASE( pTextureDCTSRV );
    //SAFE_RELEASE( pTextureQ );
    //SAFE_RELEASE( pTextureQSRV );
    //SAFE_RELEASE( pTexture1Row );
    //SAFE_RELEASE( pTexture1RowSRV );
    //SAFE_RELEASE( pTexture1RowRTV );
    //SAFE_RELEASE( pTexture2Row );
    //SAFE_RELEASE( pTexture2RowSRV );
    //SAFE_RELEASE( pTexture2RowRTV );
    //SAFE_RELEASE( pTexture1Col );
    //SAFE_RELEASE( pTexture1ColSRV );
    //SAFE_RELEASE( pTexture1ColRTV );
    //SAFE_RELEASE( pTexture2Col );
    //SAFE_RELEASE( pTexture2ColSRV );
    //SAFE_RELEASE( pTexture2ColRTV );
    //SAFE_RELEASE( pTextureTarget );
    //SAFE_RELEASE( pTextureTargetSRV );
    //SAFE_RELEASE( pTextureTargetRTV );
}
