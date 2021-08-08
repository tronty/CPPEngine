#include "../Framework/Framework.h"
#include "../Framework/Util/OpenGLModel.h"



class MainApp : public OpenGLApp {
protected:
	OpenGLModel *model;

	TextureID environment;

	TextureID rt0, rt1, rt2;
	ShaderID lightingVS, lightingFS;
	ShaderID blur;

	void drawObject(float push);
	void blurScene();
public:
	void selectPixelFormat(PixelFormat &pixelFormat);

	bool init();
	bool exit();
	bool load();
	bool unload();
	bool drawFrame();
	
	void resetCamera();
};

void MainApp::resetCamera(){
	position = Vertex(0, 0, -400);
	Wx = 0;
	Wy = 0;
	Wz = 0;
}

void MainApp::selectPixelFormat(PixelFormat &pixelFormat){
	pixelFormat.redBits   = 8;
	pixelFormat.greenBits = 8;
	pixelFormat.blueBits  = 8;
	pixelFormat.alphaBits = 0;

	pixelFormat.depthBits = 24;
}

bool MainApp::init(){
	model = new OpenGLModel();
	model->loadFromFile("Gold.hmdl");
	return true;
}

bool MainApp::exit(){
	delete model;
	return true;
}

bool MainApp::load(){
	((OpenGLBatch *) model->getBatch(0))->uploadToVertexBuffer();

	String supported, missing;

    if (GL_ARB_vertex_program)   supported += "GL_ARB_vertex_program\n";   else missing += "GL_ARB_vertex_program\n";
    if (GL_ARB_fragment_program) supported += "GL_ARB_fragment_program\n"; else missing += "GL_ARB_fragment_program\n";
#ifdef _WIN32
	if (WGL_ARB_render_texture)  supported += "WGL_ARB_render_texture\n";  else missing += "WGL_ARB_render_texture\n";
#else
    if (GLX_ATI_render_texture)  supported += "GLX_ATI_render_texture\n";  else missing += "GLX_ATI_render_texture\n";
#endif
    if (missing.getLength() > 0){
        ALERT("The following required OpenGL extensions are supported:\n\n" + supported +
			  "\nToo bad, these required OpenGL extensions aren't supported by the graphic card and driver you're using:\n\n" + missing +
              "\nThis application will now exit.", "Missing extensions");
        return false;
    }

	environment = renderer->addCubemap(
		"../Textures/CubeMaps/Terrain/negx.jpg",
		"../Textures/CubeMaps/Terrain/posx.jpg",
		"../Textures/CubeMaps/Terrain/negy.jpg",
		"../Textures/CubeMaps/Terrain/posy.jpg",
		"../Textures/CubeMaps/Terrain/negz.jpg",
		"../Textures/CubeMaps/Terrain/posz.jpg"
	);

	rt0 = renderer->addRenderTexture(128, 128, false, false, RPF_RGBA8);
	rt1 = renderer->addRenderTexture(128, 128, false, false, RPF_RGBA8);
	rt2 = renderer->addRenderTexture(128, 128, false, false, RPF_RGBA8);

	textTexture = renderer->addTexture("../Textures/font2.dds");

	projection.loadProjectionMatrix(80 * PI / 180, 0.75f, 0.1f, 6000);

	lightingVS = renderer->addVertexShader("Lighting.vsh");
	if (glGetError() != GL_NO_ERROR){
		char *str = (char *) glGetString(GL_PROGRAM_ERROR_STRING_ARB);
		ALERT(str, "Error");
	}

	lightingFS = renderer->addFragmentShader(GL_FRAGMENT_PROGRAM_ARB, "Lighting.fsh");
	if (glGetError() != GL_NO_ERROR){
		char *str = (char *) glGetString(GL_PROGRAM_ERROR_STRING_ARB);
		ALERT(str, "Error");
	}

	blur = renderer->addFragmentShader(GL_FRAGMENT_PROGRAM_ARB, "Blur.fsh");
	if (glGetError() != GL_NO_ERROR){
		char *str = (char *) glGetString(GL_PROGRAM_ERROR_STRING_ARB);
		ALERT(str, "Error");
	}

	return true;
}

bool MainApp::unload(){
	return true;
}

void MainApp::drawObject(float push){
    glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(modelView);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(projection);

	renderer->setVertexShader(lightingVS);
	renderer->setFragmentShader(lightingFS);
	renderer->setTextures(environment);
	renderer->apply();

	glProgramLocalParameter4fARB(GL_VERTEX_PROGRAM_ARB, 0, push, push, push, 0);
	glProgramLocalParameter4fARB(GL_VERTEX_PROGRAM_ARB, 1, position.x, position.y, position.z, 0);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	model->draw();

	glDisable(GL_CULL_FACE);
}

void MainApp::blurScene(){
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,1,0,1,-1,1);

	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex2f(0, 0);
		glTexCoord2f(1, 0);
		glVertex2f(1, 0);
		glTexCoord2f(1, 1);
		glVertex2f(1, 1);
		glTexCoord2f(0, 1);
		glVertex2f(0, 1);
	glEnd();
}

bool MainApp::drawFrame(){
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(projection);

	modelView.loadRotateZXY(-Wx, -Wy, -Wz);
    glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(modelView);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	renderer->setTextures(environment);
	renderer->setTexGen(TEXGEN_OBJECT_LINEAR);
	renderer->apply();

	glBegin(GL_QUAD_STRIP);
		glVertex3f(-1, -1, -1);
		glVertex3f( 1, -1, -1);
		glVertex3f(-1, -1,  1);
		glVertex3f( 1, -1,  1);
		glVertex3f(-1,  1,  1);
		glVertex3f( 1,  1,  1);
		glVertex3f(-1,  1, -1);
		glVertex3f( 1,  1, -1);
	glEnd();
	glBegin(GL_QUAD_STRIP);
		glVertex3f( 1,  1,  1);
		glVertex3f( 1, -1,  1);
		glVertex3f( 1,  1, -1);
		glVertex3f( 1, -1, -1);
		glVertex3f(-1,  1, -1);
		glVertex3f(-1, -1, -1);
		glVertex3f(-1,  1,  1);
		glVertex3f(-1, -1,  1);
	glEnd();

	glEnable(GL_DEPTH_TEST);

	modelView.translate(-position);
	drawObject(0);

	renderer->changeRenderTarget(rt0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	drawObject(20);

	renderer->changeRenderTarget(rt1);
	renderer->setTextures(rt0);
	renderer->setFragmentShader(blur);
	renderer->apply();

	blurScene();

	renderer->changeRenderTarget(rt2);
	renderer->setTextures(rt1);
	renderer->setFragmentShader(blur);
	renderer->apply();

	blurScene();

	renderer->changeRenderTarget();

	renderer->setTextures(rt2);
	renderer->setFragmentShader(blur);
	renderer->setBlending(GL_ONE, GL_ONE);
	renderer->apply();

	glDisable(GL_DEPTH_TEST);

	blurScene();

	return true;
}

DECLARE_MAIN_APP(MainApp)
