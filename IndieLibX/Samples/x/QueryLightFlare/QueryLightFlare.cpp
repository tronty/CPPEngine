//-----------------------------------------------------------------------------
// File: QueryLightFlare.cpp
// Copyright (c) 2005 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------





#include "QueryLightFlare.h"




//---------------------------------------------------------------------------
/*
    Welcome to the QueryLightFlare sample source!  This sample demonstrates
    the use of occlusion queries to create a light-flare effect.

    Functions of interest:
        - load()
        - unload()
        - drawFrame()
*/
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
// Global defines.
//---------------------------------------------------------------------------
#define SAFE_RELEASE(p) { if( (p) ) { p->Release(); p = NULL; } }




//---------------------------------------------------------------------------
// Create an instance of the app.
//---------------------------------------------------------------------------
BaseApp *app = new SampleApp();




//---------------------------------------------------------------------------
// App load
//
//---------------------------------------------------------------------------
bool SampleApp::load()
{
    // Make sure the device supports what we need
    //
    if( FAILED( d3d->CheckDeviceFormat( caps.AdapterOrdinal, caps.DeviceType,
                    m_deviceSettings.adapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, 
                    D3DRTYPE_TEXTURE, m_deviceSettings.pp.BackBufferFormat ) ) )
        return false;

	if( FAILED( d3d->CheckDeviceFormat( caps.AdapterOrdinal, caps.DeviceType,
		            m_deviceSettings.adapterFormat, D3DUSAGE_RENDERTARGET | D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING,
					D3DRTYPE_SURFACE, m_deviceSettings.pp.BackBufferFormat ) ) )
		return false;


    // Load a bunch of data into the render manager
    //
    const VertexAttribute lfVertexAttribs[] = {
        { TYPE_VERTEX,   FORMAT_FLOAT, 3 }, // POSITION
        { TYPE_TEXCOORD, FORMAT_FLOAT, 2 }  // TEXCOORD0
    };

    if( VF_NONE == ( m_lfVertexFormat = renderer->addVertexFormat( lfVertexAttribs, elementsOf( lfVertexAttribs ) ) ) )
        return false;

    if( VB_NONE == ( m_leaf = renderer->addVertexBuffer( m_lfVbSizeInBytes, NULL, 0 ) ) )
        return false;

    if( VB_NONE == ( m_flare = renderer->addVertexBuffer( m_lfVbSizeInBytes, NULL, 0 ) ) )
        return false;

    if( TEXTURE_NONE == ( m_leafTexture = renderer->addTexture( TEX_PATH "MapleLeaves.dds", FILTER_LINEAR, CLAMP ) ) )
        return false;

    if( TEXTURE_NONE == ( m_flareTexture = renderer->addTexture( TEX_PATH "Flare.png", FILTER_LINEAR, CLAMP ) ) )
        return false;

    updateLfData();

	if (!onReset()) return false;

    return true;
}


//---------------------------------------------------------------------------
// App update leaf/flare data
//
//---------------------------------------------------------------------------
void SampleApp::updateLfData()
{
    const float leafData[] = {
		-1.0f,  1.0f, 0.0f, 0.0f, 0.0f, // x, y, z, tu, tv
		-1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 1.0f
    };
    renderer->updateVertexBuffer( m_leaf, 0, m_lfVbSizeInBytes, (void*)&leafData[0] );

    const float flareData[] = {
		-2.5f,  2.5f, 0.0f, 0.0f, 1.0f, // x, y, z, tu, tv
		-2.5f, -2.5f, 0.0f, 0.0f, 0.0f,
		 2.5f,  2.5f, 0.0f, 1.0f, 1.0f,
		 2.5f, -2.5f, 0.0f, 1.0f, 0.0f
    };
    renderer->updateVertexBuffer( m_flare, 0, m_lfVbSizeInBytes, (void*)&flareData[0] );
}


//---------------------------------------------------------------------------
// App unload
//
//---------------------------------------------------------------------------
bool SampleApp::unload()
{
    return onPreReset();
}

bool SampleApp::onPreReset()
{
    SAFE_RELEASE( m_pLightObject );
	for( int i = 0; i < NUM_QUERIES; i++ )
	{
		SAFE_RELEASE( m_pLightMaximalVizQuery[i] );
		SAFE_RELEASE( m_pLightActualVizQuery[i]  );
	}

	return true;
}

bool SampleApp::onReset()
{
    if( FAILED( D3DXLoadMeshFromX( MODEL_PATH "sphere0.x", D3DXMESH_MANAGED, dev, NULL, NULL, NULL, NULL, &m_pLightObject ) ) )
        return false;

	// Check to see if device supports visibility query
    //
    if( D3DERR_NOTAVAILABLE == dev->CreateQuery( D3DQUERYTYPE_OCCLUSION, NULL ) )
    {
        m_deviceSupportsOcclusionQuery = false;
    }
    else
    {
        m_deviceSupportsOcclusionQuery = true;

        // Create visibility queries
		for( int i = 0; i < NUM_QUERIES; i++ )
		{
			dev->CreateQuery( D3DQUERYTYPE_OCCLUSION, &m_pLightMaximalVizQuery[i] );
			dev->CreateQuery( D3DQUERYTYPE_OCCLUSION, &m_pLightActualVizQuery[i] );
		}
    }
	return true;
}



//---------------------------------------------------------------------------
// App init GUI
//
//---------------------------------------------------------------------------
void SampleApp::initGUI()
{
    m_renderMenu = mainMenu->addMenu( "Rendering" );
    m_myMenuItem[0] = m_renderMenu->addItem( "L: Pause light source animation" );
    m_myMenuItem[1] = m_renderMenu->addItem( "P: Pause all animation" );
    BaseApp::initGUI();
}


//---------------------------------------------------------------------------
// App time update ('tick')
//
//---------------------------------------------------------------------------
void SampleApp::updateTime(const float fTime)
{
    BaseApp::updateTime( fTime );

    if( m_animateLight )
    {
        const float rotAngle = fmodf( 2.0F * time, D3DX_PI * 2.0F );
        m_worldLightObject = rotateY4(  rotAngle );
        mat4 rotY          = rotateY4(  rotAngle );
        mat4 rotMinusY     = rotateY4( -rotAngle );
        mat4 transX        = translate( vec3( 1.5F, 0.0F, 0.0F ) );
        mat4 transZ        = translate( vec3( 0.0F, 0.0F, 8.0F ) );
        mat4 scale         = scale4( 0.1F, 0.1F, 0.1F );

        m_worldLightObject = transZ * rotY * transX * m_worldLightObject * scale;
        m_worldLightFlare  = transZ * rotY * transX * rotMinusY;
    }

    m_worldBlocker = translate( vec3( 0.5F * sinf( 0.25F * time ), 0.0F, 0.0F ) );
}


//---------------------------------------------------------------------------
// App draw frame
//
//---------------------------------------------------------------------------
void SampleApp::drawFrame()
{
    // Set up some default render states for the frame
    //
    dev->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    dev->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

    dev->Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000044, 1.0f, 0L );


    //
    // Set up to draw the leaf
    //

    ((D3D9Renderer*)renderer)->setTexture( (uint32)0, m_leafTexture );
    renderer->setVertexBuffer( m_leaf );
    renderer->setVertexFormat( m_lfVertexFormat );
    renderer->setCullFace( CULL_NONE );
    renderer->apply();

    const mat4 view = lookAt( vec3( 0.0F, 0.0F, -5.0F ), vec3( 0.0F, 0.0F, 0.0F ) );
    renderer->changeProjectionMatrix( perspectiveMatrixX( D3DX_PI / 4.0F, width, height, 1.0F, 100.0F, d3dStyleProjMatrix ) );
    renderer->changeModelViewMatrix( view * m_worldBlocker );

    dev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    dev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
    dev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    dev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    dev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );
    dev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
    dev->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    dev->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

    // Mask away the blocking leaves with alpha testing
    dev->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
    dev->SetRenderState( D3DRS_ALPHAREF, m_alphaRef );
    dev->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATER );

    // Antialias the edges with alpha blending
    dev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    dev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
    dev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

    // Draw the maple leaf blocking the light
    renderer->drawArrays( TRIANGLE_STRIP, 0, 4 );

    // Reset the device states
    dev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    dev->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
    dev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR );
    dev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );


    //
    // Set up the light object render states
    //

    renderer->setCullFace( BACK );
    renderer->apply();

    renderer->changeModelViewMatrix( view * m_worldLightObject );

    dev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2 );
    dev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );

    if( m_deviceSupportsOcclusionQuery )
    {
        //
        // Issue visibility queries
        //

        // Turn off Z write and color write.  Always pass Z compare.
        dev->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
        dev->SetRenderState( D3DRS_COLORWRITEENABLE, FALSE );
        dev->SetRenderState( D3DRS_ZFUNC, D3DCMP_ALWAYS );

        m_pLightMaximalVizQuery[m_frame % NUM_QUERIES]->Issue( D3DISSUE_BEGIN );

        m_pLightObject->DrawSubset( 0 );

        m_pLightMaximalVizQuery[m_frame % NUM_QUERIES]->Issue(D3DISSUE_END);

        // Turn on Z write and color write.  Use LEqual Z compare.
        dev->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
        dev->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
        dev->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALPHA |
                                                     D3DCOLORWRITEENABLE_BLUE  |
                                                     D3DCOLORWRITEENABLE_GREEN |
                                                     D3DCOLORWRITEENABLE_RED );

        m_pLightActualVizQuery[m_frame % NUM_QUERIES]->Issue( D3DISSUE_BEGIN );

        m_pLightObject->DrawSubset( 0 );

        m_pLightActualVizQuery[m_frame % NUM_QUERIES]->Issue( D3DISSUE_END );



        //
        // Wait for visibility query results
        //

		if( m_frame >= NUM_QUERIES - 1 )
		{
			// Block until we have a query result from one of the previous frames
			while( S_FALSE == m_pLightMaximalVizQuery[(m_frame - NUM_QUERIES + 1) % NUM_QUERIES]->GetData( (void*)&m_maximalVizData, sizeof( __DWORD__ ), D3DGETDATA_FLUSH ) )
			{
				// void
			}

			// Block until we have a query result from one of the previous frames
			while( S_FALSE == m_pLightActualVizQuery[(m_frame - NUM_QUERIES + 1) % NUM_QUERIES]->GetData( (void*)&m_actualVizData, sizeof( __DWORD__ ), D3DGETDATA_FLUSH ) )
			{
				// void
			}
		}
		else
		{
			// fake data for the first frames where query data isn't available
			m_maximalVizData = 1;
			m_actualVizData = 0;
		}
    }


    //
    // Draw the flare geometry blended over the light source
    //

    if ( 0 != m_actualVizData )
    {
        // Set up to draw the light flare
        //
        ((D3D9Renderer*)renderer)->setTexture( (uint32)0, m_flareTexture );
        renderer->setVertexBuffer( m_flare );
        renderer->setVertexFormat( m_lfVertexFormat );
        renderer->setCullFace( CULL_NONE );
        renderer->apply();

        dev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
        dev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );

        dev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        dev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
        dev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
        dev->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
        dev->SetRenderState( D3DRS_ZFUNC, D3DCMP_ALWAYS );

        m_lightBrightness = (BYTE) (255*(((float)m_actualVizData)/((float)m_maximalVizData)));
        dev->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA(m_lightBrightness, m_lightBrightness, m_lightBrightness, m_lightBrightness));

        renderer->changeModelViewMatrix( view * m_worldLightFlare );

        // Draw the light flare
        renderer->drawArrays( TRIANGLE_STRIP, 0, 4 );

        // Unset the device states
        //
        dev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
        dev->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
        dev->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
        dev->SetRenderState( D3DRS_TEXTUREFACTOR, 0xFFFFFFFF );
        dev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR );
        dev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
    }

    // Reset device states to defaults (for subsequent rendering)
    //
    dev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    dev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );
    dev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    dev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    dev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
    dev->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );

    // Render some text to the GUI
    //
    renderText();

	// Update frame count
	//
	m_frame++;
}


//---------------------------------------------------------------------------
// App render text
//
// Utility function to draw some GUI text...
//---------------------------------------------------------------------------
void SampleApp::renderText()
{
    renderer->setCullFace( BACK );
    renderer->setBlending( CONSTANT_COLOR, ZERO );
    renderer->setDepthFunc( DEPTH_NONE );
    renderer->apply();

	renderer->changeModelViewMatrix( identity4() );
	renderer->changeProjectionMatrix( orthoMatrix( 0, (float)width, 0, (float)height, -1.0F, 1.0F, d3dStyleProjMatrix ) );

    renderer->changeBlendConstantColor( vec4( 1.0F, 0.6F, 0.3F, 0.0F ) );

    char buf[255];
    if( m_deviceSupportsOcclusionQuery )
    {
        sprintf( buf, "%u light source pixels visible of %u possible", m_actualVizData, m_maximalVizData );
    }
    else
    {
        sprintf( buf, "Occlusion query not supported by device" );
    }

    mainFont.drawText( renderer, buf, 8, (float)height, 24, 30, 0.17f, -0.12f, HA_LEFT, VA_BOTTOM );
}


//---------------------------------------------------------------------------
// App keyboard handler.
//
//---------------------------------------------------------------------------
bool SampleApp::onKeyPress( const unsigned int key, const bool pressed )
{
    if( BaseApp::onKeyPress( key, pressed ) ) {
        return true;
    }

    if( pressed )
    {
        switch( key )
        {
        case STX_KEY_L:
            m_animateLight = !m_animateLight;
            break;
        case STX_KEY_P:
            setStatic( !isStatic );
            break;
        }
    }

    return false;
}


//---------------------------------------------------------------------------
// App menu handler.
//
//---------------------------------------------------------------------------
void SampleApp::onMenuItemPress( Menu *menu, MenuItemID item )
{
    BaseApp::onMenuItemPress( menu, item );

    if( menu == m_renderMenu )
    {
        if( item == m_myMenuItem[0] )
        {
            m_animateLight = !m_animateLight;
        }
        else // item == m_myMenuItem[1]
        {
            setStatic( !isStatic );
        }
    }
}
