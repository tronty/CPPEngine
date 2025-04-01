/*
  Copyright (c) 2020 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
#include <Framework3/RendererHLSLCg.h>
#include <Framework3/RendererHLSLD3D11.h>

struct SimpleVertex
{
    D3DXFROMWINEVECTOR3 Pos;
    D3DXFROMWINEVECTOR2 Tex;
};

struct CBNeverChanges
{
    D3DXFROMWINEMATRIX mView;
};

struct CBChangeOnResize
{
    D3DXFROMWINEMATRIX mProjection;
};

struct CBChangesEveryFrame
{
    D3DXFROMWINEMATRIX mWorld;
    D3DXFROMWINEVECTOR4 vMeshColor;
};

ID3D11RenderTargetView*             g_pRenderTargetView = NULL;
ID3D11RenderTargetView*             g_pHeightRenderTargetView = NULL;
ID3D11RenderTargetView*             g_pTerrainRenderTargetView = NULL;
ID3D11Texture2D*                    g_pDepthStencil = NULL;
ID3D11DepthStencilView*             g_pDepthStencilView = NULL;
ID3D11VertexShader*                 g_pVertexShader = NULL;
ID3D11PixelShader*                  g_pPixelShader = NULL;
ID3D11InputLayout*                  g_pVertexLayout = NULL;
ID3D11Buffer*                       g_pVertexBuffer = NULL;
ID3D11Buffer*                       g_pIndexBuffer = NULL;
ID3D11Buffer*                       g_pCBNeverChanges = NULL;
ID3D11Buffer*                       g_pCBChangeOnResize = NULL;
ID3D11Buffer*                       g_pCBChangesEveryFrame = NULL;
ID3D11ShaderResourceView*           g_pTextureRV = NULL;
ID3D11SamplerState*                 g_pSamplerLinear = NULL;
D3DXFROMWINEMATRIX                            g_World;
D3DXFROMWINEMATRIX                            g_View;
D3DXFROMWINEMATRIX                            g_Projection;
D3DXFROMWINEVECTOR4                            g_vMeshColor( 0.7f, 0.7f, 0.7f, 1.0f );

HRESULT CompileShaderFromFile( CHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut )
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DX11CompileFromFile( szFileName, NULL, NULL, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL );
    if( FAILED(hr) )
    {
        if( pErrorBlob != NULL )
            OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() );
        if( pErrorBlob ) pErrorBlob->Release();
        return hr;
    }
    if( pErrorBlob ) pErrorBlob->Release();

    return S_OK;
}

HRESULT CompileShaders(char* fn="../../InfiniteTerrainII_DX11/Tutorial07.fx")
{
    HRESULT hr = S_OK;

    ID3DBlob* pVSBlob = NULL;
    hr = CompileShaderFromFile( fn, "VS", "vs_4_0", &pVSBlob );
    if( FAILED( hr ) )
    {
        MessageBox( NULL,
                    "The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", "Error", MB_OK );
        return hr;
    }

    hr = g_pd3dDevice->CreateVertexShader( pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &g_pVertexShader );
    if( FAILED( hr ) )
    {    
        pVSBlob->Release();
        return hr;
    }

    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = ARRAYSIZE( layout );

    hr = g_pd3dDevice->CreateInputLayout( layout, numElements, pVSBlob->GetBufferPointer(),
                                          pVSBlob->GetBufferSize(), &g_pVertexLayout );
    pVSBlob->Release();
    if( FAILED( hr ) )
        return hr;

    g_pImmediateContext->IASetInputLayout( g_pVertexLayout );

    ID3DBlob* pPSBlob = NULL;
    hr = CompileShaderFromFile( fn, "PS", "ps_4_0", &pPSBlob );
    if( FAILED( hr ) )
    {
        MessageBox( NULL,
                    "The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", "Error", MB_OK );
        return hr;
    }

    hr = g_pd3dDevice->CreatePixelShader( pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &g_pPixelShader );
    pPSBlob->Release();
    if( FAILED( hr ) )
        return hr;
    return hr;
}

HRESULT InitDevice()
{
    HRESULT hr = S_OK;

    ID3D11Texture2D* pBackBuffer = NULL;
    hr = g_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );
    if( FAILED( hr ) )
        return hr;

    hr = g_pd3dDevice->CreateRenderTargetView( pBackBuffer, NULL, &g_pRenderTargetView );
    pBackBuffer->Release();
    if( FAILED( hr ) )
        return hr;

    hr = g_pd3dDevice->CreateRenderTargetView( pBackBuffer, NULL, &g_pHeightRenderTargetView );
    pBackBuffer->Release();
    if( FAILED( hr ) )
        return hr;

    hr = g_pd3dDevice->CreateRenderTargetView( pBackBuffer, NULL, &g_pTerrainRenderTargetView );
    pBackBuffer->Release();
    if( FAILED( hr ) )
        return hr;

    D3D11_TEXTURE2D_DESC descDepth;
    ZeroMemory( &descDepth, sizeof(descDepth) );
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    hr = g_pd3dDevice->CreateTexture2D( &descDepth, NULL, &g_pDepthStencil );
    if( FAILED( hr ) )
        return hr;

    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory( &descDSV, sizeof(descDSV) );
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = g_pd3dDevice->CreateDepthStencilView( g_pDepthStencil, &descDSV, &g_pDepthStencilView );
    if( FAILED( hr ) )
        return hr;

    g_pImmediateContext->OMSetRenderTargets( 1, &g_pRenderTargetView, g_pDepthStencilView );

    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    g_pImmediateContext->RSSetViewports( 1, &vp );
    
    CompileShaders("../../InfiniteTerrainII_DX11/Tutorial07.fx");

    SimpleVertex vertices[] =
    {
        { D3DXFROMWINEVECTOR3( -1.0f, 1.0f, -1.0f ), D3DXFROMWINEVECTOR2( 0.0f, 0.0f ) },
        { D3DXFROMWINEVECTOR3( 1.0f, 1.0f, -1.0f ), D3DXFROMWINEVECTOR2( 1.0f, 0.0f ) },
        { D3DXFROMWINEVECTOR3( 1.0f, 1.0f, 1.0f ), D3DXFROMWINEVECTOR2( 1.0f, 1.0f ) },
        { D3DXFROMWINEVECTOR3( -1.0f, 1.0f, 1.0f ), D3DXFROMWINEVECTOR2( 0.0f, 1.0f ) },

        { D3DXFROMWINEVECTOR3( -1.0f, -1.0f, -1.0f ), D3DXFROMWINEVECTOR2( 0.0f, 0.0f ) },
        { D3DXFROMWINEVECTOR3( 1.0f, -1.0f, -1.0f ), D3DXFROMWINEVECTOR2( 1.0f, 0.0f ) },
        { D3DXFROMWINEVECTOR3( 1.0f, -1.0f, 1.0f ), D3DXFROMWINEVECTOR2( 1.0f, 1.0f ) },
        { D3DXFROMWINEVECTOR3( -1.0f, -1.0f, 1.0f ), D3DXFROMWINEVECTOR2( 0.0f, 1.0f ) },

        { D3DXFROMWINEVECTOR3( -1.0f, -1.0f, 1.0f ), D3DXFROMWINEVECTOR2( 0.0f, 0.0f ) },
        { D3DXFROMWINEVECTOR3( -1.0f, -1.0f, -1.0f ), D3DXFROMWINEVECTOR2( 1.0f, 0.0f ) },
        { D3DXFROMWINEVECTOR3( -1.0f, 1.0f, -1.0f ), D3DXFROMWINEVECTOR2( 1.0f, 1.0f ) },
        { D3DXFROMWINEVECTOR3( -1.0f, 1.0f, 1.0f ), D3DXFROMWINEVECTOR2( 0.0f, 1.0f ) },

        { D3DXFROMWINEVECTOR3( 1.0f, -1.0f, 1.0f ), D3DXFROMWINEVECTOR2( 0.0f, 0.0f ) },
        { D3DXFROMWINEVECTOR3( 1.0f, -1.0f, -1.0f ), D3DXFROMWINEVECTOR2( 1.0f, 0.0f ) },
        { D3DXFROMWINEVECTOR3( 1.0f, 1.0f, -1.0f ), D3DXFROMWINEVECTOR2( 1.0f, 1.0f ) },
        { D3DXFROMWINEVECTOR3( 1.0f, 1.0f, 1.0f ), D3DXFROMWINEVECTOR2( 0.0f, 1.0f ) },

        { D3DXFROMWINEVECTOR3( -1.0f, -1.0f, -1.0f ), D3DXFROMWINEVECTOR2( 0.0f, 0.0f ) },
        { D3DXFROMWINEVECTOR3( 1.0f, -1.0f, -1.0f ), D3DXFROMWINEVECTOR2( 1.0f, 0.0f ) },
        { D3DXFROMWINEVECTOR3( 1.0f, 1.0f, -1.0f ), D3DXFROMWINEVECTOR2( 1.0f, 1.0f ) },
        { D3DXFROMWINEVECTOR3( -1.0f, 1.0f, -1.0f ), D3DXFROMWINEVECTOR2( 0.0f, 1.0f ) },

        { D3DXFROMWINEVECTOR3( -1.0f, -1.0f, 1.0f ), D3DXFROMWINEVECTOR2( 0.0f, 0.0f ) },
        { D3DXFROMWINEVECTOR3( 1.0f, -1.0f, 1.0f ), D3DXFROMWINEVECTOR2( 1.0f, 0.0f ) },
        { D3DXFROMWINEVECTOR3( 1.0f, 1.0f, 1.0f ), D3DXFROMWINEVECTOR2( 1.0f, 1.0f ) },
        { D3DXFROMWINEVECTOR3( -1.0f, 1.0f, 1.0f ), D3DXFROMWINEVECTOR2( 0.0f, 1.0f ) },
    };

    D3D11_BUFFER_DESC bd;
    ZeroMemory( &bd, sizeof(bd) );
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof( SimpleVertex ) * 24;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory( &InitData, sizeof(InitData) );
    InitData.pSysMem = vertices;
    hr = g_pd3dDevice->CreateBuffer( &bd, &InitData, &g_pVertexBuffer );
    if( FAILED( hr ) )
        return hr;

    UINT stride = sizeof( SimpleVertex );
    UINT offset = 0;
    g_pImmediateContext->IASetVertexBuffers( 0, 1, &g_pVertexBuffer, &stride, &offset );

    WORD indices[] =
    {
        3,1,0,
        2,1,3,

        6,4,5,
        7,4,6,

        11,9,8,
        10,9,11,

        14,12,13,
        15,12,14,

        19,17,16,
        18,17,19,

        22,20,21,
        23,20,22
    };

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof( WORD ) * 36;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    InitData.pSysMem = indices;
    hr = g_pd3dDevice->CreateBuffer( &bd, &InitData, &g_pIndexBuffer );
    if( FAILED( hr ) )
        return hr;

    g_pImmediateContext->IASetIndexBuffer( g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0 );

    g_pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(CBNeverChanges);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    hr = g_pd3dDevice->CreateBuffer( &bd, NULL, &g_pCBNeverChanges );
    if( FAILED( hr ) )
        return hr;
    
    bd.ByteWidth = sizeof(CBChangeOnResize);
    hr = g_pd3dDevice->CreateBuffer( &bd, NULL, &g_pCBChangeOnResize );
    if( FAILED( hr ) )
        return hr;
    
    bd.ByteWidth = sizeof(CBChangesEveryFrame);
    hr = g_pd3dDevice->CreateBuffer( &bd, NULL, &g_pCBChangesEveryFrame );
    if( FAILED( hr ) )
        return hr;

    hr = D3DX11CreateShaderResourceViewFromFile( g_pd3dDevice, "seafloor.dds", NULL, NULL, &g_pTextureRV, NULL );
    if( FAILED( hr ) )
        return hr;

    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory( &sampDesc, sizeof(sampDesc) );
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    hr = g_pd3dDevice->CreateSamplerState( &sampDesc, &g_pSamplerLinear );
    if( FAILED( hr ) )
        return hr;

    g_World = D3DXFROMWINEMatrixIdentity();

    XMVECTOR Eye = XMVectorSet( 0.0f, 3.0f, -6.0f, 0.0f );
    XMVECTOR At = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
    XMVECTOR Up = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
    g_View = D3DXFROMWINEMatrixLookAtLH( Eye, At, Up );

    CBNeverChanges cbNeverChanges;
    cbNeverChanges.mView = D3DXFROMWINEMatrixTranspose( g_View );
    g_pImmediateContext->UpdateSubresource( g_pCBNeverChanges, 0, NULL, &cbNeverChanges, 0, 0 );

    g_Projection = D3DXFROMWINEMatrixPerspectiveFovLH( D3DXFROMWINE_PI/4, width / (FLOAT)height, 0.01f, 100.0f );
    
    CBChangeOnResize cbChangesOnResize;
    cbChangesOnResize.mProjection = D3DXFROMWINEMatrixTranspose( g_Projection );
    g_pImmediateContext->UpdateSubresource( g_pCBChangeOnResize, 0, NULL, &cbChangesOnResize, 0, 0 );

    return S_OK;
}

void CleanupDevice()
{
    if( g_pImmediateContext ) g_pImmediateContext->ClearState();

    if( g_pSamplerLinear ) g_pSamplerLinear->Release();
    if( g_pTextureRV ) g_pTextureRV->Release();
    if( g_pCBNeverChanges ) g_pCBNeverChanges->Release();
    if( g_pCBChangeOnResize ) g_pCBChangeOnResize->Release();
    if( g_pCBChangesEveryFrame ) g_pCBChangesEveryFrame->Release();
    if( g_pVertexBuffer ) g_pVertexBuffer->Release();
    if( g_pIndexBuffer ) g_pIndexBuffer->Release();
    if( g_pVertexLayout ) g_pVertexLayout->Release();
    if( g_pVertexShader ) g_pVertexShader->Release();
    if( g_pPixelShader ) g_pPixelShader->Release();
    if( g_pDepthStencil ) g_pDepthStencil->Release();
    if( g_pDepthStencilView ) g_pDepthStencilView->Release();
    if( g_pRenderTargetView ) g_pRenderTargetView->Release();
    if( g_pHeightRenderTargetView ) g_pHeightRenderTargetView->Release();
    if( g_pTerrainRenderTargetView ) g_pTerrainRenderTargetView->Release();
    if( g_pSwapChain ) g_pSwapChain->Release();
    if( g_pImmediateContext ) g_pImmediateContext->Release();
    if( g_pd3dDevice ) g_pd3dDevice->Release();
}

void Render()
{
    static float t = 0.0f;
    if( g_driverType == D3D_DRIVER_TYPE_REFERENCE )
    {
        t += ( float )D3DXFROMWINE_PI * 0.0125f;
    }
    else
    {
        static DWORD dwTimeStart = 0;
        DWORD dwTimeCur = GetTickCount();
        if( dwTimeStart == 0 )
            dwTimeStart = dwTimeCur;
        t = ( dwTimeCur - dwTimeStart ) / 1000.0f;
    }

    g_World = D3DXFROMWINEMatrixRotationY( t );

    g_vMeshColor.x = ( sinf( t * 1.0f ) + 1.0f ) * 0.5f;
    g_vMeshColor.y = ( cosf( t * 3.0f ) + 1.0f ) * 0.5f;
    g_vMeshColor.z = ( sinf( t * 5.0f ) + 1.0f ) * 0.5f;





    g_pImmediateContext->OMSetRenderTargets( 1, &g_pHeightRenderTargetView, g_pDepthStencilView );
    g_pImmediateContext->OMSetRenderTargets( 1, &g_pTerrainRenderTargetView, g_pDepthStencilView );




    g_pImmediateContext->OMSetRenderTargets( 1, &g_pRenderTargetView, g_pDepthStencilView );
    float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
    g_pImmediateContext->ClearRenderTargetView( g_pRenderTargetView, ClearColor );

    g_pImmediateContext->ClearDepthStencilView( g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );

    CBChangesEveryFrame cb;
    cb.mWorld = D3DXFROMWINEMatrixTranspose( g_World );
    cb.vMeshColor = g_vMeshColor;
    g_pImmediateContext->UpdateSubresource( g_pCBChangesEveryFrame, 0, NULL, &cb, 0, 0 );

    g_pImmediateContext->VSSetShader( g_pVertexShader, NULL, 0 );
    g_pImmediateContext->VSSetConstantBuffers( 0, 1, &g_pCBNeverChanges );
    g_pImmediateContext->VSSetConstantBuffers( 1, 1, &g_pCBChangeOnResize );
    g_pImmediateContext->VSSetConstantBuffers( 2, 1, &g_pCBChangesEveryFrame );
    g_pImmediateContext->PSSetShader( g_pPixelShader, NULL, 0 );
    g_pImmediateContext->PSSetConstantBuffers( 2, 1, &g_pCBChangesEveryFrame );
    g_pImmediateContext->PSSetShaderResources( 0, 1, &g_pTextureRV );
    g_pImmediateContext->PSSetSamplers( 0, 1, &g_pSamplerLinear );
    g_pImmediateContext->DrawIndexed( 36, 0, 0 );

    g_pSwapChain->Present( 0, 0 );
}

