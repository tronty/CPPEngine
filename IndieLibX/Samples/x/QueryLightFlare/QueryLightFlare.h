//-----------------------------------------------------------------------------
// File: QueryLightFlare.h
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------





#ifndef QUERYLIGHTFLARE_H
#define QUERYLIGHTFLARE_H


#include "../../Framework/Direct3D/D3D9App.h"

#define NUM_QUERIES 2

//---------------------------------------------------------------------------
// Main sample application class.
//---------------------------------------------------------------------------
class SampleApp : public D3D9App
{
public:
    SampleApp()
        : m_lfVertexFormat( VF_NONE )
        , m_lfVbSizeInBytes( 80 )
        , m_leaf( VB_NONE )
        , m_flare( VB_NONE )
        , m_leafTexture( TEXTURE_NONE )
        , m_flareTexture( TEXTURE_NONE )
        , m_pLightObject( NULL )
        , m_deviceSupportsOcclusionQuery( false )
		, m_frame( 0 )
        , m_maximalVizData( 0 )
        , m_actualVizData( 0 )
        , m_alphaRef( 0x15 )
        , m_animateLight( true )
        , m_lightBrightness()
        , m_worldLightObject( identity4() )
        , m_worldLightFlare( identity4() )
        , m_worldBlocker( identity4() )
    {
		for( int i = 0; i < NUM_QUERIES; i++ )
		{
			m_pLightMaximalVizQuery[i]= NULL;
			m_pLightActualVizQuery[i]= NULL;
		}
	}

    ~SampleApp() {}

    //---------------------------------------------------------------------------
    // Overriden from framework
    //---------------------------------------------------------------------------
    char *getTitleString() { return "QueryLightFlare"; }
    char *getHomeDirectory() { return "../DirectX 9.0/QueryLightFlare"; }

    void setup(char *args[], int nArguments) {
        width  = 640;
        height = 480;
    }

    bool load();
    bool unload();

	bool onPreReset();
	bool onReset();

    void initGUI();
    void updateTime(const float fTime);
    void drawFrame();

    bool onKeyPress(const unsigned int key, const bool pressed);
    void onMenuItemPress( Menu *menu, MenuItemID item );

protected:
    // Helper utilities
    void renderText();
    void updateLfData();

private:
    const __DWORD__       m_lfVbSizeInBytes;              // size of a single vertex for the leaf and flare
    VertexFormatID    m_lfVertexFormat;               // leaf and flare (shared) vertex format handle
    VertexBufferID    m_leaf;                         // leaf vertex buffer handle (simple quad)
    VertexBufferID    m_flare;                        // flare vertex buffer handle (simple, scaled quad)
    TextureID         m_leafTexture;                  // leaf texture handle
    TextureID         m_flareTexture;                 // flare texture handle

    LPD3DXMESH        m_pLightObject;                 // mesh pointer

    bool              m_deviceSupportsOcclusionQuery; // flag set upon load()
    LPDIRECT3DQUERY9  m_pLightMaximalVizQuery[NUM_QUERIES];     // query event to determine max potential pixel count
    LPDIRECT3DQUERY9  m_pLightActualVizQuery[NUM_QUERIES];      // query event to determine actual pixel count
	__DWORD__             m_frame;

    __DWORD__             m_maximalVizData;               // maximum potential number of visible pixels
    __DWORD__             m_actualVizData;                // acutal number of pixels rendered

    const __DWORD__       m_alphaRef;                     // amount of alpha to test through
    bool              m_animateLight;                 // light animation flag
    BYTE              m_lightBrightness;              // controls brightness of light

    mat4              m_worldLightObject;             // model world matrices
    mat4              m_worldLightFlare;
    mat4              m_worldBlocker;

    // GUI stuff
    //
    Menu *m_renderMenu;
    MenuItemID m_myMenuItem[2];
};

#endif // QUERYLIGHTFLARE_H
