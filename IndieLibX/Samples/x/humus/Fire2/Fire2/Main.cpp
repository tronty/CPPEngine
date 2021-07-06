#include "../Framework2/OpenGL/OpenGLApp.h"
#include "../Framework2/Util/OpenGLModel.h"
#include "../Framework2/Util/CollisionSet.h"
#include "../Framework2/Util/ParticleSystem.h"
#include "../Framework2/Math/PerlinNoise.h"
#include "../Framework2/Util/GeometryUtil.h"

#define TSIZE 128

class FireParticles : public ParticleSystem {
public:
	void initParticle(Particle &p){
		/*static vec3 poses[] = {
			vec3(-64.6585f, -58.9325f, -41.4861f),
			vec3(-15.5899f, -62.917f, 59.894f),
			vec3(47, -55, -40)
		};*/
		p.pos = pos + vec3(random(0, 20), 0, random(0, 20));
		p.dir = normalize(vec3(random(0, 0.568f), 1, random(0, 0.568f)));
		p.dir *= random(speed, speedSpread);
		p.size = random(size, sizeSpread);
		p.life = random(life, lifeSpread);
		p.invInitialLife = 1.0f / p.life;

		p.angle = random(0, PI);
		p.angleSpeed = random(0, 2.3f);
	}

	void updateParticle(Particle &p, const float time){
		vec3 newPos = p.pos + p.dir * time;

		coll->pushSphere(newPos, 0.6f * p.size);

		if (newPos.y > 50){
			float f = min((newPos.y - 50) / 50, 2);

			newPos.x *= (1 - f * time);
			newPos.z *= (1 - f * time);
		}

		p.pos = newPos;
	}
	void setCollisisionSet(CollisionSet *c){ coll = c; }
private:
	CollisionSet *coll;
};

class MainApp : public OpenGLApp {
public:
	void resetCamera();

	bool init();
	bool exit();

	bool load();
	bool unload();

	void drawSkyBox();
	void drawTerrain();
	void drawFire();
	bool drawFrame();
protected:
	CollisionSet coll;

	OpenGLModel *wood, *terrain;

	FireParticles *fire;

	ShaderID terrainShader, woodShader, skyShader;
	
	TextureID fireTex, base0, base1, lightMap, skybox;
	TextureID particle, noise;
};

void MainApp::resetCamera(){
	position = vec3(0, 50, -400);
	wx = 0;
	wy = 0;
	wz = 0;
}

bool MainApp::init(){
	wood = new OpenGLModel();
	wood->loadFromFile("../Models/Fireplace/Fireplace.hmdl");

	for (unsigned int i = 0; i < wood->getBatchCount(); i++){
		Batch *batch = wood->getBatch(i);

		char *srcVert = batch->getVertices();
		unsigned short *srcInd = (unsigned short *) batch->getIndices();
		unsigned int size = batch->getVertexSize(); 

		for (unsigned int j = 0; j < batch->getIndexCount(); j += 3){
			coll.addTriangle(*(vec3 *) (srcVert + srcInd[j] * size), *(vec3 *) (srcVert + srcInd[j + 1] * size), *(vec3 *) (srcVert + srcInd[j + 2] * size));
		}
	}

	coll.build();


	fire = new FireParticles();
	fire->setSpawnRate(250);
	fire->setSpeed(150, 10);
	fire->setLife(1.5f, 0.5f);
	fire->setDirectionalForce(vec3(0, 100, 0));
	fire->setFrictionFactor(1.0f);
	fire->setPosition(vec3(-15, -50, 15));
	fire->setSize(35, 15);
	fire->setRotate(true);

	fire->setCollisisionSet(&coll);

	initPerlin();




	terrain = new OpenGLModel();
	OpenGLBatch *batch = new OpenGLBatch();

	batch->addFormat(ATT_VERTEX, ATT_FLOAT, 3, 0);
	batch->setPrimitiveType(PRIM_TRIANGLE_STRIP);

	vec3 *vertices = new vec3[TSIZE * TSIZE];
	unsigned int *indices = getRectStripMO(TSIZE);

	batch->setIndices(indices, getStripSize(TSIZE), 4);
	batch->setVertices(vertices, TSIZE * TSIZE, sizeof(vec3));

	terrain->addBatch(batch);

	for (int z = 0; z < TSIZE; z++){
		for (int x = 0; x < TSIZE; x++){
			float y = 100 * noise2((x - 60) * 0.048f, (z - 80) * 0.048f) - 70;

			vertices[mIndex2D(z, x)] = vec3(1500.0f * (float(x) / (TSIZE - 1) - 0.5f), y, 1500.0f * (float(z) / (TSIZE - 1) - 0.5f));
		}
	}

	return true;
}

bool MainApp::exit(){
	delete fire;

	delete terrain;
	delete wood;

	return true;
}

bool MainApp::load(){
	if (!GL_ARB_shader_objects_supported || !GL_ARB_vertex_shader_supported || !GL_ARB_fragment_shader_supported || !GL_ARB_shading_language_100_supported){
		addToLog("No GLSL support (GL_ARB_shader_objects, GL_ARB_vertex_shader, GL_ARB_fragment_shader, GL_ARB_shading_language_100)\n");
		return false;
	}

	if ((terrainShader = renderer->addShader("terrain.shd")) == SHADER_NONE) return false;
	if ((woodShader = renderer->addShader("wood.shd")) == SHADER_NONE) return false;
	if ((skyShader = renderer->addShader("sky.shd")) == SHADER_NONE) return false;

	setDefaultFont("../Textures/Fonts/Future.font", "../Textures/Fonts/Future.dds");

	if ((particle = renderer->addTexture("../Textures/Particle.png")) == TEXTURE_NONE) return false;

	if ((base0 = renderer->addTexture("../Textures/Grass2_small.dds")) == TEXTURE_NONE) return false;
	if ((base1 = renderer->addTexture("../Textures/dirt1.dds")) == TEXTURE_NONE) return false;

	if ((fireTex = renderer->addTexture3D("../Textures/Volumes/explosion3.dds", TEX_NOMIPMAPPING | TEX_CLAMP)) == TEXTURE_NONE) return false;

	if ((skybox = renderer->addCubemap(
		"../Textures/CubeMaps/Sky2/negx.dds",
		"../Textures/CubeMaps/Sky2/posx.dds",
		"../Textures/CubeMaps/Sky2/negy.dds",
		"../Textures/CubeMaps/Sky2/posy.dds",
		"../Textures/CubeMaps/Sky2/negz.dds",
		"../Textures/CubeMaps/Sky2/posz.dds")) == TEXTURE_NONE) return false;


	if ((lightMap = renderer->addTexture("lightmap.png")) == TEXTURE_NONE) return false;


	wood->uploadToVertexBuffer();
	terrain->uploadToVertexBuffer();

#define SIZE 64
#define RANGE 3.37f

	Image3D *nMap = new Image3D();

	if (!nMap->loadFromFile("nMap.dds")){
		unsigned int maxVal = 0, minVal = 255;
		unsigned char *noiseImg = new unsigned char[SIZE * SIZE * SIZE];
		unsigned char *dest = noiseImg;
		int x, y, z;
		for (z = 0; z < SIZE; z++){
			for (y = 0; y < SIZE; y++){
				for (x = 0; x < SIZE; x++){
					unsigned char val = (unsigned char) (127.5f + 127.5f * tileableTurbulence3(x * RANGE / SIZE, y * RANGE / SIZE, z * RANGE / SIZE, RANGE, RANGE, RANGE, 12));
					*dest++ = val;
					if (val > maxVal) maxVal = val;
					if (val < minVal) minVal = val;
				}
			}
		}
		dest = noiseImg;
		for (z = 0; z < SIZE; z++)
			for (y = 0; y < SIZE; y++)
				for (x = 0; x < SIZE; x++)
					*dest++ = (255 * (*dest - minVal)) / (maxVal - minVal);

		nMap->loadFromMemory(noiseImg, SIZE, SIZE, SIZE, FORMAT_I8, true, false);
		nMap->saveToFile("nMap.dds");
	}
	noise = renderer->addTexture3D(nMap, TEX_NOMIPMAPPING);

	return true;
}

bool MainApp::unload(){
	terrain->freeVertexBuffer();
	wood->freeVertexBuffer();
	return true;
}

void MainApp::drawSkyBox(){
	renderer->setTexture("SkyBox", skybox);
	renderer->setDepthFunc(DEPTH_NONE);
	renderer->setMask(COLOR);
	renderer->setShader(skyShader);
	renderer->apply();

	glBegin(GL_TRIANGLE_STRIP);
		glVertex3f( 1024,  1024, -1024);
		glVertex3f( 1024, -1024, -1024);

		glVertex3f( 1024,  1024,  1024);
		glVertex3f( 1024, -1024,  1024);
	
		glVertex3f(-1024,  1024,  1024);
		glVertex3f(-1024, -1024,  1024);
	
		glVertex3f(-1024,  1024, -1024);
		glVertex3f(-1024, -1024, -1024);
	glEnd();

	glBegin(GL_TRIANGLE_STRIP);
		glVertex3f(-1024, -1024,  1024);
		glVertex3f( 1024, -1024,  1024);

		glVertex3f(-1024, -1024, -1024);
		glVertex3f( 1024, -1024, -1024);

		glVertex3f(-1024,  1024, -1024);
		glVertex3f( 1024,  1024, -1024);

		glVertex3f(-1024,  1024,  1024);
		glVertex3f( 1024,  1024,  1024);
	glEnd();
}

void MainApp::drawTerrain(){
	renderer->setShader(terrainShader);
	renderer->setTexture("Noise", noise);
	renderer->setTexture("Base0", base0);
	renderer->setTexture("Base1", base1);
	renderer->setTexture("LightMap", lightMap);
	renderer->apply();

	terrain->draw();
}

void MainApp::drawFire(){
	vec3 dx(modelView.elem[0][0], modelView.elem[0][1], modelView.elem[0][2]);
	vec3 dy(modelView.elem[1][0], modelView.elem[1][1], modelView.elem[1][2]);

	fire->update(time);

	glColor3f(0.75f,0.75f,0.75f);

	glClientActiveTextureARB(GL_TEXTURE0_ARB);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	renderer->setTextures(fireTex);
	renderer->setBlending(ONE, ONE);
	renderer->setMask(COLOR);
	renderer->apply();

	char *vertices = fire->getVertexArray(dx, dy, false, true);
	glVertexPointer  (3, GL_FLOAT, sizeof(vec3) * 2, vertices);
	glTexCoordPointer(3, GL_FLOAT, sizeof(vec3) * 2, vertices + sizeof(vec3));

	glDrawArrays(GL_QUADS, 0, fire->getParticleCount() * 4);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

bool MainApp::drawFrame(){
	glEnable(GL_CULL_FACE);

	renderer->changeMask(ALL);
	glClear(/*GL_COLOR_BUFFER_BIT | */GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	projection = projectionMatrixX(1.5f, float(height) / float(width), 0.1f, 6000);
	glLoadMatrixf(transpose(projection));

    glMatrixMode(GL_MODELVIEW);
	modelView = rotateZXY(-wx, -wy, -wz);
	glLoadMatrixf(transpose(modelView));

	drawSkyBox();

	modelView.translate(-position);
	glLoadMatrixf(transpose(modelView));

	renderer->setShader(woodShader);
	renderer->setTexture("Noise", noise);
	renderer->apply();

	wood->draw();

	drawTerrain();

	drawFire();

	return true;
}

App *app = new MainApp();
