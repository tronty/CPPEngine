/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

struct VsIn {
	float2 position: POSITION;
	float2 texCoord: TEXCOORD0;
};
struct VsOut {
	float4 position: POSITION;
	float2 texCoord: TEXCOORD0;
};

//Vertex shader // ???
ROW_MAJOR float4x4 worldViewProj MVPSEMANTIC;
VsOut mainVS(VsIn In){
	VsOut Out=(VsOut)0;
	Out.position = mul(worldViewProj, float4(In.position.x, In.position.y, 0.0, 1.0));
	Out.texCoord = In.texCoord;
	return Out;
}

//Fragment shader // ???
sampler2D Base;
float4 mainPS(VsOut IN): COLOR {
	//return float4(0,1,0,1);
	return tex2D(Base, IN.texCoord);
}
/*
[techniquesCG]
technique Simple
{
    pass
    {      
        VertexProgram   = compile latest mainVS();
        FragmentProgram = compile latest mainPS();
    }
}
[techniques11]
technique11 Simple
{
    pass P0
    {
        SetRasterizerState(CullBackMS);
        SetDepthStencilState(DepthNormal, 0);
        SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_4_0, mainVS()));
        SetHullShader(NULL);
        SetDomainShader(NULL);
        SetGeometryShader(NULL);
        SetComputeShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, mainPS()));
    }
}
[techniques10]
technique10 Simple
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, mainVS( ) ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, mainPS( ) ) );
        SetRasterizerState( Solid );
        SetBlendState( DisableBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( EnableDepth, 0 );
    }
}
[techniques9]
technique Simple
{
    pass P0
    {          
        VertexShader = compile vs_3_0 mainVS();
        PixelShader  = compile ps_3_0 mainPS();
    }
}
*/
[techniques]
<Textures>
	    <Texture sampler="Base" file="/test.bmp" dimension="2" mipmap="false" SamplerState="linear" />
</Textures>
<Techniques>
<technique name="Simple">
    <pass name="P0">
        <!-- 
        <Textures>
	        <Texture sampler="Base" file="/test.bmp" dimension="2" mipmap="false" SamplerState="linear" />
        </Textures>
        -->
        <DepthState name="NoDepthTest" />
	    <RasterizerState name="NoCullMS" />
        <DepthStencilState name="NoDepthStencil" X="0" />
        <BlendState name="NoBlending" R="0.0" G="0.0" B="0.0" A="0.0" X="0xFFFFFFFF" />
	    <PixelShader profile="ps_3_3" name="mainPS" />
        <VertexShader profile="vs_3_3" name="mainVS" />
        <GeometryShader profile="gs_4_0" name="" />
        <HullShader profile="hs_5_0" name="" />
        <DomainShader profile="ds_5_0" name="" />
        <ComputeShader profile="cs_5_0" name="" />
	<VertexFormat>
        	<FormatDesc Type="VERTEX" 	Format="FLOAT2" />
		<FormatDesc Type="TEXCOORD"	Format="FLOAT2" />
	</VertexFormat>
    </pass>
</technique>
</Techniques>

