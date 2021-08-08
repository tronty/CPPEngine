import os, sys, re

def replaceStringInFile(filePath):
    "replaces all string by a regex substitution"
    tempName=filePath+'~~~'
    input = open(filePath)
    outtext=input.read()
    input.close()
    outtext=re.sub(r'\bcaps\.MaxTextureHeight\b','IRenderer::GetRendererInstance()->GetMaxTextureSize()',outtext)
    output = open(tempName,'w')
    output.write(outtext)
    output.close()
    os.unlink(filePath)
    os.rename(tempName,filePath)

def replaceStringInFile_(filePath):
	"replaces all string by a regex substitution"
	tempName=filePath+'~~~'
	input = open(filePath)
	outtext=input.read()
	input.close()

	'''
	outtext=re.sub(r'\bdev\-\>SetTexture\(D3DDMAPSAMPLER\, NULL\)\;', '', outtext)
	outtext=re.sub(r'\bSetTexture\(D3DDMAPSAMPLER\b', 'setTexture(\"DMAP\"', outtext)

	outtext=re.sub(r'\bsetTexture\\\(', 'setTexture(', outtext)

	outtext=re.sub(r'\bIDirect3DBaseTexture9\s*\*', 'TextureID', outtext)
	outtext=re.sub(r'\bIDirect3DTexture9\s*\*', 'TextureID', outtext)
	outtext=re.sub(r'\bIDirect3DSurface9\s*\*', 'SurfaceID', outtext)
	outtext=re.sub(r'\bIDirect3DVertexBuffer9\s*\*', 'VertexBufferID', outtext)
	outtext=re.sub(r'\bIDirect3DIndexBuffer9\s*\*', 'IndexBufferID', outtext)
	outtext=re.sub(r'\bIDirect3DVertexDeclaration9\s*\*', 'VertexFormatID', outtext)
	#outtext=re.sub(r'\bIDirect3D9\*', 'ID', outtext)
	#outtext=re.sub(r'\bIDirect3D9\*', 'ID', outtext)
	outtext=re.sub(r'\bfloat4\b', 'D3DXFROMWINEVECTOR4', outtext)
	outtext=re.sub(r'\bfloat3\b', 'D3DXFROMWINEVECTOR3', outtext)
	outtext=re.sub(r'\bfloat2\b', 'D3DXFROMWINEVECTOR2', outtext)
	#outtext=re.sub(r'\bfloat4\b', 'D3DXFROMWINEVECTOR4', outtext)

	outtext=re.sub(r'\bD3DXMATRIX\b', 'D3DXFROMWINEMATRIX', outtext)
	outtext=re.sub(r'\bmat4\b', 'D3DXFROMWINEMATRIX', outtext)
	outtext=re.sub(r'\bfloat4x4\b', 'D3DXFROMWINEMATRIX', outtext)
	outtext=re.sub(r'\bD3DVIEWPORT9\b', 'D3DFROMWINEVIEWPORT9', outtext)
	outtext=re.sub(r'\bD3DXVECTOR4\b', 'D3DXFROMWINEVECTOR4', outtext)
	outtext=re.sub(r'\bD3DXVECTOR3\b', 'D3DXFROMWINEVECTOR3', outtext)
	outtext=re.sub(r'\bD3D9App\:\:onSize\(w\, h\)\;', '', outtext)
	outtext=re.sub(r'\bkey\=\=KEY\_', 'STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_', outtext)
	outtext=re.sub(r'\breturn BaseApp\:\:onKeyPress\(key\, pressed\)','return false', outtext)
	outtext=re.sub(r'\bPI\b', 'D3DXFROMWINE_PI', outtext)
	outtext=re.sub(r'\bHRESULT\b', 'int', outtext)
	outtext=re.sub(r'\bImage\b', 'Image3', outtext)
	outtext=re.sub(r'\brenderer\b', 'IRenderer::GetRendererInstance()', outtext)
	outtext=re.sub(r'\bdev\b', 'IRenderer::GetRendererInstance()', outtext)
	outtext=re.sub(r'\bERRORMSG\b', 'LOG_PRINT', outtext)
	outtext=re.sub(r'\(\s*TEX_PATH\s*\"', '', outtext)
	outtext=re.sub(r'\bTEX_PATH\s*\"', '', outtext)

	outtext=re.sub(r'\bDWORD\b', '__DWORD__', outtext)
	outtext=re.sub(r'\bWORD\b', '__WORD__', outtext)
	outtext=re.sub(r'\bNULL\b', '0', outtext)
	outtext=re.sub(r'\bpd3dDevice\b', 'IRenderer::GetRendererInstance()', outtext)

	outtext=re.sub(r'\#include \"\.\.\/\.\.\/Framework\/Direct3D\/D3D9App\.h\"', '', outtext)
	outtext=re.sub(r'\#include \"\.\.\/\.\.\/Framework\/Direct3D\/atir2vb\.h\"', '', outtext)
 	outtext=re.sub(r'\: public D3D9App', '', outtext)
	outtext=re.sub(r'\(LPDIRECT3DDEVICE9 IRenderer\:\:GetRendererInstance\(\),', '', outtext)
	outtext=re.sub(r'\(LPDIRECT3DDEVICE9 IRenderer\:\:GetRendererInstance\(\)\)', '', outtext)

	outtext=re.sub(r'\bBaseApp\b', 'SampleApp', outtext)

	outtext=re.sub(r'\bFILTER_POINT\b', '', outtext)
	outtext=re.sub(r'\bFILTER_BILINEAR\b', '', outtext)
	outtext=re.sub(r'\bFILTER_TRILINEAR\b', '', outtext)
	
	outtext=re.sub(r'\bD3DPT_TRIANGLELIST\b', 'PRIM_TRIANGLES', outtext)
	outtext=re.sub(r'\bBaseApp::updateTime( fTime );', '//BaseApp::updateTime( fTime );', outtext)
	outtext=re.sub(r'\bOutputDebugString\b', 'LOG_PRINT', outtext)
	outtext=re.sub(r'\IRenderer::GetRendererInstance()->SetSamplerState', '//IRenderer::GetRendererInstance()->SetSamplerState', outtext)
	outtext=re.sub(r'\IRenderer::GetRendererInstance()->SetRenderState', '//IRenderer::GetRendererInstance()->SetRenderState', outtext)

	outtext=re.sub(r'\bSetVertexDeclaration\b', 'setVertexFormat', outtext)
	outtext=re.sub(r'\bBaseApp\:\:onMouseMove\(x\, y\, lButton\, mButton\, rButton\)', '', outtext)
	outtext=re.sub(r'\bBaseApp\:\:onMouseClick\(x\, y\, button\, pressed\)', '', outtext)
	outtext=re.sub(r'\bchangeShaderConstant', 'setShaderConstant', outtext)

	outtext=re.sub(r'\bIRenderer\:\:GetRendererInstance\(\)\-\>apply\(\)\;', '', outtext)
	outtext=re.sub(r'\bsetDepthFunc\(DEPTH_NONE\)\;', 'setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());', outtext)

	outtext=re.sub(r'\bD3DPT_TRIANGLEFAN\b', 'PRIM_TRIANGLE_FAN', outtext)	
	outtext=re.sub(r'\bD3DPT_TRIANGLELIST\b', 'PRIM_TRIANGLES', outtext)
	outtext=re.sub(r'\bD3DPT_TRIANGLESTRIP\b', 'PRIM_TRIANGLE_STRIP', outtext)
	outtext=re.sub(r'\bD3DPT_LINELIST\b', 'PRIM_LINES', outtext)
	outtext=re.sub(r'\bD3DPT_LINESTRIP\b', 'PRIM_LINE_STRIP', outtext)
	outtext=re.sub(r'\bD3DPT_POINTLIST\b', 'PRIM_POINTS', outtext)
	outtext=re.sub(r'\bIRenderer\:\:GetRendererInstance\(\)\-\>setMask\(COLOR\)\;', '//IRenderer::GetRendererInstance()->setMask(COLOR);', outtext)
	outtext=re.sub(r'\bIRenderer\:\:GetRendererInstance\(\)\-\>setBlending\(CONSTANT_COLOR\, ZERO\)\;', '//IRenderer::GetRendererInstance()->setBlending(CONSTANT_COLOR, ZERO);', outtext)
	outtext=re.sub(r'\bsetBlendState\(SRC_ALPHA\, ONE_MINUS_SRC_ALPHA\)\;', 'setBlending(IRenderer::GetRendererInstance()->GetblendSrcAlpha());', outtext)
	outtext=re.sub(r'\bsetCullFace\(BACK\)\;', 'setRasterizerState(IRenderer::GetRendererInstance()->GetcullBack());', outtext)
	outtext=re.sub(r'\bIRenderer\:\:GetRendererInstance\(\)\-\>setDepthFunc\(LESS\)\;', 'IRenderer::GetRendererInstance()->setDepthFunc(LESS);', outtext)
	
	outtext=re.sub(r'\bStreamVertexAttribute\b', 'FormatDesc', outtext)
	outtext=re.sub(r'\bVertexAttribute\b', 'FormatDesc', outtext)
	outtext=re.sub(r'\bLPDIRECT3DTEXTURE9\b', 'TextureID', outtext)

	outtext=re.sub(r'\bFILTER_TRILINEAR_ANISO\b', 'IRenderer::GetRendererInstance()->GettrilinearAniso()', outtext)
	outtext=re.sub(r'\bD3DFORMAT', 'FORMAT', outtext)
	outtext=re.sub(r'\bD3DFMT_A16B16G16R16F', 'FORMAT_RGBA16F', outtext)
	outtext=re.sub(r'\bD3DFMT_A32B32G32R32F', 'FORMAT_RGBA32F', outtext)

	outtext=re.sub(r'\bSetTexture\(D3DVERTEXTEXTURESAMPLER0\, \(\(D3D9Renderer \*\) IRenderer\:\:GetRendererInstance\(\)\)\-\>getD3DTexture\b','setTexture(\"DMAP0\", ', outtext)
	outtext=re.sub(r'\bSetTexture\(D3DVERTEXTEXTURESAMPLER1\, \(\(D3D9Renderer \*\) IRenderer\:\:GetRendererInstance\(\)\)\-\>getD3DTexture\b','setTexture(\"DMAP1\", ', outtext)
	outtext=re.sub(r'\bSetTexture\(D3DVERTEXTEXTURESAMPLER2\, \(\(D3D9Renderer \*\) IRenderer\:\:GetRendererInstance\(\)\)\-\>getD3DTexture\b','setTexture(\"DMAP2\", ', outtext)
	outtext=re.sub(r'\bSetTexture\(D3DVERTEXTEXTURESAMPLER3\, \(\(D3D9Renderer \*\) IRenderer\:\:GetRendererInstance\(\)\)\-\>getD3DTexture\b','setTexture(\"DMAP3\", ', outtext)
	outtext=re.sub(r'\bSetTexture\(D3DVERTEXTEXTURESAMPLER4\, \(\(D3D9Renderer \*\) IRenderer\:\:GetRendererInstance\(\)\)\-\>getD3DTexture\b','setTexture(\"DMAP4\", ', outtext)
	
	outtext=re.sub(r'\bvoid App\:\:setup\(char \*args\[\]\, int nArguments\)', 'void App::setup()', outtext)
	outtext=re.sub(r'\bApp\:\:', '', outtext)

	outtext=re.sub(r'\"../Math/Vector.h\"', '<STX/STX.h>\n#include <STX/d3dx9mathFromWine.h>', outtext)
	outtext=re.sub(r'\"Vector.h\"', '<STX/STX.h>\n#include <STX/d3dx9mathFromWine.h>', outtext)

	renderer->setDepthFunc(LESS);
	renderer->apply();
    outtext=re.sub(r'\"../Platform.h\"', '<STX/STX.h>\n#include <STX/d3dx9mathFromWine.h>', outtext)
	outtext=re.sub(r'\brenderer\-\>apply\(\)\;', '', outtext)
	outtext=re.sub(r'\brenderer\-\>setDepthFunc\(LESS\)\;', '//IRenderer::GetRendererInstance()->setDepthFunc(LESS);', outtext)

	outtext=re.sub(r'\bchangeShaderConstant', 'setShaderConstant', outtext)	
	outtext=re.sub(r'\brenderer\b', 'IRenderer::GetRendererInstance()', outtext)	
	outtext=re.sub(r'\bdev\b', 'IRenderer::GetRendererInstance()', outtext)	
	outtext=re.sub(r'\.elem\[', '.m[', outtext)
	
	outtext=re.sub(r'\bmat4\b', 'D3DXFROMWINEMATRIX', outtext)
	outtext=re.sub(r'\bfloat4x4\b', 'D3DXFROMWINEMATRIX', outtext)
	outtext=re.sub(r'\bfloat4\b', 'D3DXFROMWINEVECTOR4', outtext)
	outtext=re.sub(r'\bfloat3\b', 'D3DXFROMWINEVECTOR3', outtext)
	outtext=re.sub(r'\bfloat2\b', 'D3DXFROMWINEVECTOR2', outtext)
	outtext=re.sub(r'\bvec4\b', 'D3DXFROMWINEVECTOR4', outtext)
	outtext=re.sub(r'\bvec3\b', 'D3DXFROMWINEVECTOR3', outtext)
	outtext=re.sub(r'\bvec2\b', 'D3DXFROMWINEVECTOR2', outtext)

    	#outtext=re.sub(r'\bRenderer \*IRenderer\:\:GetRendererInstance\(\)', '', outtext)
	outtext=re.sub(r'\bD3DPT_TRIANGLEFAN\b', 'PRIM_TRIANGLE_FAN', outtext)	
	outtext=re.sub(r'\bD3DPT_TRIANGLELIST\b', 'PRIM_TRIANGLES', outtext)
	outtext=re.sub(r'\bD3DPT_TRIANGLESTRIP\b', 'PRIM_TRIANGLE_STRIP', outtext)
	outtext=re.sub(r'\bD3DPT_LINELIST\b', 'PRIM_LINES', outtext)
	outtext=re.sub(r'\bD3DPT_LINESTRIP\b', 'PRIM_LINE_STRIP', outtext)
	outtext=re.sub(r'\bD3DPT_POINTLIST\b', 'PRIM_POINTS', outtext)

	outtext=re.sub(r'\bWORD\b', '__WORD__', outtext)
	outtext=re.sub(r'\bDWORD\b', '__DWORD__', outtext)
	
	outtext=re.sub(r'\bD3DX', 'D3DXFROMWINE', outtext)
	outtext=re.sub(r'FROMWINEFROMWINE', 'FROMWINE', outtext)

	outtext=re.sub(r'\bHRESULT\b', 'int', outtext) 
	outtext=re.sub(r'\bCALLBACK\b', '', outtext) 
	outtext=re.sub(r'\bV_RETURN\b', '', outtext) 
	outtext=re.sub(r'\bS_OK\b', '0', outtext) 
	outtext=re.sub(r'\bSAFE_DELETE\b', '//SAFE_DELETE', outtext) 
	outtext=re.sub(r'\bSAFE_RELEASE\b', '//SAFE_RELEASE', outtext) 

	outtext=re.sub(r'\,\s*void\s*\*\s*pUserContext\b', '', outtext) 
	outtext=re.sub(r'\bID3D10DeviceContext\s*\*\s*pd3dImmediateContext\s*\,', '', outtext) 
	outtext=re.sub(r'\bID3D11DeviceContext\s*\*\s*pd3dImmediateContext\s*\,', '', outtext) 

	outtext=re.sub(r'\bID3DX10Effect\s*\*', 'EffectID ', outtext) 
	outtext=re.sub(r'\bID3D10Texture2D\s*\*', 'TextureID ', outtext) 
	outtext=re.sub(r'\bID3D10DepthStencilView\s*\*', 'DepthStencilViewID ', outtext) 
	outtext=re.sub(r'\bID3D10ShaderResourceView\s*\*', 'ShaderResourceViewID ', outtext) 
	outtext=re.sub(r'\bID3D10Buffer\s*\*', 'exBufferID ', outtext) 
	outtext=re.sub(r'\bID3D10InputLayout\s*\*', 'VertexFormatID ', outtext) 

	outtext=re.sub(r'\bID3DX10Effect\b', 'EffectID ', outtext) 
	outtext=re.sub(r'\bID3D10Texture2D\b', 'TextureID ', outtext) 
	outtext=re.sub(r'\bID3D10DepthStencilView\b', 'DepthStencilViewID ', outtext) 
	outtext=re.sub(r'\bID3D10ShaderResourceView\b', 'ShaderResourceViewID ', outtext) 
	outtext=re.sub(r'\bID3D10Buffer\b', 'exBufferID ', outtext) 
	outtext=re.sub(r'\bID3D10InputLayout\b', 'VertexFormatID ', outtext) 

	outtext=re.sub(r'\bID3DX11Effect\s*\*', 'EffectID ', outtext) 
	outtext=re.sub(r'\bID3D11Texture2D\s*\*', 'TextureID ', outtext) 
	outtext=re.sub(r'\bID3D11DepthStencilView\s*\*', 'DepthStencilViewID ', outtext) 
	outtext=re.sub(r'\bID3D11ShaderResourceView\s*\*', 'ShaderResourceViewID ', outtext) 
	outtext=re.sub(r'\bID3D11Buffer\s*\*', 'exBufferID ', outtext) 
	outtext=re.sub(r'\bID3D11InputLayout\s*\*', 'VertexFormatID ', outtext) 

	outtext=re.sub(r'\bID3DX11Effect\b', 'EffectID', outtext) 
	outtext=re.sub(r'\bID3D11Texture2D\b', 'TextureID', outtext) 
	outtext=re.sub(r'\bID3D11DepthStencilView\b', 'DepthStencilViewID', outtext) 
	outtext=re.sub(r'\bID3D11ShaderResourceView\b', 'ShaderResourceViewID', outtext) 
	outtext=re.sub(r'\bID3D11Buffer\b', 'exBufferID', outtext) 
	outtext=re.sub(r'\bID3D11InputLayout\b', 'VertexFormatID', outtext) 

	outtext=re.sub(r'\bID3D11Device* pd3dDevice, 
	outtext=re.sub(r'\bIDirect3DDevice9*
	outtext=re.sub(r'\bIDirect3D9*

	outtext=re.sub(r'\bNULL\b', '0', outtext) 
	outtext=re.sub(r'\bOnD3D9', 'On', outtext) 
	outtext=re.sub(r'\bOnD3D10', 'On', outtext) 
	outtext=re.sub(r'\bOnD3D11', 'On', outtext) 
	outtext=re.sub(r'\bCHAR\b', 'char', outtext) 
	outtext=re.sub(r'\bWCHAR\b', 'char', outtext) 
	outtext=re.sub(r'\bD3D10_VIEWPORT\b', 'D3DFROMWINEVIEWPORT9', outtext)
	outtext=re.sub(r'\bD3D11_VIEWPORT\b', 'D3DFROMWINEVIEWPORT9', outtext)

	outtext=re.sub(r'\bID3D10Device\s*\*\s*pd3dDevice\s*\,\s*','', outtext)
	outtext=re.sub(r'\bID3D11Device\s*\*\s*pd3dDevice\s*\,\s*','', outtext)
	outtext=re.sub(r'\bpd3dDevice\s*\,\s*','', outtext)
	outtext=re.sub(r'\bID3D10Device\s*\*\s*pd3dDevice\b','', outtext)
	outtext=re.sub(r'\bID3D11Device\s*\*\s*pd3dDevice\b','', outtext)
	outtext=re.sub(r'\bpd3dDevice\b','', outtext)

	outtext=re.sub(r'\bD3D11_TEXTURE2D_DESC\b','TEXTURE_DESC', outtext)
	outtext=re.sub(r'\bD3D11_SHADER_RESOURCE_VIEW_DESC\b','SHADER_RESOURCE_VIEW_DESC', outtext)
	outtext=re.sub(r'\bD3D11_DEPTH_STENCIL_VIEW_DESC\b','DEPTH_STENCIL_VIEW_DESC', outtext)

	outtext=re.sub(r'\bDXGI_FORMAT_R8G8B8A8_UNORM\b', 'FORMAT_RGBA8', outtext)
	outtext=re.sub(r'\bDXGI_FORMAT_R32G32B32_FLOAT\b', 'FORMAT_RGBA32f', outtext)
	outtext=re.sub(r'\bDXGI_FORMAT_R32G32_FLOAT\b', 'FORMAT_RG32f', outtext)
	outtext=re.sub(r'\bID3D11RenderTargetView\s*\*', 'RenderTargetViewID ', outtext)
	outtext=re.sub(r'\bSUCCEEDED\b', '', outtext)
	outtext=re.sub(r'\bFAILED\b', '', outtext)
	outtext=re.sub(r'\bDXUTTRACE\(\s*L\"', '(', outtext)

	outtext=re.sub(r'\bID3D10Effect\s*\*', 'EffectID ', outtext)
	outtext=re.sub(r'\bID3D10StateBlock\s*\*', 'StateBlockID ', outtext)
	outtext=re.sub(r'\bCDXUTMesh10\b', 'MeshRenderer2', outtext)
	outtext=re.sub(r'\bID3DX10Mesh\s*\*', 'MeshRenderer2', outtext)

	outtext=re.sub(r'\bID3D10EffectVectorVariable\s*\*', 'EffectVectorVariableID ', outtext)
	outtext=re.sub(r'\bID3D10EffectMatrixVariable\s*\*', 'EffectMatrixVariableID ', outtext)
	outtext=re.sub(r'\bID3D10EffectScalarVariable\s*\*', 'EffectScalarVariableID ', outtext)
	outtext=re.sub(r'\bID3D10EffectShaderResourceVariable\s*\*', 'EffectShaderResourceVariableID ', outtext)
	outtext=re.sub(r'\bID3D10EffectTechnique\s*\*', 'EffectTechniqueID ', outtext)
	outtext=re.sub(r'\bD3D10_TEXTURE2D_DESC\b', 'TEXTURE2D_DESC', outtext)

	outtext=re.sub(r'\bD3D10_SHADER_RESOURCE_VIEW_DESC', 'SHADER_RESOURCE_VIEW_DESC', outtext)
	outtext=re.sub(r'\bD3D10_DEPTH_STENCIL_VIEW_DESC', 'EPTH_STENCIL_VIEW_DESC', outtext)
	outtext=re.sub(r'\bD3D10_PASS_DESC D3D10_INPUT_ELEMENT_DESC', 'PASS_DESC_INPUT_ELEMENT_DESC', outtext)
	outtext=re.sub(r'\bD3D10_PASS_DESC D3D11_INPUT_ELEMENT_DESC', 'PASS_DESC_INPUT_ELEMENT_DESC', outtext)

	outtext=re.sub(r'\bwchar_t\b', 'char', outtext) 
	outtext=re.sub(r'\bLPCWSTR\b', 'std::string', outtext)  
	outtext=re.sub(r'\bLPCSTR\b', 'std::string', outtext) 
	outtext=re.sub(r'\bLPWSTR\b', 'std::string', outtext)  
	outtext=re.sub(r'\bLPSTR\b', 'std::string', outtext)
	outtext=re.sub(r'\bLPVOID\b', 'void*', outtext)
	outtext=re.sub(r'\bMB_OK\b', '0', outtext)
	outtext=re.sub(r'\bMessageBox\b', 'LOG_PRINT', outtext)

    	outtext=re.sub(r'\bID3D10RenderTargetView\s*\*', 'RenderTargetViewID ', outtext)
    	outtext=re.sub(r'\bD3D10_PASS_DESC\b', 'PASS_DESC', outtext)
   	outtext=re.sub(r'\bD3D10_INPUT_ELEMENT_DESC\b', 'INPUT_ELEMENT_DESC', outtext)
    	outtext=re.sub(r'\bD3D10_INPUT_PER_VERTEX_DATA\b', 'INPUT_PER_VERTEX_DATA', outtext)
    	outtext=re.sub(r'\bCDXUTSDKMesh\b', 'MeshRenderer2', outtext)
    	outtext=re.sub(r'\bV\(', '(', outtext)
    	outtext=re.sub(r'//SAFE_DELETE\b', 'SAFE_DELETE', outtext)
    	outtext=re.sub(r'\bE_FAIL\b', '1', outtext)
    	outtext=re.sub(r'\bD3D10_TECHNIQUE_DESC\b', 'TECHNIQUE_DESC', outtext)
	outtext=re.sub(r'\bg_pD3DDevice\b', 'IRenderer::GetRendererInstance()', outtext)
    	outtext=re.sub(r'\bID3DXEffect\s*\*', 'EffectID', outtext)
    	outtext=re.sub(r'\bID3DXMesh\b', 'MeshRenderer2', outtext)
    	outtext=re.sub(r'\bIDirect3DBaseTexture9\s*\*\s*\*', 'TextureID', outtext)
    	outtext=re.sub(r'\bIDirect3DBaseTexture9\s*\*', 'TextureID', outtext)
    	outtext=re.sub(r'\bIDirect3DTexture9\s*\*\s*\*', 'TextureID', outtext)
    	outtext=re.sub(r'\bIDirect3DTexture9\s*\*', 'TextureID', outtext)
    	outtext=re.sub(r'\bD3DXFROMWINEMATRIXA16\b', 'D3DXFROMWINEMATRIX', outtext)
    	outtext=re.sub(r'\bprintf\b', 'LOG_PRINT', outtext)
    	outtext=re.sub(r'\bIDirect3DDevice9\s*\*', '', outtext)
    	outtext=re.sub(r'\bIDirect3DDevice9\b', '', outtext)

    	outtext=re.sub(r'\bconst INPUT_ELEMENT_DESC\b', '', outtext)
    	outtext=re.sub(r'\bINPUT_ELEMENT_DESC\b', '', outtext)
    	outtext=re.sub(r'\bINPUT_PER_VERTEX_DATA\b', '', outtext)
    	outtext=re.sub(r'\bDXGI_FORMAT_R32_FLOAT\b', 'FORMAT_R32f', outtext)
    	outtext=re.sub(r'\bIASetInputLayout\b', 'setVertexFormat', outtext)
    	outtext=re.sub(r'\bIASetVertexBuffers\b', 'setVertexBuffer', outtext)
    	outtext=re.sub(r'\bIASetIndexBuffer\b', 'setIndexBuffer', outtext)
    	outtext=re.sub(r'\bDXGI_FORMAT_R16_UINT\b', 'FORMAT_R16', outtext)
    	outtext=re.sub(r'\bIASetPrimitiveTopology\b', 'DrawPrimitive', outtext)

    	outtext=re.sub(r'\bD3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST\b', 'PRIM_TRIANGLES', outtext)
	outtext=re.sub(r'\bD3D10_PRIMITIVE_TOPOLOGY_LINELIST\b', 'PRIM_LINES', outtext)
	outtext=re.sub(r'\bD3D10_PRIMITIVE_TOPOLOGY_POINTLIST\b', 'PRIM_POINTS', outtext)
	outtext=re.sub(r'\bD3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP\b', 'PRIM_TRIANGLE_STRIP', outtext)
	outtext=re.sub(r'\bD3D10_PRIMITIVE_TOPOLOGY_LINESTRIP\b', 'PRIM_LINE_STRIP', outtext)

    	outtext=re.sub(r'\bD3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST\b', 'PRIM_TRIANGLES', outtext)
	outtext=re.sub(r'\bD3D11_PRIMITIVE_TOPOLOGY_LINELIST\b', 'PRIM_LINES', outtext)
	outtext=re.sub(r'\bD3D11_PRIMITIVE_TOPOLOGY_POINTLIST\b', 'PRIM_POINTS', outtext)
	outtext=re.sub(r'\bD3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP\b', 'PRIM_TRIANGLE_STRIP', outtext)
	outtext=re.sub(r'\bD3D11_PRIMITIVE_TOPOLOGY_LINESTRIP\b', 'PRIM_LINE_STRIP', outtext)

	outtext=re.sub(r'\bD3D10_PRIMITIVE_TOPOLOGY\b', 'Primitives', outtext)

	outtext=re.sub(r'^\s*\-\>', 'IRenderer::GetRendererInstance()->', outtext)

	# TEXT( "stones.bmp" )
    	#outtext=re.sub(r'\bTEXT\(\s*\"[\w\.\\\/]+\"\s*\)', '\1', outtext)
	outtext=re.sub(r'\bMeshRenderer2\s*\*', 'MeshRenderer2', outtext)
	outtext=re.sub(r'\bMeshRenderer2\s*\*', 'MeshRenderer2', outtext)
	outtext=re.sub(r'\bLPD3DXMESH\b', 'MeshRenderer2', outtext)

	outtext=re.sub(r'\bD3DXFROMWINECOLOR\b', 'D3DXFROMWINEVECTOR4', outtext)

	outtext=re.sub(r'\bCModelViewerCamera\b', 'stx_CModelViewerCamera', outtext)
	outtext=re.sub(r'\bEffectID\b', 'stx_Effect', outtext)

	outtext=re.sub(r'\bDWORD\b', '__DWORD__', outtext)
	outtext=re.sub(r'\bWORD\b', '__WORD__', outtext)
	outtext=re.sub(r'\.elem\[', '.m[', outtext)

	outtext=re.sub(r'\bfloat4x4\b', 'D3DXFROMWINEMATRIX', outtext)

	outtext=re.sub(r'\"\, \(\(D3D9Renderer \*\) IRenderer\:\:GetRendererInstance\(\)\)\-\>getD3DTexture\(', '', outtext)

	outtext=re.sub(r'\(TextureID\)\(\(\(D3D9Renderer \*\) IRenderer\:\:GetRendererInstance\(\)\)\-\>getD3DTexture\(', '', outtext)
	outtext=re.sub(r'\bmainFont.drawText', '//mainFont.drawText', outtext)
	outtext=re.sub(r'\bCGGL1_1\b', 'GLCG1_1', outtext)

	outtext=re.sub(r'\bpDevice\-\>', 'IRenderer::GetRendererInstance()->', outtext)
	outtext=re.sub(r'\bdevice\-\>', 'IRenderer::GetRendererInstance()->', outtext)
	outtext=re.sub(r'\bpContext\-\>', 'IRenderer::GetRendererInstance()->', outtext)
	outtext=re.sub(r'\bmemcpy\b', 'stx_memcpy', outtext)
	outtext=re.sub(r'\bmemset\b', 'stx_memset', outtext)

	outtext=re.sub(r'\bLPDIRECT3DSURFACE9\b', 'SurfaceID', outtext)
	outtext=re.sub(r'\bLPDIRECT3DTEXTURE9\b', 'TextureID', outtext)
	outtext=re.sub(r'\bLPDIRECT3DDEVICE9\b', 'IRenderer::GetRendererInstance()', outtext)

	outtext=re.sub(r'\bLPDIRECT3DVERTEXBUFFER9\b', 'VerteXBufferID', outtext)
	outtext=re.sub(r'\bLPDIRECT3DINDEXBUFFER9\b', 'IndexBufferID', outtext)
	outtext=re.sub(r'\bLPDIRECT3DCUBETEXTURE9\b', 'TextureID', outtext)
	outtext=re.sub(r'\bLPDIRECT3DVOLUMETEXTURE9\b', 'TextureID', outtext)

	outtext=re.sub(r'\bPDIRECT3DSURFACE9\b', 'SurfaceID', outtext)
	outtext=re.sub(r'\bPDIRECT3DTEXTURE9\b', 'TextureID', outtext)
	outtext=re.sub(r'\bPDIRECT3DDEVICE9\b', 'IRenderer::GetRendererInstance()', outtext)

	outtext=re.sub(r'\bLPDIRECT3DPIXELSHADER9\b', 'ShaderID', outtext)
	outtext=re.sub(r'\bLPDIRECT3DVERTEXSHADER9\b', 'ShaderID', outtext)
	outtext=re.sub(r'\bLPDIRECT3DVERTEXDECLARATION9\b', 'VerteXFormatID', outtext)

	outtext=re.sub(r'\bLPDIRECT3D9\b.*?\;', '', outtext)
	outtext=re.sub(r'\bIDirect3DDevice9\s*\*\s*pd3dDevice\s*\,', '', outtext)
	outtext=re.sub(r'\bIDirect3DDevice9\s*\*\s*pd3dDevice\s*\)', ')', outtext)

	outtext=re.sub(r'\bLPDIRECT3DBASETEXTURE9\b', 'TextureID', outtext)

	outtext=re.sub(r'\bIDirect3DBaseTexture9\b', 'TextureID', outtext)
	outtext=re.sub(r'\bIID_IDirect3DBaseTexture9\b', 'TextureID', outtext)

	outtext=re.sub(r'\bIDirect3DCubeTexture9\b', 'TextureID', outtext)
	outtext=re.sub(r'\bIDirect3DVertexDeclaration9\b', 'VerteXFormatID', outtext)
	outtext=re.sub(r'\bIDirect3D9\b', 'IRenderer::GetRendererInstance()', outtext)

	outtext=re.sub(r'\bIDirect3DDevice9\s*\*\s*\w+\,', '', outtext)
	outtext=re.sub(r'\bIDirect3DDevice9\s*\*\s*\w+\)', ')', outtext)
	outtext=re.sub(r'\bIDirect3DTexture9\s*\*', 'TextureID ', outtext)
	outtext=re.sub(r'\bIDirect3DSurface9\s*\*', 'SurfaceID ', outtext)

	outtext=re.sub(r'\bIDirect3DDevice9\s*\*\s*\w+\;', '', outtext)
	outtext=re.sub(r'\bIDirect3DSurfaceID\b', 'SurfaceID ', outtext)
    	outtext=re.sub(r'\bIRenderer\:\:GetRendererInstance\(\)\-\>addTexture\w', 'IRenderer::GetRendererInstance()->addTexture\(\"', outtext)

	outtext=re.sub(r'\baddTexture\\\(', 'addTexture(', outtext)
    
    	outtext=re.sub(r'\bclearBuffers\s*\(\s*COLOR\_BUFFER\s*\|\s*DEPTH\_BUFFER\s*\,\s*D3DXFROMWINEVECTOR4\b', 'Clear(true, true, D3DXFROMWINEVECTOR4', outtext)

    	outtext=re.sub(r'\bIRenderer\:\:GetRendererInstance\(\)\-\>SetTexture\b', 'IRenderer::GetRendererInstance()->setTexture', outtext)

    	outtext=re.sub(r'\bSetStreamSource\b', 'setVertexBuffer', outtext)
	outtext=re.sub(r'\bSetIndices\b', 'setIndexBuffer', outtext)
	outtext=re.sub(r'\bRELEASE\b', '//RELEASE', outtext)

	outtext=re.sub(r'\(\(D3D9Renderer\*\)IRenderer\:\:GetRendererInstance\(\)\)\-\>getD3DTexture\b', '', outtext)

	outtext=re.sub(r'\bSetRenderTarget\b', 'changeRenderTarget', outtext)
	outtext=re.sub(r'\bIRenderer\:\:GetRendererInstance\(\)\-\>SetDepthStencilSurface\b', '//IRenderer::GetRendererInstance()->SetDepthStencilSurface', outtext)
	outtext=re.sub(r'\bSetViewport\b', 'setViewport', outtext)
	outtext=re.sub(r'\bchangeShader\b', 'setShader', outtext)
		
    	outtext=re.sub(r'\bIRenderer\:\:GetRendererInstance\(\)\-\>SetTextureStageState\b', '//IRenderer::GetRendererInstance()->SetTextureStageState', outtext)
    	outtext=re.sub(r'\(IRenderer\:\:GetRendererInstance\(\)\,', '(', outtext)

    	outtext=re.sub(r'\bIRenderer\:\:GetRendererInstance\(\)\-\>setCullFace\b', '//IRenderer::GetRendererInstance()->setCullFace', outtext)
    	outtext=re.sub(r'\bIRenderer\:\:GetRendererInstance\(\)\-\>setDepthFunc\b', '//IRenderer::GetRendererInstance()->setDepthFunc', outtext)

    	outtext=re.sub(r'\bIRenderer\:\:GetRendererInstance\(\)\-\>SetPixelShader\(0\)\;', '//IRenderer::GetRendererInstance()->SetPixelShader(0);', outtext)
    	outtext=re.sub(r'\bIRenderer\:\:GetRendererInstance\(\)\-\>SetVertexShader\(0\)\;', '//IRenderer::GetRendererInstance()->SetVertexShader(0);', outtext)

    	outtext=re.sub(r'\bCreateVertexDeclaration\b', 'addVertexFormat', outtext)
    	outtext=re.sub(r'\bD3DVERTEXELEMENT9\b', 'FormatDesc', outtext)
    	outtext=re.sub(r'\bCreateVertexBuffer\b', 'addVertexFormat', outtext)

    	outtext=re.sub(r'\bD3DFMT_INDEX16\b', 'CONSTANT_INDEX2', outtext)
    	outtext=re.sub(r'\bD3DFMT_INDEX32\b', 'CONSTANT_INDEX4', outtext)

    	outtext=re.sub(r'\bCreateTexture\b', 'addTexture', outtext)

    	outtext=re.sub(r'\breturn \(hres\=\=D3D\_OK\)\;', 'return 0;', outtext)

    	outtext=re.sub(r'\bSampleApp\b', 'App', outtext)

    	outtext=re.sub(r'\(\)\)\) \=\= TEXTURE\_NONE\) return false\;', '());', outtext)

    	#outtext=re.sub(r'\bcaps.MaxTextureHeight\b', '', outtext)

    	outtext=re.sub(r'\bresetRenderTarget\b', 'changeToMainFramebuffer', outtext)

    	outtext=re.sub(r'VB\, 0\, 4\*sizeof\(unsigned short\)\)\;', 'VB);', outtext)
    	outtext=re.sub(r'\bhres \= IRenderer\:\:', 'IRenderer::', outtext)
    	outtext=re.sub(r'\bhres \|\= IRenderer\:\:', 'IRenderer::', outtext)
    	outtext=re.sub(r'\bhres \=  IRenderer\:\:', 'IRenderer::', outtext)
    	outtext=re.sub(r'\bhres\=IRenderer\:\:', 'IRenderer::', outtext)

    	outtext=re.sub(r'\bif\s*\(IRenderer\:\:', 'IRenderer::', outtext)

    	outtext=re.sub(r'\)\s*\!\=\s*\=\s*D3D\_OK\s*\)\s*\{.*?\}', ');', outtext, 0, re.M)
    	outtext=re.sub(r'\)\s*\=\=\s*\=\s*D3D\_OK\s*\)\s*\{.*?\}', ');', outtext, 0, re.M)
	
    	outtext=re.sub(r'\bhres \= \/\/', '//', outtext)

    	outtext=re.sub(r'\bhres \=  IRenderer\:\:', 'IRenderer::', outtext)
	
    	outtext=re.sub(r'\bsetBlending\(SRC\_ALPHA\, ONE\)', 'setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha())', outtext)
    	outtext=re.sub(r'\bIRenderer\:\:GetRendererInstance\(\)\-\>SetRenderState\b', '//IRenderer::GetRendererInstance()->SetRenderState', outtext)
    	outtext=re.sub(r'\bIRenderer\:\:GetRendererInstance\(\)\-\>SetSamplerState\b', '//IRenderer::GetRendererInstance()->SetSamplerState', outtext)
	outtext=re.sub(r'////////', '//', outtext)
	outtext=re.sub(r'//////', '//', outtext)
	outtext=re.sub(r'////', '//', outtext)
	# ) != D3D_OK)\{.*?\}', ');', outtext, 0, re.M)
    '''
    inctext='''/*
  Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
	'''

	inctext_='''
#include <Framework3/IRenderer.h>

struct App
{
	App(){}
	virtual ~App(){}
	'''
	maintext='''
	void render()
	{
	}	
	int init(const char* aTitle)
	{
		return 0;
	}
};
int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("");	
	IInput*    i=STX_Service::GetInputInstance();
	App app;
	app.init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		app.render();
	}
	return 0;
}
	'''

    #outtext=re.sub(r'\bcaps\.MaxTextureHeight\b', 'IRenderer::GetRendererInstance()->GetMaxTextureSize()', outtext)
	output = open(tempName,'w')
	#output.write(inctext)
	output.write(outtext)
	#output.write(maintext)
	output.close()

	os.unlink(filePath)
	os.rename(tempName,filePath)

def myfun(dummy, dirr, filess):
	for child in filess:
		if '.h' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
			replaceStringInFile(dirr+'/'+child)
			print child
	for child in filess:
		if '.cpp' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
			replaceStringInFile(dirr+'/'+child)
			print child

v=['../Libraries/Framework', '../Samples/new/new/x', '../Samples/new/new/R2VB_VTF']
v=['../Samples/new/new/x', '../Samples/new/new/R2VB_VTF']
v=['../Samples/new/new/x/R2VB_VTF']
for i in v:
	os.path.walk(i, myfun, 13)

