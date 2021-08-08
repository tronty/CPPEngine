#include "SceneFrame.h"
#include "Terrain/SkyMesh.h"

SceneFrame::SceneFrame()
{
  skyInfo.set(0.25f, 0.5f, 1.0f, 1.0f);

  renderSky = true;
  renderWater  =  true;
  active       =  true;
  paused       = false;
  renderTextures= false;
  STXGUI::init("/NatureScene/GUILayout.xml");
  camera=Camera3(Camera3::LANDOBJECT);
  camera.updateClipPlanes(4.0f, 30000.0f);//7500.0f);
  D3DXFROMWINEVECTOR3 eye(-510.0f, 68.0f, -2210.0f);
  D3DXFROMWINEVECTOR3 at(652.0f, 95.0f, 1670.0f);
  D3DXFROMWINEVECTOR3 up(0.0f,  1.0f,    0.0f);
  camera.setPosition(&eye);
  camera.setLook(&at);
  camera.setUp(&up);
}

bool SceneFrame::initialize()
{
  srand((unsigned) time( 0));

  setPerspective(0, 0, IRenderer::GetRendererInstance()->GetViewportWidth(), IRenderer::GetRendererInstance()->GetViewportHeight());
#ifdef USEGROUPS
  Image3 img;
  img.create(FORMAT_RGBA8, 512, 512, 1, 1);
  SamplerStateID ss=IRenderer::GetRendererInstance()->Getlinear();
  waterTexture=IRenderer::GetRendererInstance()->addTexture(&img, true, ss);

  m_pWaterMesh=new WaterMesh();
  if(m_pWaterMesh) m_pWaterMesh->init();
  waterShader = IRenderer::GetRendererInstance()->addShaderFromFile("/NatureScene/WaterShader.shd", "main", "main");
	{FormatDesc format[] =
	{
		0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
		0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	waterVF = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), waterShader);}
  Image3 imgn;
  ubyte* ptrn=imgn.create(FORMAT_RGBA8, 256, 256, 1, 1);
	    for (int x = 0; x < 256; x++)
	    {
		for (int y = 0; y < 256; y++)
		{
		    ptrn[4*(x + (y * 256))] = 255.0f;
		    ptrn[4*(x + (y * 256))+1] = 255.0f;
		    ptrn[4*(x + (y * 256))+2] = 255.0f;
		    ptrn[4*(x + (y * 256))+3] = 255.0f;

		}
	    }
  normalmap=IRenderer::GetRendererInstance()->addTexture(&imgn, false, ss);

  m_pSkyMesh=new SkyMesh();
  if(m_pSkyMesh) m_pSkyMesh->init();
  skyTexture=IRenderer::GetRendererInstance()->addImageLibTexture("/NatureScene/Sky/clouds.png", false, ss);
  skyShader = IRenderer::GetRendererInstance()->addShaderFromFile("/NatureScene/SkyShader.shd", "main", "main");
	{FormatDesc format[] =
	{
					0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
					0, TYPE_NORMAL,   FORMAT_FLOAT, 3,
					0, TYPE_BINORMAL, FORMAT_FLOAT, 3,
					0, TYPE_TANGENT,  FORMAT_FLOAT, 3,
					0, TYPE_TEXCOORD, FORMAT_FLOAT, 3,
					0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	skyVF = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), skyShader);}
#endif
  
  terrain.initialize(true);
  waterDepth = IRenderer::GetRendererInstance()->addImageLibTexture("/NatureScene/Terrain/watermap.jpg", false, IRenderer::GetRendererInstance()->Getlinear());

if(1){
	std::string fn=stx_convertpath("/Framework3/Framework3.xml");
    TiXmlDocument doc(fn.c_str());
    if (!doc.LoadFile())
	{//LOG_PRINT("!doc.LoadFile(%s)\n", fn.c_str());
      stx_exit(0);}
    TiXmlHandle docHandle(&doc);
    std::string Str = docHandle.FirstChild("Body").FirstChild("Debug").Element()->Attribute("Value");
	if(Str!="False")
	{
		
  renderSky = true;
  renderWater  =  false;
	}}

  return (waterDepth>-1);
}

void SceneFrame::updateClipPlanes(float nearClip, float farClip)
{
  if(0)
  	camera.updateClipPlanes(nearClip, farClip);
  else
  	camera.updateClipPlanes(0.001f, 30000.0f);
}

bool SceneFrame::run()
{
	if(renderTextures)
	{
		DrawAtlas();
		return true;
	}
  static  float privateTimeClock = 0.0f;
  D3DXFROMWINEVECTOR4 sceneInfo;
  float   frameInterval          = 0.0f;//benchmark.getFrameInterval();
  int     renderedTriangles      = 0,
          waterMeshTri           = 0;

  //benchmark.markFrameStart();
#ifdef USEGROUPS
  if(renderWater)
	updateWaterSurface(privateTimeClock);
#endif
  setPerspective(0, 0, IRenderer::GetRendererInstance()->GetViewportWidth(), IRenderer::GetRendererInstance()->GetViewportHeight());

  terrain.adjustCameraYLocation(&camera, 60.0f);
  camera.update();
  frustum.update();

  D3DXFROMWINEVECTOR3 up(0.0f,  1.0f,    0.0f);
  if(STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_F1))
  {
	D3DXFROMWINEVECTOR3 eye(510.0f, 68.0f, 2210.0f);
  	D3DXFROMWINEVECTOR3 at(652.0f, 95.0f, 1670.0f);
  	camera.setPosition(&eye);
  	camera.setLook(&at);
  	camera.setUp(&up);
  }
  else if(STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_F2))
  {
	D3DXFROMWINEVECTOR3 eye(2210.0f, 68.0f, 510.0f);
  	D3DXFROMWINEVECTOR3 at(652.0f, 95.0f, 1670.0f);
  	camera.setPosition(&eye);
  	camera.setLook(&at);
  	camera.setUp(&up);
  }
  else if(STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_F3))
  {
	D3DXFROMWINEVECTOR3 eye(-510.0f, 68.0f, -2210.0f);
  	D3DXFROMWINEVECTOR3 at(652.0f, 95.0f, 1670.0f);
  	camera.setPosition(&eye);
  	camera.setLook(&at);
  	camera.setUp(&up);
  }
  else if(STX_Service::GetInputInstance()->OnKeyPress(STX_KEY_F4))
  {
	D3DXFROMWINEVECTOR3 eye(-2210.0f, 68.0f, -510.0f);
  	D3DXFROMWINEVECTOR3 at(652.0f, 95.0f, 1670.0f);
  	camera.setPosition(&eye);
  	camera.setLook(&at);
  	camera.setUp(&up);
  }
  sceneInfo    = terrain.render(camera, frustum, privateTimeClock);
#ifdef USEGROUPS
  if(renderWater)
	waterMeshTri = renderWaterSurface(privateTimeClock);
  sceneInfo.x += renderSkyDome();
#endif
#if 1

	sceneInfo.x += waterMeshTri;
    cellCounter  = (GUILabel*)guiFrame->getWidgetByCallbackString("cellCounter");
    triCounter   = (GUILabel*)guiFrame->getWidgetByCallbackString("triCounter");
    fpsCounter   = (GUILabel*)guiFrame->getWidgetByCallbackString("fpsCounter");
	char buf[256];
	
  if(triCounter){stx_snprintf(buf, 256, "Triangle count: %d\n", (int)sceneInfo.x);triCounter ->setLabelString(buf);}
	
  if(cellCounter){stx_snprintf(buf, 256, "Visible Cells: %d\n", (int)sceneInfo.y);cellCounter->setLabelString(buf);}
	
  if(fpsCounter)
{    
    static float framesPerSecond    = 0.0f;       // This will store our fps
    static float lastTime   = 0.0f;       // This will hold the time from the last frame
    float currentTime = timeGetTime() * 0.001f;    
    ++framesPerSecond;
    if( currentTime - lastTime > 1.0f )
    {
        lastTime = currentTime;
		stx_snprintf(buf, 256, "Current FPS: %d\n", (int)framesPerSecond );
		fpsCounter ->setLabelString(buf);
        framesPerSecond = 0;
    }
}
#endif
#ifndef __APPLE__
  STXGUI::update();
#endif

  privateTimeClock += frameInterval*!paused;

  return false;
}

#ifdef USEGROUPS
int SceneFrame::renderSkyDome()
{
  int             skyTriCount = 0;
if(m_pSkyMesh&&renderSky) {
  D3DXFROMWINEMATRIX mvpSky, I;
  static float    skyTimer    = 0.0f;
  const  float    timeScale   = 0.0125f;

	D3DXFROMWINEMatrixIdentity(&I);
  
  if(1)
	skyTriCount=m_pSkyMesh->render(&I);
  else if(0)
	skyTriCount=m_pSkyMesh->render(&mvpSky);
  else if(0)
	skyTriCount=m_pSkyMesh->render(&camera.GetMVP());

  skyTimer += !paused*timeScale*skyInfo.w;//???*benchmark.getFrameInterval();
	}
  return skyTriCount;
}

int  SceneFrame::renderWaterSurface(float timer)
{ 
  int  waterTriCount = 0;
  if(m_pWaterMesh&&renderWater) {
  
  D3DXFROMWINEVECTOR4 translation			(-TERRAIN_X_OFFSET*TERRAIN_WIDTH_SCALE,
                                                         -TERRAIN_Z_OFFSET*TERRAIN_DEPTH_SCALE,
                                                          TERRAIN_WIDTH_SCALE*255.0f,
                                                          TERRAIN_DEPTH_SCALE*255.0f);
  D3DXFROMWINEVECTOR3 cameraPosition;
  cameraPosition.set(camera.getViewerPosition().x, camera.getViewerPosition().y, camera.getViewerPosition().z);
	waterTriCount=m_pWaterMesh->render(&camera.GetMVP());}
  return waterTriCount;
}

void SceneFrame::updateWaterSurface(float timer)
{
  D3DXFROMWINEMATRIX transform, projection;
  D3DXFROMWINEMatrixTranslation(&transform, 510.0f, -10.0f, 1500.0f);
  D3DXFROMWINEMATRIX        inverseTranView = camera.getModelViewMatrix();
                  
  D3DXFROMWINEVECTOR4         cameraPosition,
                  translation;

  cameraPosition.set(camera.getViewerPosition().x, camera.getViewerPosition().y, camera.getViewerPosition().z, 0.0f);
  //???inverseTranView.setInverseTranspose();
  float f=0.0f;
  D3DXFROMWINEMatrixInverse(&inverseTranView, &f, &inverseTranView);
  D3DXFROMWINEMatrixTranspose(&inverseTranView, &inverseTranView);

  waterMatrix.set(0.5f, 0.0f, 0.0f, 0.0f,
                  0.0f, 0.5f, 0.0f, 0.0f, 
                  0.0f, 0.0f, 0.5f, 0.0f,
                  0.5f, 0.5f, 0.5f, 1.0f);
#ifndef _MSC_VER
  IRenderer::GetRendererInstance()->changeRenderTarget(waterTexture);
	IRenderer::GetRendererInstance()->BeginScene();
	//D3DXFROMWINEVECTOR4 color(skyInfo.x, skyInfo.y, skyInfo.z, 1.0f);
	IRenderer::GetRendererInstance()->Clear(true, true, skyInfo);

  setPerspective(0, 0, IRenderer::GetRendererInstance()->GetTextureWidth(waterTexture), IRenderer::GetRendererInstance()->GetTextureHeight(waterTexture));
  D3DXFROMWINEMatrixPerspectiveFovLH(&projection, 90.0f, float(IRenderer::GetRendererInstance()->GetViewportWidth())/IRenderer::GetRendererInstance()->GetViewportHeight(), 4.0f, 7500.0f);
  camera.update();
  frustum.update();

  projection  *= camera.getModelViewMatrix();
  waterMatrix *= projection;
    terrain.renderSimple(timer);
#ifdef USEGROUPS
    renderSkyDome();
#endif
	IRenderer::GetRendererInstance()->EndScene();
  IRenderer::GetRendererInstance()->changeToMainFramebuffer();
	IRenderer::GetRendererInstance()->BeginScene();
#endif
} 
#endif

#if 1//def USEactionPerformed
void SceneFrame::actionPerformed(GUIEvent &evt)
{

  char buf[256];


  const NSString &callbackString  = evt.getCallbackString();

  GUIRectangle *sourceRectangle = evt.getEventSource();

  int           widgetType      = sourceRectangle->getWidgetType();


  if(widgetType == CHECK_BOX)
  {

    GUICheckBox   *checkBox = (GUICheckBox*)sourceRectangle;

    if(checkBox->isClicked())
    {

      if(callbackString == "aToCoverage") terrain.setAlphaToCoverageFlag(checkBox->isChecked());

      if(callbackString == "eOcclusion")  terrain.setOcclusionCullingFlag(checkBox->isChecked());

      if(callbackString == "eFrustum")    terrain.setFrustumCullingFlag(checkBox->isChecked());
      if(callbackString == "rGrass")      terrain.setDrawGrassFlag(checkBox->isChecked());

      if(callbackString == "rTerrain")    terrain.setDrawTerrainFlag(checkBox->isChecked());
      if(callbackString == "rTextures")   renderTextures = checkBox->isChecked();

      if(callbackString == "rSky")        renderSky = checkBox->isChecked();
      if(callbackString == "rWater")      renderWater = checkBox->isChecked();

      if(callbackString == "rAABB")       terrain.setRenderAABBFlag(checkBox->isChecked());

      if(callbackString == "rMode")       terrain.setWireFrameFlag(checkBox->isChecked());

      if(callbackString == "rTBN")        terrain.setDrawTBNFlag(checkBox->isChecked());

    }
  }


  if(widgetType == SLIDER)
  {

    GUISlider   *slider = (GUISlider*)sourceRectangle;


    if(callbackString == "aReference")
    {
      terrain.setAlphaReference(slider->getProgress());
      stx_snprintf(buf, 256, "Alpha Reference: %4.2f", slider->getProgress());
      slider->setLabelString(buf);
    }


    if(callbackString == "aBooster")
    {
      terrain.setAlphaBooster((slider->getProgress()*2.0f));
      stx_snprintf(buf, 256, "Alpha Booster: %4.2f", terrain.getAlphaBooster()); 
      slider->setLabelString(buf);
    }


    if(callbackString == "red")
    {
      skyInfo.x = slider->getProgress();
      stx_snprintf(buf, 256, "Red Intensity: %4.2f", skyInfo.x); 
      slider->setLabelString(buf);
    }


    if(callbackString == "blue")
    {
      skyInfo.z = slider->getProgress();
      stx_snprintf(buf, 256, "Blue Intensity: %4.2f", skyInfo.z); 
      slider->setLabelString(buf);
    }


    if(callbackString == "green")
    {
      skyInfo.y = slider->getProgress();
      stx_snprintf(buf, 256, "Green Intensity: %4.2f", skyInfo.y); 
      slider->setLabelString(buf);
    }


    if(callbackString == "sSpeed")
    {
      skyInfo.w = slider->getProgress()*2.0f;
      stx_snprintf(buf, 256, "Scrolling Speed: %4.2f", slider->getProgress()); 
      slider->setLabelString(buf);
    }


  }
}
#endif

void SceneFrame::setPerspective(unsigned int x, unsigned int y, unsigned int w, unsigned int h)
{
  camera.updateClipPlanes(10.0f, 30000.0f);//4000.0f);
}
void SceneFrame::destroy()
{
  active = false;
}

SceneFrame::~SceneFrame()
{
  destroy();
}

void SceneFrame::DrawAtlas()
{
	std::vector<TextureID> v;
	v.push_back(terrain.weight);
	v.push_back(terrain.fungus);
	v.push_back(terrain.dirt);
	v.push_back(terrain.grass);
	v.push_back(waterTexture);
	v.push_back(normalmap);
	v.push_back(waterDepth);
	v.push_back(terrain.weight);
	v.push_back(terrain.weightR);
	v.push_back(terrain.weightG);
	v.push_back(terrain.weightB);
	v.push_back(terrain.fungus);
	v.push_back(terrain.dirt);
	v.push_back(terrain.grass);
	v.push_back(skyTexture);
	v.push_back(terrain.grassPack);
	v.push_back(m_pWaterMesh->texID);
	IRenderer::GetRendererInstance()->DrawAtlas(v);
	return; 
}

	SkyMesh::SkyMesh(){}
	SkyMesh::~SkyMesh(){}
	void SkyMesh::init(tShader aShader)
	{
  skyTexture=IRenderer::GetRendererInstance()->addImageLibTexture("/RadeonTerrainDemo/Sky1.bmp", false, IRenderer::GetRendererInstance()->Getlinear());
  skyShader = IRenderer::GetRendererInstance()->addShaderFromFile("/NatureScene/SkyShader2.shd", "main", "main");
	{FormatDesc format[] =
	{
					0, TYPE_VERTEX,   FORMAT_FLOAT, 3,
					0, TYPE_NORMAL,   FORMAT_FLOAT, 3,
					0, TYPE_BINORMAL, FORMAT_FLOAT, 3,
					0, TYPE_TANGENT,  FORMAT_FLOAT, 3,
					0, TYPE_TEXCOORD, FORMAT_FLOAT, 3,
					0, TYPE_TEXCOORD, FORMAT_FLOAT, 2
	};
	skyVF = IRenderer::GetRendererInstance()->addVertexFormat(format, elementsOf(format), skyShader);}
		
        	shape3D.CreateHalfSphere(1.0f, eShaderNone);
	}
	unsigned int SkyMesh::render(D3DXFROMWINEMATRIX* mvp)
	{

  IRenderer::GetRendererInstance()->setShader(skyShader);
      IRenderer::GetRendererInstance()->setVertexFormat(skyVF);

	float    skyTimer    = 0.001f*timeGetTime();

  IRenderer::GetRendererInstance()->setTexture("noise", skyTexture);

#if 1
float4  params=float4(-0.167888, -0.035063, 0.930527, skyTimer);
float3  offset=float3(2560.000000, 2560.000000, -5100.000000);
float4 skyInfo=float4(0.25, 0.5, 1.0, 0.0);
#elif 0
float4  params=float4(0.000000, 0.000000, 1.000333, skyTimer);
float3  offset=float3(324.739990, 193.405914, -2237.400879);
float4 skyInfo=float4(0.25, 0.5, 1.0, 0.0);
#elif 0
float4  params=float4(0.0, 0.0, 0.0, skyTimer);
float3  offset=float3(0.0, 0.0, 0.0);
float4 skyInfo=float4(0.0, 0.0, 0.0, 0.0);
#endif
		IRenderer::GetRendererInstance()->setShaderConstant4f("params", params);
		IRenderer::GetRendererInstance()->setShaderConstant3f("offset", offset);
		IRenderer::GetRendererInstance()->setShaderConstant4f("skyInfo", skyInfo);


	float AngleX=0.0f;
	float AngleY=0.0f;
		D3DXFROMWINEMATRIX w;
		#if 1
		if(1)
		{
			static float mAngleX=0.0f;
			static float mAngleY=0.0f;

			float s=5100.0f;
			//s=1.0f;
			D3DXFROMWINEMATRIX W,R,S,T;
			D3DXFROMWINEMatrixRotationYawPitchRoll(&R, D3DXFROMWINEToRadian(mAngleX), D3DXFROMWINEToRadian(mAngleY), 0.0f); 
			D3DXFROMWINEMatrixScaling(&S, s, s, s);
			D3DXFROMWINEVECTOR3 position(0.0f, 0.0f, s);
			D3DXFROMWINEMatrixTranslation(&T, position.x, position.y, position.z);
			W=R*S*T;
			IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", W);
		}
		else if(0)
		{
		D3DXFROMWINEMATRIX I;
		D3DXFROMWINEMatrixIdentity(&I);
		D3DXFROMWINEMatrixRotationYawPitchRoll( &w,
		                            D3DXFROMWINEToRadian(AngleX),
		                            D3DXFROMWINEToRadian(AngleY),
		                            0.0f );

		IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", w);
		}
		else if(0)//(mvp)
		{
		IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", *mvp);
		}
		else if(0)
		{
		D3DXFROMWINEMATRIX W, R, S;
		D3DXFROMWINEMatrixRotationYawPitchRoll(&R, D3DXFROMWINEToRadian(180.0f), D3DXFROMWINEToRadian(180.0f), 0.0f);
		D3DXFROMWINEMatrixScaling(&S, 2.0f, 1.5f, 2.0f);
		W=R*S;
		IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", W);
		}

	unsigned int SkyTriCount=0;

	#if 1
	const uint flags=RESET_ALL;
	#else
	const uint flags=RESET_VB|RESET_IB;
	#endif
		if(shape3D.indices.size()){
			IRenderer::GetRendererInstance()->DrawIndexedPrimitiveUP(PRIM_TRIANGLES, 0, shape3D.vertices.size(), shape3D.indices.size()/3, &shape3D.indices[0], &shape3D.indices[0], CONSTANT_INDEX2, &shape3D.vertices[0], &shape3D.vertices[0], sizeof(stx_VertexPositionNormalTexture), flags);
			SkyTriCount=shape3D.indices.size()/3;
		}
		else if(shape3D.vertices.size()){
			IRenderer::GetRendererInstance()->DrawPrimitiveUP(PRIM_TRIANGLES, shape3D.vertices.size()/3, &shape3D.vertices[0], &shape3D.vertices[0], sizeof(stx_VertexPositionNormalTexture), flags);
			SkyTriCount=shape3D.indices.size()/3;
		}
		else{
			shape3D.render(&w);
			SkyTriCount=shape3D.indices.size()/3;
		}
		#else
		shape3D.render(&w);
			SkyTriCount=shape3D.indices.size()/3;
		#endif
		return SkyTriCount;
	}

