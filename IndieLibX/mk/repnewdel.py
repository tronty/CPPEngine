import os, sys, re

def replaceString(outtext):
    outtext=re.sub(r'\bsrand\b', 'stx_srand', outtext)
    outtext=re.sub(r'\brand\b', 'stx_rand', outtext)
    return outtext
    outtext=re.sub(r'1e\-3', '0.001', outtext)
    outtext=re.sub(r'\bfract\b', 'frac', outtext)
    outtext=re.sub(r'\bgl_FragCoord\b', 'fragCoord', outtext)
    outtext=re.sub(r'\bgl_FragColor\b', 'fragColor', outtext)
    outtext=re.sub(r'\buniform\b', '', outtext)
    outtext=re.sub(r'\bvec2\b', 'float2', outtext)
    outtext=re.sub(r'\bvec3\b', 'float3', outtext)
    outtext=re.sub(r'\bvec4\b', 'float4', outtext)
    outtext=re.sub(r'\bmat2\b', 'float2x2', outtext)
    outtext=re.sub(r'\bmat3\b', 'float3x3', outtext)
    outtext=re.sub(r'\bmat4\b', 'float4x4', outtext)
    #outtext=re.sub(r'\bOut\.uv \= In\.uv\;', 'Out.uv = 1.0-In.uv;', outtext)
    #outtext=outtext.replace('Out.uv = 1.0-In.uv;', 'Out.uv.x = In.uv.x;Out.uv.y = 1.0-In.uv.y;');
    #outtext=re.sub(r'\bapp\.initGUI\b', 'BaseApp::initGUI', outtext)
    #outtext=re.sub(r'\<SDL\/SDL\.h\>', '<SDL2/SDL.h>', outtext)
    #outtext=re.sub(r'\<SDL\/', '<SDL2/', outtext)

    #outtext=re.sub(r'\bSTX_PRINTF\b', 'printf', outtext)
    #outtext=re.sub(r'\bSTX_PRINT\b', 'printf', outtext)

    # CModelViewerCamera

    # outtext=re.sub(r'\bDXGI_FORMAT_','FORMAT_', outtext)
    #outtext=re.sub(r'\bapp\.load\(\)\;','app.load();initGUI();', outtext)

    #outtext=re.sub(r'.shd", "main", "main"','\.shd\"\, \"mainVS\"\, \"mainPS\"', outtext)

    '''
    outtext=re.sub(r'\bD3D10_RTV_DIMENSION_TEXTURE1DARRAY\b', 'RTV_DIMENSION_TEXTURE1DARRAY', outtext)
    outtext=re.sub(r'\bD3D10_RTV_DIMENSION_TEXTURE2DARRAY\b', 'RTV_DIMENSION_TEXTURE2DARRAY', outtext)
    outtext=re.sub(r'\bD3D10_RTV_DIMENSION_TEXTURE3DARRAY\b', 'RTV_DIMENSION_TEXTURE3DARRAY', outtext)
    outtext=re.sub(r'\bD3D10_SRV_DIMENSION_TEXTURE1DARRAY\b', 'SRV_DIMENSION_TEXTURE1DARRAY', outtext)
    outtext=re.sub(r'\bD3D10_SRV_DIMENSION_TEXTURE2DARRAY\b', 'SRV_DIMENSION_TEXTURE2DARRAY', outtext)
    outtext=re.sub(r'\bD3D10_SRV_DIMENSION_TEXTURE3DARRAY\b', 'SRV_DIMENSION_TEXTURE3DARRAY', outtext)

    outtext=re.sub(r'\bD3D11_RTV_DIMENSION_TEXTURE1DARRAY\b', 'RTV_DIMENSION_TEXTURE1DARRAY', outtext)
    outtext=re.sub(r'\bD3D11_RTV_DIMENSION_TEXTURE2DARRAY\b', 'RTV_DIMENSION_TEXTURE2DARRAY', outtext)
    outtext=re.sub(r'\bD3D11_RTV_DIMENSION_TEXTURE3DARRAY\b', 'RTV_DIMENSION_TEXTURE3DARRAY', outtext)
    outtext=re.sub(r'\bD3D11_SRV_DIMENSION_TEXTURE1DARRAY\b', 'SRV_DIMENSION_TEXTURE1DARRAY', outtext)
    outtext=re.sub(r'\bD3D11_SRV_DIMENSION_TEXTURE2DARRAY\b', 'SRV_DIMENSION_TEXTURE2DARRAY', outtext)
    outtext=re.sub(r'\bD3D11_SRV_DIMENSION_TEXTURE3DARRAY\b', 'SRV_DIMENSION_TEXTURE3DARRAY', outtext)

    outtext=re.sub(r'\bFORMAT_R8G8B8A8_FLOAT', 'FORMAT_RGBA8F', outtext)
    outtext=re.sub(r'\bFORMAT_R16G16B16A16_FLOAT', 'FORMAT_RGBA16F', outtext)
    outtext=re.sub(r'\bFORMAT_R32G32B32A32_FLOAT', 'FORMAT_RGBA32F', outtext)

    outtext=re.sub(r'\bFORMAT_R8G8B8A8', 'FORMAT_RGBA8', outtext)
    outtext=re.sub(r'\bFORMAT_R16G16B16A16', 'FORMAT_RGBA16', outtext)
    outtext=re.sub(r'\bFORMAT_R32G32B32A32', 'FORMAT_RGBA32', outtext)

    outtext=re.sub(r'\bD3D11_PRIMITIVE_TOPOLOGY_', 'PRIM_', outtext)
    outtext=re.sub(r'\bD3D10_PRIMITIVE_TOPOLOGY_', 'PRIM_', outtext)

    outtext=re.sub(r'\bD3D11_USAGE_DEFAULT\b', 'USAGE_DEFAULT', outtext)
    outtext=re.sub(r'\bD3D10_USAGE_DEFAULT\b', 'USAGE_DEFAULT', outtext)

    outtext=re.sub(r'\bD3D10_SRV_DIMENSION_BUFFER\b', 'SRV_DIMENSION_BUFFER', outtext)
    outtext=re.sub(r'\bD3D11_SRV_DIMENSION_BUFFER\b', 'SRV_DIMENSION_BUFFER', outtext)
    outtext=re.sub(r'\bD3D10_RTV_DIMENSION_BUFFER\b', 'RTV_DIMENSION_BUFFER', outtext)
    outtext=re.sub(r'\bD3D11_RTV_DIMENSION_BUFFER\b', 'RTV_DIMENSION_BUFFER', outtext)

    outtext=re.sub(r'\bD3D10_BIND_VERTEX_BUFFER', 'BIND_VERTEX_BUFFER', outtext)
    outtext=re.sub(r'\bD3D11_BIND_VERTEX_BUFFER', 'BIND_VERTEX_BUFFER', outtext)

    outtext=re.sub(r'\bD3D10_BIND_DEPTH_STENCIL', 'BIND_DEPTH_STENCIL', outtext)
    outtext=re.sub(r'\bD3D11_BIND_DEPTH_STENCIL', 'BIND_DEPTH_STENCIL', outtext)

    outtext=re.sub(r'\bD3D10_USAGE_IMMUTABLE\b', 'USAGE_IMMUTABLE', outtext)
    outtext=re.sub(r'\bD3D10_BIND_RENDER_TARGET\b', 'BIND_RENDER_TARGET', outtext)
    outtext=re.sub(r'\bD3D10_SRV_DIMENSION_TEXTURE1D\b', 'SRV_DIMENSION_TEXTURE1D', outtext)
    outtext=re.sub(r'\bD3D10_SRV_DIMENSION_TEXTURE2D\b', 'SRV_DIMENSION_TEXTURE2D', outtext)
    outtext=re.sub(r'\bD3D10_SRV_DIMENSION_TEXTURE3D\b', 'SRV_DIMENSION_TEXTURE3D', outtext)

    outtext=re.sub(r'\bD3D11_USAGE_IMMUTABLE\b', 'USAGE_IMMUTABLE', outtext)
    outtext=re.sub(r'\bD3D11_BIND_RENDER_TARGET\b', 'BIND_RENDER_TARGET', outtext)
    outtext=re.sub(r'\bD3D11_SRV_DIMENSION_TEXTURE1D\b', 'SRV_DIMENSION_TEXTURE1D', outtext)
    outtext=re.sub(r'\bD3D11_SRV_DIMENSION_TEXTURE2D\b', 'SRV_DIMENSION_TEXTURE2D', outtext)
    outtext=re.sub(r'\bD3D11_SRV_DIMENSION_TEXTURE3D\b', 'SRV_DIMENSION_TEXTURE3D', outtext)

    outtext=re.sub(r'\bD3D11_BIND_UNORDERED_ACCESS\b', 'BIND_UNORDERED_ACCESS', outtext)
    outtext=re.sub(r'\bD3D11_BIND_SHADER_RESOURCE\b', 'BIND_SHADER_RESOURCE', outtext)
    outtext=re.sub(r'\bD3D10_BIND_UNORDERED_ACCESS\b', 'BIND_UNORDERED_ACCESS', outtext)
    outtext=re.sub(r'\bD3D10_BIND_SHADER_RESOURCE\b', 'BIND_SHADER_RESOURCE', outtext)

    outtext=re.sub(r'\bD3D11_DEPTH_STENCIL_DESC\b.*?;', '', outtext)
    outtext=re.sub(r'\bD3D11_SUBRESOURCE_DATA\b.*?;', '', outtext)
    outtext=re.sub(r'\bD3D11_BUFFER_DESC\b.*?;', '', outtext)
    outtext=re.sub(r'\bD3D10_DEPTH_STENCIL_DESC\b.*?;', '', outtext)
    outtext=re.sub(r'\bD3D10_SUBRESOURCE_DATA\b.*?;', '', outtext)
    outtext=re.sub(r'\bD3D10_BUFFER_DESC\b.*?;', '', outtext)
    outtext=re.sub(r'\bD3D11_INPUT_ELEMENT_DESC\b', 'FormatDesc', outtext)
    outtext=re.sub(r'\bD3D10_INPUT_ELEMENT_DESC\b', 'FormatDesc', outtext)

    outtext=re.sub(r'\bD3D11_SAMPLER_DESC\b.*?;', '', outtext)
    outtext=re.sub(r'\bD3D11_RASTERIZER_DESC\b.*?;', '', outtext)
    outtext=re.sub(r'\bD3D11_BLEND_DESC\b.*?;', '', outtext)
    outtext=re.sub(r'\bD3D10_SAMPLER_DESC\b.*?;', '', outtext)
    outtext=re.sub(r'\bD3D10_RASTERIZER_DESC\b.*?;', '', outtext)
    outtext=re.sub(r'\bD3D10_BLEND_DESC\b.*?;', '', outtext)

    outtext=re.sub(r'\bD3D10_TECHNIQUE_DESC\b.*?;', '', outtext)
    outtext=re.sub(r'\bD3D11_TECHNIQUE_DESC\b.*?;', '', outtext)

    outtext=re.sub(r'\bD3D10_TEXTURE1D_DESC\b.*?;', '', outtext)
    outtext=re.sub(r'\bD3D10_TEXTURE2D_DESC\b.*?;', '', outtext)
    outtext=re.sub(r'\bD3D10_TEXTURE3D_DESC\b.*?;', '', outtext)
    outtext=re.sub(r'\bD3D11_TEXTURE1D_DESC\b.*?;', '', outtext)
    outtext=re.sub(r'\bD3D11_TEXTURE2D_DESC\b.*?;', '', outtext)
    outtext=re.sub(r'\bD3D11_TEXTURE3D_DESC\b.*?;', '', outtext)

    outtext=re.sub(r'\bD3D10_PASS_DESC\b.*?;', '', outtext)
    outtext=re.sub(r'\bD3D11_PASS_DESC\b.*?;', '', outtext)

    outtext=re.sub(r'\bD3D10_PASS_SHADER_DESC\b.*?;', '', outtext)
    outtext=re.sub(r'\bD3D11_PASS_SHADER_DESC\b.*?;', '', outtext)

    outtext=re.sub(r'\bD3D11_MAPPED_SUBRESOURCE\b.*?;', '', outtext)
    outtext=re.sub(r'\bD3D11_DEPTH_STENCIL_VIEW_DESC\b.*?;', '', outtext)

    outtext=re.sub(r'\bD3D10_MAPPED_SUBRESOURCE\b.*?;', '', outtext)
    outtext=re.sub(r'\bD3D10_DEPTH_STENCIL_VIEW_DESC\b.*?;', '', outtext)

    outtext=re.sub(r'\bpd3dImmediateContext\b','IRenderer::GetRendererInstance()', outtext)
    outtext=re.sub(r'\bpdev\b','IRenderer::GetRendererInstance()', outtext)

    outtext=re.sub(r'\bIASetInputLayout\s*\(','setVertexFormat(0, ', outtext)

    outtext=re.sub(r'ShaderID\s*\*\s*\*','ShaderID* ', outtext)
    outtext=re.sub(r'ShaderID\s*\*','ShaderID ', outtext)
    
    outtext=re.sub(r'\bstx_ShaderResourceVariableID\b','stx_ShaderResourceVariable', outtext)

    outtext=re.sub(r'\bVertexFormatID\s*\*\s*\*','VertexFormatID* ', outtext)
    outtext=re.sub(r'\bVertexFormatID\s*\*','VertexFormatID ', outtext)

    outtext=re.sub(r'\bSurfaceID\s*\*\s*\*','SurfaceID* ', outtext)
    outtext=re.sub(r'\bSurfaceID\s*\*','SurfaceID ', outtext)

    outtext=re.sub(r'\bexBufferID\s*\*\s*\*','exBufferID* ', outtext)
    outtext=re.sub(r'\bIndexBufferID\s*\*\s*\*','IndexBufferID* ', outtext)

    outtext=re.sub(r'\bexBufferID\s*\*','exBufferID ', outtext)
    outtext=re.sub(r'\bIndexBufferID\s*\*','IndexBufferID ', outtext)

    outtext=re.sub(r'\bVertexBufferID\s*\*\s*\*','VertexBufferID* ', outtext)
    outtext=re.sub(r'\bIndexBufferID\s*\*\s*\*','IndexBufferID* ', outtext)
    outtext=re.sub(r'\bTextureID\s*\*\s*\*','TextureID* ', outtext)

    outtext=re.sub(r'\bVertexBufferID\s*\*','VertexBufferID ', outtext)
    outtext=re.sub(r'\bIndexBufferID\s*\*','IndexBufferID ', outtext)
    outtext=re.sub(r'\bTextureID\s*\*','TextureID ', outtext)

    outtext=re.sub(r'\(\s*IRenderer\:\:GetRendererInstance\(\)\s*\,', '(', outtext)
    outtext=re.sub(r'\(\s*IRenderer\:\:GetRendererInstance\(\)\s*\)', '()', outtext)
    outtext=re.sub(r'\,\s*IRenderer\:\:GetRendererInstance\(\)\s*\,', ',', outtext)
    outtext=re.sub(r'\,\s*IRenderer\:\:GetRendererInstance\(\)\s*\)', ')', outtext)

    outtext=re.sub(r'\bstx_Technique\b', 'stx_Effect::stx_Technique', outtext)
    outtext=re.sub(r'\bstx_Effect::stx_Effect::Technique\b', 'stx_Effect::stx_Technique', outtext)

    outtext=re.sub(r'\(\s*ID3D10Device\s*\*\s*IRenderer\:\:GetRendererInstance\(\)\s*\,', '(', outtext)
    outtext=re.sub(r'\(\s*ID3D10Device\s*\*\s*IRenderer\:\:GetRendererInstance\(\)\s*\)', '()', outtext)
    outtext=re.sub(r'\,\s*ID3D10Device\s*\*\s*IRenderer\:\:GetRendererInstance\(\)\s*\,', '(', outtext)
    outtext=re.sub(r'\,\s*ID3D10Device\s*\*\s*IRenderer\:\:GetRendererInstance\(\)\s*\)', '()', outtext)

    outtext=re.sub(r'\(\s*ID3D11DeviceContext\s*\*\s*IRenderer\:\:GetRendererInstance\(\)\s*\,', '(', outtext)
    outtext=re.sub(r'\(\s*ID3D11DeviceContext\s*\*\s*IRenderer\:\:GetRendererInstance\(\)\s*\)', '()', outtext)
    outtext=re.sub(r'\,\s*ID3D11DeviceContext\s*\*\s*IRenderer\:\:GetRendererInstance\(\)\s*\,', '(', outtext)
    outtext=re.sub(r'\,\s*ID3D11DeviceContext\s*\*\s*IRenderer\:\:GetRendererInstance\(\)\s*\)', '()', outtext)

    outtext=re.sub(r'\(\s*ID3D11Device\s*\*\s*IRenderer\:\:GetRendererInstance\(\)\s*\,', '(', outtext)
    outtext=re.sub(r'\(\s*ID3D11Device\s*\*\s*IRenderer\:\:GetRendererInstance\(\)\s*\)', '()', outtext)
    outtext=re.sub(r'\,\s*ID3D11Device\s*\*\s*IRenderer\:\:GetRendererInstance\(\)\s*\,', '(', outtext)
    outtext=re.sub(r'\,\s*ID3D11Device\s*\*\s*IRenderer\:\:GetRendererInstance\(\)\s*\)', '()', outtext)

    outtext=re.sub(r'\bEffectID\s*\*', 'stx_Effect* ', outtext)
    outtext=re.sub(r'\bEffectID\b', 'stx_Effect* ', outtext)
    outtext=re.sub(r'\bLPWSTR\b', 'char*', outtext)
    '''
    return outtext

    outtext=re.sub(r'\bMessageBoxA\s*\(\s*0\s*\,\s*\"', 'LOG_PRINT(\"', outtext)
    outtext=re.sub(r'\bS_FALSE\b', '1', outtext)
    outtext=re.sub(r'\bLPCWSTR\b', 'char', outtext)
    outtext=re.sub(r'\bLPCSTR\b', 'char', outtext)
    outtext=re.sub(r'\bm_pD3DDevice\b', 'IRenderer::GetRendererInstance()', outtext)
    outtext=re.sub(r'\bm_pD3DContext\b', 'IRenderer::GetRendererInstance()', outtext)

    outtext=re.sub(r'\bD3DX', 'D3DXFROMWINE', outtext)
    outtext=re.sub(r'\bD3DXFROMWINEFROMWINE', 'D3DXFROMWINE', outtext)
    outtext=re.sub(r'\bTEXT\b', '',outtext)
    outtext=re.sub(r'\bTCHAR\b', 'char', outtext)
    outtext=re.sub(r'\bWCHAR\b', 'char', outtext)
    outtext=re.sub(r'\bNULL\b', '0', outtext)
    outtext=re.sub(r'\bHRESULT\b', 'int', outtext)
    outtext=re.sub(r'\bS_OK\b', '0', outtext)
    outtext=re.sub(r'\bV\s*\(', '(', outtext)
    outtext=re.sub(r'\bV_RETURN\s*\(', '(', outtext)
    outtext=re.sub(r'\bV_GET_VARIABLE_RETBOOL\s*\(', '(', outtext)
    outtext=re.sub(r'\bV_GET_VARIABLE_RET\s*\(', '(', outtext)

    outtext=re.sub(r'\bLPCTSTR\b', 'std::string', outtext)
    outtext=re.sub(r'\bL\"', '\"', outtext)
    outtext=re.sub(r'\s*CALLBACK\s*', ' ', outtext)
    outtext=re.sub(r'\bSAFE_RELEASE\b', '//SAFE_RELEASE', outtext)
    #outtext=re.sub(r'\bSAFE_DELETE\b', '', outtext)

    outtext=re.sub(r'\bUINT\b', 'unsigned int', outtext)
    
    cr='''/*
\tCopyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/
\tLicensed under The GNU Lesser General Public License, version 2.1:
\t\thttp://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
'''
    if outtext.find("wWinMain")>0:
        cr+='''
class App: public STXGUI
{
public:
    App(){}
    virtual ~App(){}
    int init(const char* aTitle){STXGUI::init("/GUILayout.xml");}
    void render( ){STXGUI::update();}
'''
    main=''
    if (outtext.find("wWinMain")>0) or (outtext.find("WinMain")>0):
        main+='''
virtual void actionPerformed(GUIEvent &evt)
{
  const NSString &callbackString  = evt.getCallbackString();
  GUIRectangle *sourceRectangle = evt.getEventSource();
  int widgetType      = sourceRectangle->getWidgetType();

  if(widgetType == RADIO_BUTTON)
  {
    GUIRadioButton   *radioButton = (GUIRadioButton*)sourceRectangle;
    if(radioButton->isPressed() && radioButton->isChecked())
    {
		if(callbackString == "")
      	{
		}
	}
	}
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

    txt=cr+outtext+main
    txt = txt.replace("wWinMain", "")
    txt = txt.replace("WinMain", "")
    txt=outtext
    return txt

def replaceStringInshdFile(filePath):
    "replaces all string by a regex substitution"
    tempName=filePath+'~~~'
    input = open(filePath)
    outtext=input.read()
    input.close()
    outtext=replaceString(outtext)

    output = open(tempName,'w')
    output.write(outtext)
    output.close()
    os.unlink(filePath)
    os.rename(tempName,filePath)

def replaceStringInmemFile(filePath):
    "replaces all string by a regex substitution"
    tempName=filePath+'~~~'
    input = open(filePath)
    outtext=input.read()
    input.close()
    #outtext=replaceString(outtext)
    if len(re.findall(r"\bmemcpy\b", outtext))+len(re.findall(r"\bmemset\b", outtext)):
    	print filePath
    	print re.findall(r"\bmemcpy\b", outtext)
    	print re.findall(r"\bmemset\b", outtext)
    return
    outtext=re.sub(r'\bmemcpy\b', 'stx_memcpy', outtext)
    outtext=re.sub(r'\bmemset\b', 'stx_memset', outtext)
    output = open(tempName,'w')
    output.write(outtext)
    output.close()
    os.unlink(filePath)
    os.rename(tempName,filePath)

def myfunmem(dummy, dirr, filess):
	#print dirr+'/'+child
	for child in filess:
	    #print child
	    
	    if '.shd' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
	    	    replaceStringInmemFile(dirr+'/'+child)

def myfunshd(dummy, dirr, filess):
	#print dummy, dirr, filess
	for child in filess:
	    print child
	    
	    if '.shd' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
	    	    print dirr+'/'+child
	    	    replaceStringInshdFile(dirr+'/'+child)

def replaceStringInDX8File(filePath):
    "replaces all string by a regex substitution"
    tempName=filePath+'~~~'
    input = open(filePath)
    outtext=input.read()
    input.close()
    outtext=replaceString(outtext)

    #outtext=re.sub(r'\b\b', '', outtext)

    outtext=re.sub(r'\bLPDIRECT3DDEVICE8\b', '', outtext)
    outtext=re.sub(r'\bLPDIRECT3DVERTEXBUFFER8\b', 'VertexBufferID', outtext)
    outtext=re.sub(r'\bLPDIRECT3DTEXTURE8\b', 'TextureID', outtext)

    output = open(tempName,'w')
    output.write(outtext)
    output.close()
    os.unlink(filePath)
    os.rename(tempName,filePath)

def replaceStringInDX9File(filePath):
    "replaces all string by a regex substitution"
    tempName=filePath+'~~~'
    input = open(filePath)
    outtext=input.read()
    input.close()
    outtext=replaceString(outtext)

    #outtext=re.sub(r'\b\b', '', outtext)

    outtext=re.sub(r'\bLPDIRECT3DDEVICE9\b', '', outtext)
    outtext=re.sub(r'\bLPDIRECT3DVERTEXDECLARATION9\b', 'VertexFormatID', outtext)
    outtext=re.sub(r'\bLPDIRECT3DVERTEXBUFFER9\b', 'VertexBufferID', outtext)
    outtext=re.sub(r'\bLPDIRECT3DINDEXBUFFER9\b', 'IndexBufferID', outtext)
    outtext=re.sub(r'\bLPDIRECT3DTEXTURE9\b', 'TextureID', outtext)
    outtext=re.sub(r'\bLPDIRECT3DCUBETEXTURE9\b', 'TextureID', outtext)
    outtext=re.sub(r'\bLPDIRECT3DSURFACE9\b', 'SurfaceID', outtext)

    outtext=re.sub(r'\bLPDIRECT3DPIXELSHADER9\b', 'ShaderID', outtext)
    outtext=re.sub(r'\bLPDIRECT3DVERTEXSHADER9\b', 'ShaderID', outtext)
    #outtext=re.sub(r'\bD3DCOLOR\b', '', outtext)

    output = open(tempName,'w')
    output.write(outtext)
    output.close()
    os.unlink(filePath)
    os.rename(tempName,filePath)

def replaceStringInDX10File(filePath):
    "replaces all string by a regex substitution"
    tempName=filePath+'~~~'
    input = open(filePath)
    outtext=input.read()
    input.close()
    outtext=replaceString(outtext)

    #outtext=re.sub(r'\b\b', '', outtext)
    '''
( ID3D10Device* IRenderer::GetRendererInstance(), 
LPCWSTR file,LPCSTR shaderTextureName, ID3D10Device* IRenderer::GetRendererInstance()
(IRenderer::GetRendererInstance(), 
(ID3D10Device* IRenderer::GetRendererInstance())
(ID3D11Device* IRenderer::GetRendererInstance(), ID3D11DeviceContext* IRenderer::GetRendererInstance(), 
(ID3D11Device* IRenderer::GetRendererInstance(), ID3D11DeviceContext *IRenderer::GetRendererInstance())
ID3DX10MeshBuffer\b', 'exBuffer', outtext)
ID3DX11EffectTechnique
(ID3D11Device* IRenderer::GetRendererInstance())
(ID3D11Device* IRenderer::GetRendererInstance(), ID3D11DeviceContext* IRenderer::GetRendererInstance())
    '''

    outtext=re.sub(r'\bID3D10Device* pd3dDevice\s*\)', ')', outtext)
    outtext=re.sub(r'\(\s*ID3D10DeviceContext\s*\*\s*pd3dContext\s*\,\s*', '(', outtext)
    outtext=re.sub(r'\bpd3dDevice\b', 'IRenderer::GetRendererInstance()', outtext)
    outtext=re.sub(r'\bpd3dContext\b', 'IRenderer::GetRendererInstance()', outtext)
    outtext=re.sub(r'\(ID3D10Device* pd3dDevice, ID3D10DeviceContext* pd3dContext\b', '(', outtext)
    outtext=re.sub(r'\, void* pUserContext\)', ')', outtext)

    #outtext=re.sub(r'\bIASetPrimitiveTopology\b', '', outtext)

    outtext=re.sub(r'\bIASetVertexBuffers\b', 'setVertexBuffer', outtext)
    outtext=re.sub(r'\bIASetIndexBuffer\b', 'setIndexBuffer', outtext)

    outtext=re.sub(r'\bCModelViewerCamera\s+g_Camera\s*\;', 'D3DXFROMWINEMATRIX g_CameraEyePt;\nD3DXFROMWINEMATRIX g_CameraProj;\nD3DXFROMWINEMATRIX g_CameraView;\nD3DXFROMWINEMATRIX g_CameraWorld;\n', outtext)

    outtext=re.sub(r'\bCDXUTSDKMesh\b', 'MeshRenderer2', outtext)
    outtext=re.sub(r'\bCDXUTMesh10\b', 'MeshRenderer2', outtext)

	#PRIM_TRIANGLE_FAN
	#PRIM_QUADS
	#PRIM_LINE_LOOP

    outtext=re.sub(r'\bD3D10_PRIMITIVE_TOPOLOGY_LINELIST_ADJ\b', 'PRIM_LINES /* _ADJ */', outtext)
    outtext=re.sub(r'\bD3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST\b', 'PRIM_TRIANGLES', outtext)
    #outtext=re.sub(r'\bD3D10_PRIMITIVE_TOPOLOGY_UNDEFINED\b', 'PRIM_', outtext)
    outtext=re.sub(r'\bD3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP\b', 'PRIM_TRIANGLE_STRIP', outtext)
    outtext=re.sub(r'\bD3D10_PRIMITIVE_TOPOLOGY_LINELIST\b', 'PRIM_LINES', outtext)
    outtext=re.sub(r'\bD3D10_PRIMITIVE_TOPOLOGY_LINESTRIP\b', 'PRIM_LINE_STRIP', outtext)
    outtext=re.sub(r'\bD3D10_PRIMITIVE_TOPOLOGY_POINTLIST\b', 'PRIM_POINTS', outtext)

    outtext=re.sub(r'\bID3D10RasterizerState\b', 'RasterizerStateID', outtext)
    #outtext=re.sub(r'\bD3DX10_PASS_DESC\b', '', outtext)
    outtext=re.sub(r'\bID3D10RenderTargetView\b', 'TextureID', outtext)
    outtext=re.sub(r'\bID3D10DepthStencilView\b', 'TextureID', outtext)
    outtext=re.sub(r'\bID3D10Buffer\b', 'exBufferID', outtext)
    outtext=re.sub(r'\bID3D10ShaderResourceView\b', 'TextureID', outtext)
    outtext=re.sub(r'\bID3D10SamplerState\b', 'SamplerStateID', outtext)
    outtext=re.sub(r'\bD3D10_RENDER_TARGET_VIEW_DESC\b', 'SURFACE_DESC', outtext)
    outtext=re.sub(r'\bID3D10ComputeShader\b', 'ComputeShaderID', outtext)

    #outtext=re.sub(r'\bID3D10UnorderedAccessView\b', '', outtext)
    #outtext=re.sub(r'\bD3D10_INPUT_ELEMENT_DESC\b', '', outtext)

    outtext=re.sub(r'\bID3DX10Effect\b', 'EffectID', outtext)
    outtext=re.sub(r'\bID3D10Effect\b', 'EffectID', outtext)

    outtext=re.sub(r'\b ID3DX10EffectTechnique\b', 'stx_Technique', outtext)
    outtext=re.sub(r'\bID3DX10EffectScalarVariable\b', 'stx_Variable', outtext)
    outtext=re.sub(r'\bID3DX10EffectVectorVariable\b', 'stx_Variable', outtext)
    outtext=re.sub(r'\bID3DX10EffectShaderResourceVariable\b', 'stx_Variable', outtext)
    outtext=re.sub(r'\bID3DX10EffectMatrixVariable\b', 'stx_Variable', outtext)
    outtext=re.sub(r'\bID3D10Texture1D\b', 'TextureID', outtext)
    outtext=re.sub(r'\bID3D10Texture2D\b', 'TextureID', outtext)
    outtext=re.sub(r'\bID3D10Texture3D\b', 'TextureID', outtext)
    outtext=re.sub(r'\bD3D10_VIEWPORT\b', 'D3DFROMWINEVIEWPORT9', outtext)
    outtext=re.sub(r'\bD3D10_TEXTURE2D_DESC\b', 'SURFACE_DESC', outtext)
    outtext=re.sub(r'\bID3D10InputLayout*\b', 'VertexFormatID', outtext)

    outtext=re.sub(r'\bID3D10EffectScalarVariable\b', 'stx_Variable', outtext)
    outtext=re.sub(r'\bID3D10EffectVectorVariable\b', 'stx_Variable', outtext)
    outtext=re.sub(r'\bID3D10EffectMatrixVariable\b', 'stx_Variable', outtext)
    outtext=re.sub(r'\bID3D10EffectTechnique\b', 'stx_Technique', outtext)

    output = open(tempName,'w')
    output.write(outtext)
    output.close()
    os.unlink(filePath)
    os.rename(tempName,filePath)

def replaceStringInDX11File(filePath):
    "replaces all string by a regex substitution"
    tempName=filePath+'~~~'
    input = open(filePath)
    outtext=input.read()
    input.close()
    outtext=replaceString(outtext)

    #outtext=re.sub(r'\b\b', '', outtext)

    outtext=re.sub(r'\bID3D11BlendState\s*\*', 'BlendStateID', outtext)
    outtext=re.sub(r'\bID3D11DepthStencilState\s*\*', 'DepthStateID', outtext)
    outtext=re.sub(r'\bID3D11VertexShader\s*\*', 'VertexShaderID', outtext)
    outtext=re.sub(r'\bID3D11GeometryShader\s*\*', 'GeometryShaderID', outtext)
    outtext=re.sub(r'\bID3D11PixelShader\s*\*', 'PixelShaderID', outtext)

    outtext=re.sub(r'\bID3D11Device* pd3dDevice\s*\)', ')', outtext)
    outtext=re.sub(r'\(\s*ID3D11DeviceContext\s*\*\s*pd3dContext\s*\,\s*', '(', outtext)
    outtext=re.sub(r'\(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dContext\b', '(', outtext)
    outtext=re.sub(r'\bpd3dDevice\b', 'IRenderer::GetRendererInstance()', outtext)
    outtext=re.sub(r'\bpd3dContext\b', 'IRenderer::GetRendererInstance()', outtext)
    outtext=re.sub(r'\, void* pUserContext\)', ')', outtext)

    #outtext=re.sub(r'\bIASetPrimitiveTopology\b', '', outtext)

    outtext=re.sub(r'\bIASetVertexBuffers\b', 'setVertexBuffer', outtext)
    outtext=re.sub(r'\bIASetIndexBuffer\b', 'setIndexBuffer', outtext)

    outtext=re.sub(r'\bCModelViewerCamera\s+g_Camera\s*\;', 'D3DXFROMWINEMATRIX g_CameraEyePt;\nD3DXFROMWINEMATRIX g_CameraProj;\nD3DXFROMWINEMATRIX g_CameraView;\nD3DXFROMWINEMATRIX g_CameraWorld;\n', outtext)

    outtext=re.sub(r'\bCDXUTSDKMesh\b', 'MeshRenderer2', outtext)
    outtext=re.sub(r'\bCDXUTMesh11\b', 'MeshRenderer2', outtext)

	#PRIM_TRIANGLE_FAN
	#PRIM_QUADS
	#PRIM_LINE_LOOP

    outtext=re.sub(r'\bD3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ\b', 'PRIM_LINES /* _ADJ */', outtext)
    outtext=re.sub(r'\bD3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST\b', 'PRIM_TRIANGLES', outtext)
    #outtext=re.sub(r'\bD3D11_PRIMITIVE_TOPOLOGY_UNDEFINED\b', 'PRIM_', outtext)
    outtext=re.sub(r'\bD3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP\b', 'PRIM_TRIANGLE_STRIP', outtext)
    outtext=re.sub(r'\bD3D11_PRIMITIVE_TOPOLOGY_LINELIST\b', 'PRIM_LINES', outtext)
    outtext=re.sub(r'\bD3D11_PRIMITIVE_TOPOLOGY_LINESTRIP\b', 'PRIM_LINE_STRIP', outtext)
    outtext=re.sub(r'\bD3D11_PRIMITIVE_TOPOLOGY_POINTLIST\b', 'PRIM_POINTS', outtext)

    outtext=re.sub(r'\bID3D11RasterizerState\b', 'RasterizerStateID', outtext)
    #outtext=re.sub(r'\bD3DX11_PASS_DESC\b', '', outtext)
    outtext=re.sub(r'\bID3D11RenderTargetView\b', 'TextureID', outtext)
    outtext=re.sub(r'\bID3D11DepthStencilView\b', 'TextureID', outtext)
    outtext=re.sub(r'\bID3D11Buffer\b', 'exBufferID', outtext)
    outtext=re.sub(r'\bID3D11ShaderResourceView\b', 'TextureID', outtext)
    outtext=re.sub(r'\bID3D11SamplerState\b', 'SamplerStateID', outtext)
    outtext=re.sub(r'\bD3D11_RENDER_TARGET_VIEW_DESC\b', 'SURFACE_DESC', outtext)
    outtext=re.sub(r'\bID3D11ComputeShader\b', 'ComputeShaderID', outtext)

    #outtext=re.sub(r'\bID3D11UnorderedAccessView\b', '', outtext)
    #outtext=re.sub(r'\bD3D11_INPUT_ELEMENT_DESC\b', '', outtext)

    outtext=re.sub(r'\bID3DX11Effect\b', 'EffectID', outtext)
    outtext=re.sub(r'\bID3D11Effect\b', 'EffectID', outtext)

    outtext=re.sub(r'\b ID3DX11EffectTechnique\b', 'stx_Technique', outtext)
    outtext=re.sub(r'\bID3DX11EffectScalarVariable\b', 'stx_Variable', outtext)
    outtext=re.sub(r'\bID3DX11EffectVectorVariable\b', 'stx_Variable', outtext)
    outtext=re.sub(r'\bID3DX11EffectShaderResourceVariable\b', 'stx_Variable', outtext)
    outtext=re.sub(r'\bID3DX11EffectMatrixVariable\b', 'stx_Variable', outtext)
    outtext=re.sub(r'\bID3D11Texture1D\b', 'TextureID', outtext)
    outtext=re.sub(r'\bID3D11Texture2D\b', 'TextureID', outtext)
    outtext=re.sub(r'\bID3D11Texture3D\b', 'TextureID', outtext)
    outtext=re.sub(r'\bD3D11_VIEWPORT\b', 'D3DFROMWINEVIEWPORT9', outtext)
    outtext=re.sub(r'\bD3D11_TEXTURE2D_DESC\b', 'SURFACE_DESC', outtext)
    outtext=re.sub(r'\bID3D11InputLayout*\b', 'VertexFormatID', outtext)

    output = open(tempName,'w')
    output.write(outtext)
    output.close()
    os.unlink(filePath)
    os.rename(tempName,filePath)

def replaceStringInGLFile(filePath):
    "replaces all string by a regex substitution"
    tempName=filePath+'~~~'
    input = open(filePath)
    outtext=input.read()
    input.close()
    outtext=replaceString(outtext)

    output = open(tempName,'w')
    output.write(outtext)
    output.close()
    os.unlink(filePath)
    os.rename(tempName,filePath)

def replaceStringInFile_(filePath):
	"replaces all string by a regex substitution"
	tempName=filePath+'~~~'
	input = open(filePath)
	text=input.read()
	input.close()
    	#text=replaceString(text)
	
	if True:
		regex = r"\bTommi Roenty\b"
		person = re.findall(regex, text)
		if True:#len(person)>1:
			print "gedit " + filePath + "&"
			#print len(person)
	else:
		txt_='''/*\n  Copyright (c) 2020 Tommi Roenty   http://www.tommironty.fi/\n  Licensed under The GNU Lesser General Public License, version 2.1:\n      http://opensource.org/licenses/LGPL-2.1\n*/\n'''
		outtext = text.replace(txt_, "")

		output = open(tempName,'w')
		output.write(outtext)
		output.close()
		os.unlink(filePath)
		os.rename(tempName,filePath)

def replaceStringInFile(filePath):
    "replaces all string by a regex substitution"
    tempName=filePath+'~~~'
    input = open(filePath)
    outtext=input.read()
    input.close()

    #outtext=re.sub(r'\b\b', '',outtext)
    outtext=replaceString(outtext)

    output = open(tempName,'w')
    output.write(outtext)
    output.close()
    os.unlink(filePath)
    os.rename(tempName,filePath)
    print filePath

def myfunDX8(dummy, dirr, filess):
	for child in filess:
		if '.h' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
			replaceStringInDX8File(dirr+'/'+child)
			print child
	for child in filess:
		if '.cpp' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
			replaceStringInDX8File(dirr+'/'+child)
			print child

def myfunDX9(dummy, dirr, filess):
	for child in filess:
		if '.h' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
			replaceStringInDX9File(dirr+'/'+child)
			print child
	for child in filess:
		if '.cpp' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
			replaceStringInDX9File(dirr+'/'+child)
			print child

def myfunDX10(dummy, dirr, filess):
	for child in filess:
		if '.h' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
			replaceStringInDX10File(dirr+'/'+child)
			print child
	for child in filess:
		if '.cpp' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
			replaceStringInDX10File(dirr+'/'+child)
			print child

def myfunDX11(dummy, dirr, filess):
	for child in filess:
		if '.h' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
			replaceStringInDX11File(dirr+'/'+child)
			print child
	for child in filess:
		if '.cpp' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
			replaceStringInDX11File(dirr+'/'+child)
			print child

def myfunGL(dummy, dirr, filess):
	for child in filess:
		if '.h' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
			replaceStringInGLFile(dirr+'/'+child)
			print child
	for child in filess:
		if '.cpp' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
			replaceStringInGLFile(dirr+'/'+child)
			print child

def myfun(dummy, dirr, filess):
	for child in filess:
		if '.cpp' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
			replaceStringInFile(dirr+'/'+child)
		if '.cxx' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
			replaceStringInFile(dirr+'/'+child)
		if '.cc' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
			replaceStringInFile(dirr+'/'+child)
		if '.c' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
			replaceStringInFile(dirr+'/'+child)
		if '.h' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
			replaceStringInFile(dirr+'/'+child)
		if '.hxx' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
			replaceStringInFile(dirr+'/'+child)
		if '.inl' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
			replaceStringInFile(dirr+'/'+child)

def myfun_(dummy, dirr, filess):
	for child in filess:
		if '.cpp' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
			replaceStringInFile_(dirr+'/'+child)
			#print child
		if '.h' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
			replaceStringInFile_(dirr+'/'+child)
			#print child

def myfunmem(dummy, dirr, filess):
	for child in filess:
		if '.cpp' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
			replaceStringInmemFile(dirr+'/'+child)
		if '.cc' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
			replaceStringInmemFile(dirr+'/'+child)
		if '.c' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
			replaceStringInmemFile(dirr+'/'+child)
		if '.h' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
			replaceStringInmemFile(dirr+'/'+child)
		if '.hxx' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
			replaceStringInmemFile(dirr+'/'+child)
		if '.inl' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
			replaceStringInmemFile(dirr+'/'+child)

os.path.walk('../Samples', myfun, 13)
#os.path.walk('../../IndieLib_resources/www.shadertoy.com/www.shadertoy.com/www.shadertoy.com', myfunshd, 13)
#os.path.walk('../Samples/new/new/new/javagames/src/Pacman', myfun, 13)
#os.path.walk('../Samples/new/new_/R2VB_VTF', myfun, 13)
exit(0)

v1=['spine-2.3', 'c3dbool', 'masters-grammar-interpreter', 'STX', 'ContentStreaming', 'MeshRenderer2', 'MeshRenderer3', 'Framework', 'NatureSceneLibrary', 'Framework3', 'nova-0.15.0', 'FW3', 'NSGUI', 'otl', 'otl_', 'otl-nightly', 'RadeonTerrainDemoLibrary', 'Image3', 'sdkmesh-to-obj', 'IndieLib']
#for i in v1:
os.path.walk('../Libraries/otl_', myfun, 13)
os.path.walk('../Libraries/otl-nightly', myfun, 13)
#os.path.walk('../Samples', myfun, 13)
exit(0)

v1=['spine-2.3', 'c3dbool', 'masters-grammar-interpreter', 'STX', 'ContentStreaming', 'MeshRenderer2', 'MeshRenderer3', 'Framework', 'NatureSceneLibrary', 'Framework3', 'nova-0.15.0', 'FW3', 'NSGUI', 'otl', 'otl_', 'otl-nightly', 'RadeonTerrainDemoLibrary', 'Image3', 'sdkmesh-to-obj', 'IndieLib']
for i in v1:
	os.path.walk('../Libraries/'+i, myfunmem, 13)
os.path.walk('../Samples', myfunmem, 13)
exit(0)

if False:
	os.system("rm -fr ../Samples/new/new/DX11/Hair")
	os.system("rm -fr ../Samples/new/new/DX10/FurShellsAndFins")
	os.system("cp -fr ../Samples/new/new/DX11/Hair_ ../Samples/new/new/DX11/Hair")
	os.system("cp -fr ../Samples/new/new/DX10/FurShellsAndFins_ ../Samples/new/new/DX10/FurShellsAndFins")

	DX11=['../Samples/new/new/DX11/Hair']#['../Samples/new/new/DX11/DetailTessellation11', '../Samples/new/new/DX11/OceanCS', '../Samples/new/new/new/DX11/nBodyCS', '../Samples/new/new/new/DX11/NBodyGravityCS11']
	DX10=['../Samples/new/new/DX10/FurShellsAndFins']#['../Samples/new/new/DX10/Global_Illumination', '../Samples/new/new/DX10/GrassInstancing', '../Samples/new/new/DX10/Skinning10', '../Samples/new/new/new/DX10/Rain', '../Samples/new/new/new/DX10/NBodyGravity', '../Samples/new/new/DX10/SkinAnimation']
	DX9=[]#['../Samples/new/new/DX9/HDR', '../Samples/new/new/DX9/HDRCubeMap', '../Samples/new/new/DX9/HDRFormats', '../Samples/new/new/DX9/HDRLighting', '../Samples/new/new/DX9/HDRPipeline']

	DX11=['../Samples/new__/NBody/DX11/NBodyGravityCS11', '../Samples/new__/NBody/DX11/nBodyCS']
	DX10=['../Samples/new__/NBody/DX10/NBodyGravity', '../Samples/new/new/DX10/SkinAnimation']

	DX10=['../Samples/Smoke', '../Samples/PerlinFire']
	DX11=['../Samples/new/new__/NBody/DX11/NBodyGravityCS11', '../Samples/new/new__/NBody/DX11/nBodyCS']

'''
Files=['../Libraries/Framework3']
Files=['../Libraries/IndieLib/IndieLib/Common']
v=['../../IndieLib_resources/www.shadertoy.com']
for i in v:
	os.path.walk(i, myfunshd, 13)
exit(0)

for i in Files:
	os.path.walk(i, myfun, 13)
for i in GL:
	os.path.walk(i, myfunGL, 13)
'''

exit(0)
DX_=['../Samples/new']
for i in DX_:
	os.path.walk(i, myfun, 13)
exit(0)

GL=[]#['../Samples/new_/R2VB_VTF/18', '../Samples/new/new/GL/ParallaxMappingOpenGL', '../Samples/new/new/GL/hdr']
DX8=['../Samples/20-Explosions/Ch20p3_ComplexExplosion'] # LPDIRECT3DVERTEXBUFFER8
DX9=['../Samples/new/new/DX9/HLSL_Instancing', '../Samples/new/new/DX9/VertexTextureFetchWater'] # LPDIRECT3DVERTEXBUFFER9
#DX10=['../Samples/new', '../Samples/Smoke', '../Samples/DeferredParticles', '../Samples/PerlinFire']

DX10=['../Samples/new']

#for i in DX8:
#	os.path.walk(i, myfunDX8, 13)
#for i in DX9:
#	os.path.walk(i, myfunDX9, 13)
for i in DX10:
	os.path.walk(i, myfunDX10, 13)
#for i in DX11:
#	os.path.walk(i, myfunDX11, 13)

