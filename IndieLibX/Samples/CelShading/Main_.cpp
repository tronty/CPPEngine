#include "Main.h"
#include "../Framework2/Util/OrderedSet.h"

void MainApp::resetCamera(){
	position = vec3(60, 10, 85);
	wx = 0.28f;
	wy = 2.47f;
	wz = 0;
	speed = 200;
}

OrderedSet <Edge> edges;

void findEdge(const Edge &edge){
	if (edges.goToObject(edge)){
		if (edges.getCurrent().count == 0){
			edges.getCurrent().normal1 = edge.normal0;
		}
		edges.getCurrent().count++;
	} else {
		edges.insert(edge);
	}
}

void MainApp::initMenu(){
	drawOutline = true;
	drawEdge = true;
	edgeThreshold = 71;
	lineWidth = 3;

	Menu *menu = menuSystem->getMainMenu();
	menu->addMenuItem("Draw outline: ", &drawOutline, INPUT_BOOL);
	menu->addMenuItem("Draw edge: ", &drawEdge, INPUT_BOOL);
	menu->addMenuItem("Edge threshold: ", &edgeThreshold, INPUT_INTEGER, 0, 100);
	menu->addMenuItem("Line width: ", &lineWidth, INPUT_INTEGER, 1, 5);

	App::initMenu();
}

bool MainApp::init(){
	model = new OpenGLModel();
	model->loadFromFile("../Models/64thundr.hmdl");
	model->getBatch(0)->optimize(0x7FFFFFFF);

	unsigned int nIndices = model->getBatch(0)->getIndexCount();
	unsigned short *indices = (unsigned short *) model->getBatch(0)->getIndices();
	//unsigned int *indices = (unsigned int *) model->getBatch(0)->getIndices();
	vec3 *vertices = (vec3 *) model->getBatch(0)->getVertices();

	unsigned int i;
	for (i = 0; i < nIndices; i += 3){
		vec3 v0 = vertices[indices[i]];
		vec3 v1 = vertices[indices[i + 1]];
		vec3 v2 = vertices[indices[i + 2]];

		vec3 normal = normalize(cross(v1 - v0, v2 - v0));

		findEdge(Edge(indices[i    ], indices[i + 1], normal));
		findEdge(Edge(indices[i + 1], indices[i + 2], normal));
		findEdge(Edge(indices[i + 2], indices[i    ], normal));
	}

	unsigned int c = 0;
	edges.goToFirst();
	do {
		if (edges.getCurrent().count == 1){
			c++;
		}
	} while (edges.next());


	Vertex *newVertices = new Vertex[2 * c];
	Vertex *dest = newVertices;

	edges.goToFirst();
	do {
		if (edges.getCurrent().count == 1){
			dest[0].vertex = vertices[edges.getCurrent().i0];
			dest[1].vertex = vertices[edges.getCurrent().i1];
			dest[0].normal0 = edges.getCurrent().normal0;
			dest[1].normal0 = edges.getCurrent().normal0;
			dest[0].normal1 = edges.getCurrent().normal1;
			dest[1].normal1 = edges.getCurrent().normal1;
			dest += 2;
		}
	} while (edges.next());

	edgeModel = new OpenGLModel();
	OpenGLBatch *batch = new OpenGLBatch();
	batch->addFormat(ATT_VERTEX,   ATT_FLOAT, 3, 0);
	batch->addFormat(ATT_TEXCOORD, ATT_FLOAT, 3, sizeof(vec3),     0);
	batch->addFormat(ATT_TEXCOORD, ATT_FLOAT, 3, sizeof(vec3) * 2, 1);
	batch->setPrimitiveType(PRIM_LINES);
	batch->setVertices(newVertices, 2 * c, sizeof(Vertex));
	edgeModel->addBatch(batch);

//	edgeModel->saveToFile("../Models/64thundrEdge.hmdl");

	delete model;
	model = new OpenGLModel();
	model->loadFromFile("../Models/64thundr.hmdl");
	model->getBatch(0)->addNormals();

	return true;
}

bool MainApp::exit(){
	delete edgeModel;
	delete model;

	return true;
}

bool MainApp::load(){
	if (!GL_ARB_shader_objects_supported || !GL_ARB_vertex_shader_supported || !GL_ARB_fragment_shader_supported || !GL_ARB_shading_language_100_supported){
		addToLog("No GLSL support (GL_ARB_shader_objects, GL_ARB_vertex_shader, GL_ARB_fragment_shader, GL_ARB_shading_language_100)\n");
		return false;
	}

	setDefaultFont("../Textures/Fonts/Future.font", "../Textures/Fonts/Future.dds");

	if ((shading = renderer->addShader("shading.shd")) == SHADER_NONE) return false;
	if ((outline = renderer->addShader("outline.shd")) == SHADER_NONE) return false;

	if ((celShade = renderer->addTexture("../Textures/celshade.tga", TEX_CLAMP | TEX_1D)) == TEXTURE_NONE) return false;

	model->uploadToVertexBuffer();
	edgeModel->uploadToVertexBuffer();

	return true;
}

bool MainApp::unload(){
	return true;
}


bool MainApp::drawFrame(){
	glEnable(GL_CULL_FACE);

	renderer->changeMask(ALL);
	glClearColor(0.7f, 0.7f, 0.7f, 0.5f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	projection = projectionMatrixX(1.5f, float(height) / float(width), 0.1f, 1000);
	glLoadMatrixf(transpose(projection));

    glMatrixMode(GL_MODELVIEW);
	modelView = rotateZXY(-wx, -wy, -wz);
	modelView.translate(-position);
	glLoadMatrixf(transpose(modelView));


	glLineWidth((float) lineWidth);

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(3, 3);

	renderer->setShader(shading);
	renderer->setTexture("CelShade", celShade);
	renderer->apply();

	renderer->changeShaderConstant3f("lightPos", position);

	model->draw();
	
	glDisable(GL_POLYGON_OFFSET_FILL);


	if (drawOutline || drawEdge){
		renderer->setShader(outline);
		renderer->setMask(COLOR);
		renderer->apply();

		renderer->changeShaderConstant3f("camPos", position);
		renderer->changeShaderConstant1f("outlineThreshold", drawOutline? 0.0f : -100000000.0f);
		renderer->changeShaderConstant1f("edgeThreshold", drawEdge? edgeThreshold * 0.01f * 2 - 1 : -1.0f);

		edgeModel->draw();
	}

	return true;
}

App *app = new MainApp();
