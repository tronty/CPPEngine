//-----------------------------------------------------------------------------
//           Name: ogl_cgfx_simple.fx
//         Author: Kevin Harris (kevin@codesampler.com)
//  Last Modified: 04/15/05
//    Description: This .fx file demonstrates how to write vertex and pixel
//                 shaders using nVIDIA's Cg shader language.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Effect File Variables
//-----------------------------------------------------------------------------

float4x4 worldViewProj : WorldViewProjection;

texture testTexture;
sampler2D Sampler = sampler_state
{
    Texture = <testTexture>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

// Our sample application will send vertices 
// down the pipeline laid-out like this...

struct VS_INPUT
{
    float3 position : POSITION;
    float2 texture0 : TEXCOORD0;
};

// Once the vertex shader is finished, it will 
// pass the vertices on to the pixel shader like this...

struct VS_OUTPUT
{
    float4 position : POSITION;
    float2 texture0 : TEXCOORD0;
    float4 color	: COLOR0;
};

// And finally, the pixel shader will send a single 
// color value to the frame buffer like this...

struct PS_OUTPUT
{
	float4 color : COLOR;
};

//-----------------------------------------------------------------------------
// Simple Vertex Shader
//-----------------------------------------------------------------------------

VS_OUTPUT myvs( const VS_INPUT IN,
			    uniform float4x4 modelViewProjection )
{
    VS_OUTPUT OUT;

    OUT.position = mul( modelViewProjection, float4(IN.position, 1.0) );

    OUT.texture0 = IN.texture0;
    
    OUT.color = float4( 1.0f, 1.0f, 1.0f, 1.0f ); // Pass white as a default color
    //OUT.color = float4( 0.0, 1.0, 0.0, 1.0 ); // Pass green to test vertex shader

    return OUT;
}

//-----------------------------------------------------------------------------
// Simple Pixel Shader
//-----------------------------------------------------------------------------

PS_OUTPUT myps( VS_OUTPUT IN )
{
    PS_OUTPUT OUT;

	// If you comment out the first line and uncomment the second line, the 
	// color passed to us by our vertex shader and the selected texture sampler 
	// will be ignored completely and all output to the frame buffer will be 
	// blue regardless.

	OUT.color = tex2D( Sampler, IN.texture0 ) * IN.color;
	//OUT.color = float4( 0.0, 0.0, 1.0, 1.0);

    return OUT;
}

//-----------------------------------------------------------------------------
// Simple Effect (1 technique with 1 pass)
//-----------------------------------------------------------------------------

technique Technique0
{
    pass Pass0
    {
        Zenable  = true;
        CullMode = None;

        VertexShader = compile arbvp1 myvs( worldViewProj );
        PixelShader  = compile arbfp1 myps();
    }
}
