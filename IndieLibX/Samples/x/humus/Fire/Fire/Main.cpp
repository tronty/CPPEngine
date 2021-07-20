#include "../Framework/Framework.h"
#include "../Framework/Math/PerlinNoise.h"

class MainApp : public OpenGLApp {
protected:
	ShaderID fire;
	TextureID turb, flame;

	unsigned char *turbBuffer;
public:
	void selectPixelFormat(PixelFormat &pixelFormat);

	bool init();
	bool exit();
	bool load();
	bool unload();
	bool drawFrame();
	
};

void MainApp::selectPixelFormat(PixelFormat &pixelFormat){
	pixelFormat.redBits   = 8;
	pixelFormat.greenBits = 8;
	pixelFormat.blueBits  = 8;
	pixelFormat.alphaBits = 8;

	pixelFormat.depthBits = 24;
}

#define XSIZE 64
#define YSIZE 64
#define ZSIZE 16
#define SCALE 0.08f

bool MainApp::init(){
	initPerlin();

	turbBuffer = new unsigned char[XSIZE * YSIZE * ZSIZE];
	unsigned char *dest = turbBuffer;
	unsigned int min = 255, max = 0;
	for (int z = 0; z < ZSIZE; z++){
		for (int y = 0; y < YSIZE; y++){
			for (int x = 0; x < XSIZE; x++){
				unsigned char t = (unsigned char) (127.5f * (1 + tileableTurbulence3(SCALE * x, SCALE * y, SCALE * z, XSIZE * SCALE, YSIZE * SCALE, ZSIZE * SCALE, 16)));
				if (t > max) max = t;
				if (t < min) min = t;
				*dest++ = t;
			}
		}
	}
	dest = turbBuffer;
	for (unsigned int i = 0; i < XSIZE * YSIZE * ZSIZE; i++){
		dest[i] = (255 * (dest[i] - min)) / (max - min);
	}

	return true;
}

bool MainApp::exit(){
	delete turbBuffer;

	return true;
}

bool MainApp::load(){

	String supported, missing;

    if (GL_ARB_fragment_program) supported += "GL_ARB_fragment_program\n"; else missing += "GL_ARB_fragment_program\n";
    if (GL_EXT_texture3D)        supported += "GL_EXT_texture3D\n";        else missing += "GL_EXT_texture3D\n";

    if (missing.getLength() > 0){
        ALERT("The following required OpenGL extensions are supported:\n\n" + supported +
			  "\nToo bad, these required OpenGL extensions aren't supported by the graphic card and driver you're using:\n\n" + missing +
              "\nThis application will now exit.", "Missing extensions");
        return false;
    }

	Image3D *img = new Image3D();
	img->loadFromMemory(turbBuffer, XSIZE, YSIZE, ZSIZE, 1, false, false);
	turb = renderer->addTexture3D(img, TEX_NOMIPMAPPING);

	flame = renderer->addTexture("../Textures/Flame.png", TEX_NOMIPMAPPING | TEX_CLAMP);
	textTexture = renderer->addTexture("../Textures/font2.dds");

	fire = renderer->addFragmentShader(GL_FRAGMENT_PROGRAM_ARB, "Fire.fsh");
	if (glGetError() != GL_NO_ERROR){
		char *str = (char *) glGetString(GL_PROGRAM_ERROR_STRING_ARB);
		ALERT(str, "Error");
		return false;
	}

	return true;
}

bool MainApp::unload(){
	return true;
}

bool MainApp::drawFrame(){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	renderer->setFragmentShader(fire);
	renderer->setTextures(turb, flame);
	renderer->apply();

	glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 0, 7 * currTime, 0, 0, 0);

	float y = 0.5f * currTime;
	float z = 0.6f * currTime;

	glBegin(GL_QUADS);
		glMultiTexCoord3fARB(GL_TEXTURE0_ARB, 0, y, z);
		glMultiTexCoord4fARB(GL_TEXTURE1_ARB, -1, -0.6f, 6, 0);
		glVertex2f(-1, -1);
		glMultiTexCoord3fARB(GL_TEXTURE0_ARB, 3, y, z);
		glMultiTexCoord4fARB(GL_TEXTURE1_ARB, 1, -0.6f, 6, 0);
		glVertex2f(1, -1);
		glMultiTexCoord3fARB(GL_TEXTURE0_ARB, 3, y - 1.0f, z);
		glMultiTexCoord4fARB(GL_TEXTURE1_ARB, 1, 0.7f, 0, 0.05f);
		glVertex2f(1, 1);
		glMultiTexCoord3fARB(GL_TEXTURE0_ARB, 0, y - 1.0f, z);
		glMultiTexCoord4fARB(GL_TEXTURE1_ARB, -1, 0.7f, 0, 0.05f);
		glVertex2f(-1, 1);
	glEnd();

	return true;
}

DECLARE_MAIN_APP(MainApp)
