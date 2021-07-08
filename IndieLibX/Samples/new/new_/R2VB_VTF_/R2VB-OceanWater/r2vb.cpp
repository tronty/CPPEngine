//-----------------------------------------------------------------------------
// File: r2vb.cpp
// Copyright (c) 2006 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#include <assert.h>
#include <windows.h>
#include <D3DX9.h>

#include "r2vb.h"

#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

typedef struct {
    LPDIRECT3DDEVICE9       pdev;
    IDirect3DVertexBuffer9* vbdummy;
} R2VB, *PR2VB;


//---------------------------------------------------------------------------
// r2vbCheckCap
//
// Checks to see if the device supports render to vertex buffer
//---------------------------------------------------------------------------
HRESULT r2vbCheckCap(LPDIRECT3D9 pd3d)
{
    return pd3d->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8,
                                   0, D3DRTYPE_SURFACE, 
                                   (D3DFORMAT) R2VB_FOURCC_R2VB);
}



//---------------------------------------------------------------------------
// r2vbInit
//
// Initializes render to vertex buffer support
//---------------------------------------------------------------------------
HRESULT r2vbInit (LPDIRECT3DDEVICE9 pdev, DWORD size, PR2VBHANDLE phr2vb)
{   
    HRESULT hr = !D3D_OK; 
    
    assert(phr2vb);
    *phr2vb = NULL;

    PR2VB pr2vb = new R2VB;
    if (pr2vb)
    {
        pr2vb->pdev = pdev;
        if(hr = pdev->CreateVertexBuffer(
                size, D3DUSAGE_WRITEONLY, 0,
                D3DPOOL_DEFAULT, &(pr2vb->vbdummy), NULL))
        {   
            return hr;
        }
        hr = D3D_OK; 
    }
    
    // Npatching is not supported with R2VB
    pdev->SetNPatchMode(0);

    // enabled the R2VB back-door for the device
    pdev->SetRenderState(D3DRS_POINTSIZE, r2vbGlbEnable_Set (TRUE));

    // initialize to the disabled state
    for( int s = 0; s < 16; s++)
    {   DWORD dw = r2vbVStrm2SmpMap_Set (s , R2VB_VSMP_OVR_DIS);
        // Signal the driver to that next SetStreamSource is a tex VB
        pdev->SetRenderState(D3DRS_POINTSIZE, dw);
    }

    *phr2vb = (PR2VBHANDLE) pr2vb;
    return hr;
}


//---------------------------------------------------------------------------
// r2vbTerm
//
// Disables render to vb and releases r2vb data
//---------------------------------------------------------------------------
HRESULT r2vbTerm (R2VBHANDLE    hr2vb)
{
    PR2VB pr2vb = (PR2VB) hr2vb;

    if (!pr2vb)
        return D3DERR_NOTFOUND;

    LPDIRECT3DDEVICE9 pdev = pr2vb->pdev;

    // disable the R2VB back-door for the device
    pdev->SetRenderState(D3DRS_POINTSIZE, r2vbGlbEnable_Set (FALSE));

    SAFE_RELEASE(pr2vb->vbdummy);
    delete pr2vb;
    return D3D_OK; 
}


//-----------------------------------------------------------------------------
//  r2vbSetStreamTexSource
//
//   Set the vertex stream source to a texture
//-----------------------------------------------------------------------------
HRESULT r2vbSetStreamTexSource(
        R2VBHANDLE              hr2vb,
        DWORD                   stream,
        DWORD                   sampler,
        IDirect3DTexture9*      ptex,
        DWORD                   offset,
        DWORD                   stride)
{
    PR2VB pr2vb = (PR2VB) hr2vb;
    LPDIRECT3DDEVICE9 pdev = pr2vb->pdev;
    HRESULT hr = D3D_OK;
    
    hr = pdev->SetTexture(D3DDMAPSAMPLER + sampler, ptex);

    // Use default sampler state
    if SUCCEEDED(hr)
    {
        DWORD dw = r2vbVStrm2SmpMap_Set (stream, sampler);
        // Signal the driver to that next SetStreamSource is a tex VB
        hr = pdev->SetRenderState(D3DRS_POINTSIZE, dw);
    }

    if SUCCEEDED(hr)
    {
        hr = pdev->SetStreamSource(stream, pr2vb->vbdummy, offset, stride);
    }

    return hr;
}

//---------------------------------------------------------------------------
// r2vbDisTexSource
//
// Disables render to vertex buffer for this stream
//---------------------------------------------------------------------------
HRESULT r2vbDisTexSource(
        R2VBHANDLE              hr2vb,
        DWORD                   stream)
{
    PR2VB pr2vb = (PR2VB) hr2vb;
    LPDIRECT3DDEVICE9 pdev;

    if (pr2vb)
    {
        pdev = pr2vb->pdev;
        pdev->SetTexture(D3DDMAPSAMPLER + stream, NULL);

        DWORD dw = r2vbVStrm2SmpMap_Set (stream, R2VB_VSMP_OVR_DIS);

        // Signal the driver to that next SetStreamSource is a tex VB
        pdev->SetRenderState(D3DRS_POINTSIZE, dw);
    }

    return D3D_OK;
}

