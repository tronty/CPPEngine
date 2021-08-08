#include <Tools/Factory3DS.h>
#include "SceneFrame.h"
//using namespace NatureScene;

D3DXFROMWINEMATRIX SceneFrame::matProj=identity4();
SceneFrame::SceneFrame(int instance) : IOXMLObject("SceneFrame"), cellCounter(0), triCounter(0), fpsCounter(0)
{
  //Logger::initialize();
  MediaPathManager::registerPath("/NatureScene/XML/");

  randommap=-1;
  waterDepth=-1;
#ifdef __ALL__
  waterTexture=-1;
#endif
  grass=-1;
  grassPack=-1;
  noise=-1;

  bTranspose=true;
  bIdentity=false;
  bInverse=true;
  bVP=false;
  bPV=false;

  skyShader=-1;
#ifdef __ALL__
  waterShader=-1;
#endif
  drawSky      =  false;
  renderWater  =  false;

  terrain=new Terrain();
  LOG_PRINT("terrain = %x\n", terrain);

  //window.setInputEventListener(this);
  //window.setAppInstance(instance);
  skyInfo.set(0.25f, 0.5f, 1.0f, 1.0f);

  //benchmark.setEnabled(false);
  mouseLocked  = false;
  active       =  true;
  paused       = false;

  eye=D3DXFROMWINEVECTOR3(652.0f, 68.0f, 2210.0f);
  look=D3DXFROMWINEVECTOR3(510.0f, 95.0f, 1670.0f);
  up=D3DXFROMWINEVECTOR3(0.0f,  1.0f,    0.0f);
}

bool SceneFrame::initialize()
{
  GUIPanel *loading  = 0;
  stx_srand((unsigned int) time( 0));

#ifdef LOADXMLSETTINGS
  if(!IOXMLObject::loadXMLSettings("/NatureScene/IniFile.xml"))
    return false;
#else
  STXGUI::init("/NatureScene/GUILayout.xml");
#endif
/*
  LOG_PRINT("Renderer: ") + (char*)glGetString(GL_RENDERER));
  LOG_PRINT("Version:  ") + (char*)glGetString(GL_VERSION));
  LOG_PRINT("Vendor:   ") + (char*)glGetString(GL_VENDOR));
*/
  camera.setup(510.0f, 68.0f, 2210.0f,
               652.0f, 95.0f, 1670.0f,
                 0.0f,  1.0f,    0.0f);
  //D3DXFROMWINEMatrixLookAtLH(&matView, &eye, &look, &up);

/*
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
*/
  setPerspective(0, 0, IRenderer::GetRendererInstance()->GetViewportWidth(), IRenderer::GetRendererInstance()->GetViewportHeight());
  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  if(loading = (GUIPanel*)GetGuiFrame()->getWidgetByCallbackString("LoadingPanel"))
  {
    //Renderer::enter2DMode(IRenderer::GetRendererInstance()->GetViewportWidth(), IRenderer::GetRendererInstance()->GetViewportHeight());
    loading->render(0.0f);
    //Renderer::exit2DMode();
  }

  //window.setVisible(true);
  //window.update();

#ifdef LOADXMLSETTINGS
#ifdef __ALL__
  //waterTexture.create2DShell("Water Texture", 512, 512, GL_RGBA8, GL_RGBA, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
  waterTexture = IRenderer::GetRendererInstance()->addRenderTarget(512, 512, FORMAT_RGBA8);
  waterTextureWidth = 512;//IRenderer::GetRendererInstance()->GetTextureWidth(waterTexture);
  waterTextureHeight = 512;//IRenderer::GetRendererInstance()->GetTextureHeight(waterTextureHeight);

  //waterShader.loadXMLSettings("WaterShader.xml");
  waterShader = IRenderer::GetRendererInstance()->addShaderFromFile("/NatureScene/WaterShader.shd", "main", "main");
  waterGroup.IOXMLObject::loadXMLSettings("/NatureScene/Water/WaterGroup.xml");
  waterGroup.compile();
#endif
  //skyShader.loadXMLSettings("Skyshader.xml");
  skyShader = IRenderer::GetRendererInstance()->addShaderFromFile("/NatureScene/SkyShader.shd", "main", "main");
  grass = IRenderer::GetRendererInstance()->addImageLibTexture("/NatureScene/Terrain/grasslayer.png", false, IRenderer::GetRendererInstance()->Getlinear());
  grassPack = IRenderer::GetRendererInstance()->addImageLibTexture("/NatureScene/Grass/grassPack.png", false, IRenderer::GetRendererInstance()->GetlinearClamp());


  	LOG_PRINT("addImageLibTexture:/NatureScene/Terrain/grasslayer.png:%x\n",grass);
	LOG_PRINT("addImageLibTexture:/NatureScene/Grass/grassPack.png:%x\n",grassPack);


	//
	//LOG_PRINT("grassPack = %x\n", grassPack);

#if 1

	

  skyDome.IOXMLObject::loadXMLSettings("/NatureScene/Sky/SkyGroup.xml");


  skyDome.compile();

#endif
#endif

noise=IRenderer::GetRendererInstance()->addImageLibTexture(
		"/NatureScene/Sky/clouds.png"
		,false,
		IRenderer::GetRendererInstance()->Getlinear());
  	LOG_PRINT("addImageLibTexture:/NatureScene/Sky/clouds.png:%x\n",noise);


#if 0
  TextureInfo *wDepth = TexturesManager::getTextureInfo("/NatureScene/Terrain/watermap.jpg");
  waterDepth.setID(wDepth ? wDepth->getMedia() : 0);
  return  (waterDepth.getID() != 0);
#else
waterDepth=-1;
#ifdef __ALL__
waterDepth=IRenderer::GetRendererInstance()->addImageLibTexture(
		"/NatureScene/Terrain/watermap.jpg"
		,false,
		IRenderer::GetRendererInstance()->Getlinear());

  	LOG_PRINT("addImageLibTexture:/NatureScene/Terrain/watermap.jpg:%x\n",waterDepth);
#endif
#ifdef LOADXMLSETTINGS
  guiFrame.setGUIEventListener(this);

  if(loading)
    loading->setVisible(false);


  guiFrame.setVisible(true);
#endif
  camera.update(0.001f);
  
  LOG_FNLN;
  terrain->initialize(true);

  LOG_FNLN;
  //Logger::flush();

return true;
#endif
}

void SceneFrame::updateClipPlanes(float nearClip, float farClip)
{
	STXGUI::update();
#if 0
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(90.0f, (float)IRenderer::GetRendererInstance()->GetViewportWidth(), IRenderer::GetRendererInstance()->GetViewportHeight(), nearClip, farClip);
  glMatrixMode(GL_MODELVIEW);
#else
	D3DXFROMWINEMatrixPerspectiveFovLH(&matProj, D3DXFROMWINE_PI / 2.0f, float(IRenderer::GetRendererInstance()->GetViewportWidth()) / float(IRenderer::GetRendererInstance()->GetViewportHeight()), nearClip, farClip);
	//D3DXFROMWINEMatrixLookAtLH(&matView, &eye, &look, &up);
#endif
}

void SceneFrame::run2()
{  	
  	setPerspective(0, 0, IRenderer::GetRendererInstance()->GetViewportWidth(), IRenderer::GetRendererInstance()->GetViewportHeight());
	IRenderer::GetRendererInstance()->Clear(true,true,D3DXFROMWINEVECTOR4(0.35f, 0.53f, 0.7, 1.0f));
  	IRenderer::GetRendererInstance()->BeginScene();
	int vpw=IRenderer::GetRendererInstance()->GetViewportWidth();
	int vph=IRenderer::GetRendererInstance()->GetViewportHeight()/2;
	unsigned int n=IRenderer::GetRendererInstance()->GetTextureCount();
	float ss=32.0f;
	
	unsigned int ny=vph/ss;
	unsigned int nx=vpw/ss;
	unsigned int c=0;
	for(unsigned int xx=0;xx<nx;xx++)
	for(unsigned int yy=0;yy<ny;yy++)
	{
	if(c>n) continue;
	D3DXFROMWINEVECTOR2 aposition(ss*xx,ss*yy);
	D3DXFROMWINEVECTOR2 asize(ss,ss);
	TexVertex dest[4];
	float x=aposition.x;
	float y=aposition.y;
	dest[0].position = D3DXFROMWINEVECTOR2(x + asize.x, y);
	dest[0].texCoord = D3DXFROMWINEVECTOR2(1.0f, 0.0f);
	dest[1].position = D3DXFROMWINEVECTOR2(x + asize.x, y + asize.y);
	dest[1].texCoord = D3DXFROMWINEVECTOR2(1.0f, 1.0f);
	dest[2].position = D3DXFROMWINEVECTOR2(x, y);
	dest[2].texCoord = D3DXFROMWINEVECTOR2(0.0f, 0.0f);
	dest[3].position = D3DXFROMWINEVECTOR2(x, y + asize.y);
	dest[3].texCoord = D3DXFROMWINEVECTOR2(0.0f, 1.0f);

	IRenderer::GetRendererInstance()->drawTextured(PRIM_TRIANGLE_STRIP, dest, 2, c++, IRenderer::GetRendererInstance()->GetblendSrcAlpha(), IRenderer::GetRendererInstance()->GetnoDepthTest());
	}
	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present();
}

void SceneFrame::run()
{
  
  static  float privateTimeClock = 0.0f;
  Tuple4i sceneInfo;
  float   frameInterval          = 0.0f;//benchmark.getFrameInterval();
  int     renderedTriangles      = 0,
          waterMeshTri           = 0;

  //benchmark.markFrameStart();
#ifdef __ALL__
  if(getDrawWaterFlag())
  	updateWaterSurface(privateTimeClock);
#endif
  setPerspective(0, 0, IRenderer::GetRendererInstance()->GetViewportWidth(), IRenderer::GetRendererInstance()->GetViewportHeight());
  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //LOG_PRINT("Clear\n");
  IRenderer::GetRendererInstance()->Clear(true,true,D3DXFROMWINEVECTOR4(0.35f, 0.53f, 0.7, 1.0f));
  
  IRenderer::GetRendererInstance()->BeginScene();

  terrain->adjustCameraYLocation(camera, 60.0f);

  static Uint32 time0=timeGetTime();
  Uint32 time=timeGetTime();
  Uint32 dt=time-time0;
  //LOG_PRINT("dt1=%ld\n", dt);
  
  camera.update(0.001f*(time-time0));
  time0=time;

  frustum.update();
  updateClipPlanes(10.0f, 4000.0f);

  if(terrain->getDrawTerrainFlag())
	sceneInfo    = terrain->render(/*camera,*/ frustum, privateTimeClock);
#ifdef __ALL__
  if(getDrawWaterFlag())
  	waterMeshTri = renderWaterSurface(privateTimeClock);
#endif
LOG_FNLN;
  if(getDrawSkyFlag())
  {
LOG_FNLN;
  	sceneInfo.x += renderSkyDome();
	}

  static bool rendergui=true;
  if(STX_Service::GetInputInstance()->IsMouseButtonPressed(STX_MBUTTON_RIGHT))
	rendergui=rendergui?false:true;
  if(rendergui)
  	guiFrame.render(frameInterval);
  
  IRenderer::GetRendererInstance()->EndScene();
  //LOG_PRINT("Present\n");
  IRenderer::GetRendererInstance()->Present();

  if(rendergui)
  	windowEventsProcessor();

  sceneInfo.x += waterMeshTri;

  if(triCounter) triCounter ->setLabelString(NSString("Triangle count: ") + sceneInfo.x*(waterMeshTri ? 2 : 1));
  if(cellCounter)cellCounter->setLabelString(NSString("Visible Cells: ")  + sceneInfo.y + "%");
  {
  float fps;
  static int n=0;
  n++;
  static Uint32 t1=timeGetTime();
  Uint32 t2=timeGetTime();
  Uint32 dt=t2-t1;
  if(dt>=1000)
  {
	fps=float(n)/dt;
	t1=t2;
	n=0;
  }

  if(fpsCounter) fpsCounter ->setLabelString(NSString("Current FPS: ")    + fps);//benchmark.getFPS()));
  }

  //Renderer::enter2DMode(IRenderer::GetRendererInstance()->GetViewportWidth() , //IRenderer::GetRendererInstance()->GetViewportHeight());
    //guiFrame.render(frameInterval);
  //Renderer::exit2DMode();

  //window.update(frameInterval);
  //benchmark.markFrameEnd();

  privateTimeClock += frameInterval*!paused;

  //??? return benchmark.running();
}

int SceneFrame::renderSkyDome()
{
LOG_FNLN;
  TransformGroup *parent      = skyDome.getGroup("directParent");
  static float    skyTimer    = 0.0f;
  const  float    timeScale   = 0.0125f;
  int             skyTriCount = 0;

  parent = parent ? parent : &skyDome;

  const BoundsDescriptor &aabb = parent->getBoundsDescriptor();
  
  updateClipPlanes(25.0f, 9000.0f);
/*
  skyShader.updateElementsf("skyInfo", 4, skyInfo);
  skyShader.updateElementsf("params",  4, D3DXFROMWINEVECTOR4(aabb.getExtents(), skyTimer));
  skyShader.updateElementsf("offset",  3, D3DXFROMWINEVECTOR3(aabb.getMinEndAABB())*-1.0);
 */
  skyDome.getTransform().setTranslations(camera.getViewerPosition());

  //skyShader.activate();
  //
  //LOG_PRINT("setShader(skyShader)\n");
  IRenderer::GetRendererInstance()->setShader(skyShader);

  D3DXFROMWINEMATRIX matVP = GetCamera().getModelViewMatrix()*GetProj();
  D3DXFROMWINEMatrixTranspose(&matVP, &matVP);

  IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", matVP);
  IRenderer::GetRendererInstance()->setShaderConstant4f("skyInfo", skyInfo);
	LOG_PRINT("skyInfo.x=%f\n", skyInfo.x);
	LOG_PRINT("aabb.getExtents().x=%f\n", aabb.getExtents().x);
	LOG_PRINT("aabb.getExtents().y=%f\n", aabb.getExtents().y);
	LOG_PRINT("aabb.getExtents().z=%f\n", aabb.getExtents().z);
	LOG_PRINT("aabb.getMinEndAABB().x=%f\n", aabb.getMinEndAABB().x);
	LOG_PRINT("aabb.getMinEndAABB().y=%f\n", aabb.getMinEndAABB().y);
	LOG_PRINT("aabb.getMinEndAABB().z=%f\n", aabb.getMinEndAABB().z);
  IRenderer::GetRendererInstance()->setShaderConstant4f("params", D3DXFROMWINEVECTOR4(aabb.getExtents().x,aabb.getExtents().y,aabb.getExtents().z, timeGetTime()/1000.0f));//skyTimer));
  IRenderer::GetRendererInstance()->setShaderConstant3f("offset", D3DXFROMWINEVECTOR3(aabb.getMinEndAABB())*-1.0);

  if(randommap==-1)
  {
  	randommap=IRenderer::GetRendererInstance()->addImageLibTexture("/NatureScene/randommap.png",false);
	
  	LOG_PRINT("addImageLibTexture:/NatureScene/randommap.png:%x\n",randommap);
  }
  IRenderer::GetRendererInstance()->setTexture("noise", noise);

  skyTriCount = skyDome.render(ALL_EFFECTS,0,RESET_ALL);
  //skyShader.deactivate();

  skyTimer += !paused*timeScale*skyInfo.w;//*benchmark.getFrameInterval();

  updateClipPlanes(10.0f, 4000.0f);
  return skyTriCount;
}
#ifdef __ALL__
int  SceneFrame::renderWaterSurface(float timer)
{ 


  if(!renderWater || !frustum.AABBInFrustum(waterGroup.getBoundsDescriptor()))
    return 0;

  updateClipPlanes(10.0f, 4000.0f);
  int  waterTriCount = 0;
  
  //glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  //???waterTexture.getTransform().set(waterMatrix);
/*
  waterTexture.activate(1);
  waterDepth.activate(2);
  waterShader.activate();
*/
  
  IRenderer::GetRendererInstance()->setShader(waterShader);
  //IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
  //IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());

  	//D3DXFROMWINEMATRIX matVP = matView*matProj;
	D3DXFROMWINEMATRIX matVP = camera.getModelViewMatrix()*matProj;

  IRenderer::GetRendererInstance()->setShaderConstant4x4f("worldViewProj", matVP);
  IRenderer::GetRendererInstance()->setShaderConstant1f("timer", timeGetTime()/1000.0f);
  IRenderer::GetRendererInstance()->setTexture("reflection", waterTexture);
  //???IRenderer::GetRendererInstance()->setTexture("normalmap", normalmap);
  IRenderer::GetRendererInstance()->setTexture("depthmap", waterDepth);
  IRenderer::GetRendererInstance()->setShaderConstant4f("terrainInfo", D3DXFROMWINEVECTOR4(-TERRAIN_X_OFFSET*TERRAIN_WIDTH_SCALE, -TERRAIN_Z_OFFSET*TERRAIN_DEPTH_SCALE, TERRAIN_WIDTH_SCALE*255.0f, TERRAIN_DEPTH_SCALE*255.0f));
  IRenderer::GetRendererInstance()->setShaderConstant4f("translation", translation);
  D3DXFROMWINEVECTOR3 eyepos=eye;
  IRenderer::GetRendererInstance()->setShaderConstant4f("camera", D3DXFROMWINEVECTOR4(eyepos.x, eyepos.y, eyepos.z, 0.0f));
#if 0
  translation.set(transform[12], transform[13],  transform[14], 0.0f);
  waterShader.updateElementsf("terrainInfo", 4,  D3DXFROMWINEVECTOR4(
-TERRAIN_X_OFFSET*TERRAIN_WIDTH_SCALE,
-TERRAIN_Z_OFFSET*TERRAIN_DEPTH_SCALE,
TERRAIN_WIDTH_SCALE*255.0f,
TERRAIN_DEPTH_SCALE*255.0f));
  IRenderer::GetRendererInstance()->setShaderConstant4f("translation",  translation);
  IRenderer::GetRendererInstance()->setShaderConstant4f("camera",  cameraPosition);
#endif

  waterTriCount += waterGroup.render(ALL_EFFECTS, &frustum);


  //waterShader.deactivate();
  //waterDepth.deactivate();
  //waterTexture.deactivate();

  //glDisable(GL_BLEND);

  return waterTriCount;
}

void SceneFrame::updateWaterSurface(float timer)
{
  if(!renderWater || !frustum.AABBInFrustum(waterGroup.getBoundsDescriptor()))
    return;

  const D3DXFROMWINEMATRIX &transform       = waterGroup.getTransform().getMatrix4f();
  D3DXFROMWINEMATRIX inverseTranView = camera.getModelViewMatrix();

  D3DXFROMWINEMATRIX projection;
                  
  D3DXFROMWINEVECTOR4         cameraPosition,
                  translation;

  D3DXFROMWINEVECTOR3 eyepos=eye;
  cameraPosition.set(eyepos.x, eyepos.y, eyepos.z, 0.0f);
  FLOAT determinant=0.0f;
  D3DXFROMWINEMatrixInverse(&inverseTranView, &determinant, &inverseTranView);
  D3DXFROMWINEMatrixTranspose(&inverseTranView, &inverseTranView);

  waterMatrix=D3DXFROMWINEMATRIX(0.5f, 0.0f, 0.0f, 0.0f,
                  0.0f, 0.5f, 0.0f, 0.0f, 
                  0.0f, 0.0f, 0.5f, 0.0f,
                  0.5f, 0.5f, 0.5f, 1.0f);
#if 0
  setPerspective(WindowEvent(0, 0, waterTexture.getWidth(), waterTexture.getHeight()));
  projection.setPerspective(90.0f, float(window.getWidth())/window.getHeight(), 4.0f, 7500.0f);
  modifyProjectionMatrix(D3DXFROMWINEVECTOR4(0.0f, -1.0f, 0.0f, 0.0f)*inverseTranView);
#else
  //???setPerspective(0, 0, waterTextureWidth, waterTextureHeight);
  D3DXFROMWINEMatrixPerspectiveLH(&projection, IRenderer::GetRendererInstance()->GetViewportWidth(), IRenderer::GetRendererInstance()->GetViewportHeight(), 4.0f, 7500.0f);
#endif
  D3DXFROMWINEVECTOR3 v1(0.0f, -1.0f, 0.0f), v2;
  D3DXFROMWINEVec3TransformCoord(&v2, &v1, &inverseTranView);
  modifyProjectionMatrix(D3DXFROMWINEVECTOR4(v2.x, v2.y, v2.z, 0.0f));

  static Uint32 time0=timeGetTime();
  Uint32 time=timeGetTime();
  Uint32 dt=time-time0;
  //LOG_PRINT("dt1=%ld\n", dt);
  
  camera.update(0.001f*(time-time0));
  time0=time;
  frustum.update();

  projection  *= camera.getModelViewMatrix();
  D3DXFROMWINEMATRIX wT;
  IRenderer::GetRendererInstance()->GetTransform(TS_WORLD, wT);
  D3DXFROMWINEMATRIX vT;
  IRenderer::GetRendererInstance()->GetTransform(TS_VIEW, vT);
  projection  = projection*vT*wT;

  waterMatrix *= projection;

  IRenderer::GetRendererInstance()->changeRenderTarget(waterTexture);
  //???setPerspective(0, 0, 512, 512);
  IRenderer::GetRendererInstance()->Clear(true,true,D3DXFROMWINEVECTOR4(0.35f, 0.53f, 0.7, 1.0f));

  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //glFrontFace(GL_CW);

  //glPushMatrix();
#if 0
    glTranslatef(0.0, transform.m[13], 0.0);
    glScalef(1.0f, -1.0f, 1.0f);
#else
    D3DXFROMWINEMATRIX mT, mS;
    D3DXFROMWINEMatrixTranslation(&mT, 0.0f, transform[13], 0.0f);
    D3DXFROMWINEMatrixScaling(&mS, 1.0f, -1.0f, 1.0f);
    D3DXFROMWINEMATRIX w=mS*mT;
    IRenderer::GetRendererInstance()->SetTransform(TS_WORLD, w);
#endif
    terrain->renderSimple(timer);
    renderSkyDome();
  //glPopMatrix();
 
  //glFrontFace(GL_CCW);

  //waterTexture.copyCurrentBuffer();
  IRenderer::GetRendererInstance()->changeToMainFramebuffer();
  setPerspective(0, 0, IRenderer::GetRendererInstance()->GetViewportWidth(), IRenderer::GetRendererInstance()->GetViewportHeight());
#if 0
  translation.set(transform[12], transform[13],  transform[14], 0.0f);
  waterShader.updateElementsf("terrainInfo", 4,  D3DXFROMWINEVECTOR4(-TERRAIN_X_OFFSET*TERRAIN_WIDTH_SCALE,
                                                         -TERRAIN_Z_OFFSET*TERRAIN_DEPTH_SCALE,
                                                          TERRAIN_WIDTH_SCALE*255.0f,
                                                          TERRAIN_DEPTH_SCALE*255.0f));
  waterShader.updateElementsf("translation", 4,  translation);
  waterShader.updateElementsf("camera",      4,  cameraPosition);
  waterShader.updateElementsf("timer",       1, &timer);
#endif
} 
#endif
void SceneFrame::actionPerformed(GUIEvent &evt)
{
  const NSString &callbackString  = evt.getCallbackString();
  GUIRectangle *sourceRectangle = evt.getEventSource();
  int           widgetType      = sourceRectangle->getWidgetType();

  if(widgetType == CHECK_BOX)
  {
    GUICheckBox   *checkBox = (GUICheckBox*)sourceRectangle;
    if(checkBox->isPressed())
    {
      if(callbackString == "aToCoverage") terrain->setAlphaToCoverageFlag(checkBox->isChecked());
      if(callbackString == "eOcclusion")  terrain->setOcclusionCullingFlag(checkBox->isChecked());
      if(callbackString == "eFrustum")    terrain->setFrustumCullingFlag(checkBox->isChecked());
      if(callbackString == "rSky")        drawSky=checkBox->isChecked();
      if(callbackString == "rGrass")      terrain->setDrawGrassFlag(checkBox->isChecked());
      if(callbackString == "rWater")      renderWater = checkBox->isChecked();
      if(callbackString == "rTerrain")    terrain->setDrawTerrainFlag(checkBox->isChecked());
      if(callbackString == "rAABB")       terrain->setRenderAABBFlag(checkBox->isChecked());
      if(callbackString == "rMode")       terrain->setWireFrameFlag(checkBox->isChecked());
      if(callbackString == "rMode2")      bSkyWireFrameFlag=checkBox->isChecked();
      if(callbackString == "rTBN")        terrain->setDrawTBNFlag(checkBox->isChecked());
/*
      if(callbackString == "W")           setWFlag(checkBox->isChecked());
      if(callbackString == "VP")          setVPFlag(checkBox->isChecked());
      if(callbackString == "PV")          setPVFlag(checkBox->isChecked());
      if(callbackString == "TRANSPOSE")	  setTransposeFlag(checkBox->isChecked());
      if(callbackString == "IDENTITY")	  setIdentityFlag(checkBox->isChecked());
      if(callbackString == "INVERSE")	  setInverseFlag(checkBox->isChecked());
*/
    }
  }

  if(widgetType == SLIDER)
  {
    GUISlider   *slider = (GUISlider*)sourceRectangle;

    if(callbackString == "aReference")
    {
      terrain->setAlphaReference(slider->getProgress());
      slider->setLabelString(NSString("Alpha Reference: ") + slider->getProgress());
    }

    if(callbackString == "aBooster")
    {
      terrain->setAlphaBooster((slider->getProgress()*2.0f));
      slider->setLabelString(NSString("Alpha Booster: ") + terrain->getAlphaBooster()); 
    }

    if(callbackString == "red")
    {
      skyInfo.x = slider->getProgress();
      slider->setLabelString(NSString("Red Intensity: ") + skyInfo.x); 
    }

    if(callbackString == "blue")
    {
      skyInfo.z = slider->getProgress();
      slider->setLabelString(NSString("Blue Intensity: ") + skyInfo.z); 
    }

    if(callbackString == "green")
    {
      skyInfo.y = slider->getProgress();
      slider->setLabelString(NSString("Green Intensity: ") + skyInfo.y); 
    }

    if(callbackString == "sSpeed")
    {
      skyInfo.w = slider->getProgress()*2.0f;
      slider->setLabelString(NSString("Scrolling Speed: ") + slider->getProgress()); 
    }
  }
}

void SceneFrame::setPerspective(int evtx, int evty, int evtWidth, int evtHeight)
{
  //glViewport(evtx, evty, evtWidth, evtHeight);
  //glMatrixMode(GL_PROJECTION);
  IRenderer::GetRendererInstance()->setViewport(evtx, evty, evtWidth, evtHeight);

  updateClipPlanes(10.0f, 4000.0f);
  //glLoadIdentity();

  if((evtWidth == IRenderer::GetRendererInstance()->GetViewportWidth()) && (evtHeight == IRenderer::GetRendererInstance()->GetViewportHeight()))
  {
    guiFrame.setPosition(0,0);
    guiFrame.setDimensions(float(IRenderer::GetRendererInstance()->GetViewportWidth()), float(IRenderer::GetRendererInstance()->GetViewportHeight()));
    guiFrame.forceUpdate(true);
  }
}

void SceneFrame::keyReleased()
{
  
  //camera.setKeyboardInput(false);
  //if(STX_Service::GetInputInstance()->IsKeyPressed(STX_KEY_F8))
      //window.scheduleSnapshot();
  if(STX_Service::GetInputInstance()->IsKeyPressed(STX_KEY_PAUSE))
      paused = !paused;
}

void SceneFrame::keyPressed ()
{
  GUIRectangle *rectangle = 0;

  
  //camera.setKeyboardInput(true);
  if(STX_Service::GetInputInstance()->IsKeyPressed(STX_KEY_h))
     if(rectangle = guiFrame.getWidgetByCallbackString("User Controls"))
       rectangle->setVisible(!rectangle->isVisible());
  if(STX_Service::GetInputInstance()->IsKeyPressed(STX_KEY_ESCAPE))
	active = false;
}

bool SceneFrame::loadXMLSettings(XMLElement *element)
{
  bool         success = false;
#ifdef LOADXMLSETTINGS
  if(!isSuitable(element))
    return false;


  XMLElement  *child   = 0;
#if 0

  for(size_t i = 0; i < element->getChildrenCount(); i++)
  {
    child = element->getChild(i);

    if(child->getName() == "MediaFolder")
    {
      MediaPathManager::registerPath(child);
      continue;
    }
  }

#endif
  if(child = element->getChildByName("Window"))
    success = true;//window.loadXMLSettings(child);
#endif

  //if(child = element->getChildByName("guiPath"))
  //if(guiFrame.GUIPanel::loadXMLSettings(child->getValuec()))
  if(1)//guiFrame.GUIPanel::loadXMLSettings("/NatureScene/GUILayout.xml"))
  {
    GUISlider *slider = 0;
    //if(slider = (GUISlider*)GetGuiFrame()->getWidgetByCallbackString("red"))    skyInfo.x = slider->getProgress();
    //if(slider = (GUISlider*)GetGuiFrame()->getWidgetByCallbackString("blue"))   skyInfo.z = slider->getProgress();
    //if(slider = (GUISlider*)GetGuiFrame()->getWidgetByCallbackString("green"))  skyInfo.y = slider->getProgress();
    //if(slider = (GUISlider*)GetGuiFrame()->getWidgetByCallbackString("sSpeed")) skyInfo.w = slider->getProgress()*2.0f;

    userControls = (GUIPanel*)GetGuiFrame()->getWidgetByCallbackString("User Controls");
    statistics   = (GUIPanel*)GetGuiFrame()->getWidgetByCallbackString("Statistics");
    //cellCounter  = (GUILabel*)GetGuiFrame()->getWidgetByCallbackString("cellCounter");
    //triCounter   = (GUILabel*)GetGuiFrame()->getWidgetByCallbackString("triCounter");
    fpsCounter   = (GUILabel*)GetGuiFrame()->getWidgetByCallbackString("fpsCounter");
  }
  else
    success = false;

  //if(child = element->getChildByName("Benchmark"))
    //benchmark.loadXMLSettings(child);

  return success;
}

void SceneFrame::mouseDoubleClicked()
{ 
  if(userControls)
  {
    const Tuple4i &windowBounds = userControls->getWindowBounds();
    mouseLocked  = (STX_Service::GetInputInstance()->GetMouseY() >= windowBounds.y) &&
                   (STX_Service::GetInputInstance()->GetMouseY() <= windowBounds.w) &&
                   (STX_Service::GetInputInstance()->GetMouseX() >= windowBounds.x) &&
                   (STX_Service::GetInputInstance()->GetMouseX() <= windowBounds.z);
  }

  guiFrame.checkMouseEvents(CLICKED );
}

void SceneFrame::mouseReleased()
{ 
  guiFrame.checkMouseEvents(RELEASED); 
  mouseLocked = false;
}

void SceneFrame::mouseClicked()
{ 
  if(userControls)
  {
    const Tuple4i &windowBounds = userControls->getWindowBounds();
    mouseLocked  = (STX_Service::GetInputInstance()->GetMouseY() >= windowBounds.y) &&
                   (STX_Service::GetInputInstance()->GetMouseY() <= windowBounds.w) &&
                   (STX_Service::GetInputInstance()->GetMouseX() >= windowBounds.x) &&
                   (STX_Service::GetInputInstance()->GetMouseX() <= windowBounds.z);
  }
  guiFrame.checkMouseEvents(CLICKED ); 
}

void SceneFrame::mouseDragged()
{
  //camera.lockMouse(!mouseLocked);
  //camera.setMouseInfo(STX_Service::GetInputInstance()->GetMouseX(), STX_Service::GetInputInstance()->GetMouseY());
  guiFrame.checkMouseEvents(DRAGGED ); 
}

void SceneFrame::mouseMoved()
{ 
  //camera.lockMouse(false);

  //camera.setMouseInfo(STX_Service::GetInputInstance()->GetMouseX(), STX_Service::GetInputInstance()->GetMouseY());
  guiFrame.checkMouseEvents(MOVED   ); 
}

void SceneFrame::destroy()
{
  TexturesManager::flushAllTextures();
  guiFrame.clear();
  active = false;
}

SceneFrame::~SceneFrame()
{
  destroy();
  //Logger::flush();
}

int SceneFrame::windowEventsProcessor()
{
	IInput* i=STX_Service::GetInputInstance();
	//i->Update();

	if(STX_Service::GetInputInstance()->IsMouseButtonPressed(STX_MBUTTON_WHEELDOWN))
           mouseScrolled();
        if(STX_Service::GetInputInstance()->IsMouseDoubleClick())
          mouseDoubleClicked();
	if(STX_Service::GetInputInstance()->IsMouseButtonReleased())
          mouseReleased();
        if(STX_Service::GetInputInstance()->IsMouseButtonPressed())
          mouseClicked();
        if(STX_Service::GetInputInstance()->IsMouseMotion())
	if(STX_Service::GetInputInstance()->IsMouseButtonPressed())
            mouseDragged();
          else
            mouseMoved();

	//if(STX_Service::GetInputInstance()->IsKeyPressed()) keyPressed();
        //if(STX_Service::GetInputInstance()->IsKeyReleased()) keyReleased();
	
	camera.setKeyboardInput();
	

#if 0
        if(STX_Service::GetInputInstance()->IsWindowMoved())
		windowPositionChanged();
        if(STX_Service::GetInputInstance()->IsWindowResized())
		windowSizeChanged();
        if(STX_Service::GetInputInstance()->IsWindowMinimised())
		windowMinimized();
        if(STX_Service::GetInputInstance()->IsWindowMaximised())
		windowMaximized();
        if(STX_Service::GetInputInstance()->IsWindowClosed())
		windowClosing(WindowEvent();
#endif
  return 0;
}

