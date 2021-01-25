/*
  Copyright (c) 2021 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
#include <STX/OrderedSet.h>

struct Edge {
	Edge(){
		count = 0;
	}
	Edge(const unsigned int ii0, const unsigned int ii1, const D3DXFROMWINEVECTOR3 &normal){
		i0 = min(ii0, ii1);
		i1 = max(ii0, ii1);
		normal0 = normal;
		count = 0;
	}

	unsigned int i0, i1;
	D3DXFROMWINEVECTOR3 normal0, normal1;
	unsigned int count;
};

bool operator != (const Edge &e0, const Edge &e1){
	return (e0.i0 != e1.i0 || e0.i1 != e1.i1);
}

bool operator < (const Edge &e0, const Edge &e1){
	if (e0.i0 < e1.i0) return true;
	if (e0.i0 > e1.i0) return false;
	return (e0.i1 < e1.i1);
}

struct Vertex {
	D3DXFROMWINEVECTOR3 vertex;
	D3DXFROMWINEVECTOR3 normal0;
	D3DXFROMWINEVECTOR3 normal1;
};

class MainApp {
protected:
	//OpenGLModel *model, *edgeModel;

	ShaderID outline, shading;
	TextureID celShade;

	int edgeThreshold, lineWidth;
	bool drawOutline, drawEdge;

    D3DXFROMWINEVECTOR3 position;
    float wx, wy, wz, speed;

public:
void resetCamera(){
	position = D3DXFROMWINEVECTOR3(60, 10, 85);
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

void initMenu(){
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

bool init(){

	unsigned int nIndices = model->getBatch(0)->getIndexCount();
	unsigned short *indices = (unsigned short *) model->getBatch(0)->getIndices();
	D3DXFROMWINEVECTOR3 *vertices = (D3DXFROMWINEVECTOR3 *) model->getBatch(0)->getVertices();

	unsigned int i;
	for (i = 0; i < nIndices; i += 3){
		D3DXFROMWINEVECTOR3 v0 = vertices[indices[i]];
		D3DXFROMWINEVECTOR3 v1 = vertices[indices[i + 1]];
		D3DXFROMWINEVECTOR3 v2 = vertices[indices[i + 2]];

		D3DXFROMWINEVECTOR3 normal = normalize(cross(v1 - v0, v2 - v0));

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
	batch->addFormat(ATT_TEXCOORD, ATT_FLOAT, 3, sizeof(D3DXFROMWINEVECTOR3),     0);
	batch->addFormat(ATT_TEXCOORD, ATT_FLOAT, 3, sizeof(D3DXFROMWINEVECTOR3) * 2, 1);
	batch->setPrimitiveType(PRIM_LINES);
	batch->setVertices(newVertices, 2 * c, sizeof(Vertex));
	edgeModel->addBatch(batch);

	return true;
}

bool load(){

	if ((shading = IRenderer::GetRendererInstance()->addShaderFromFile("/CelShading/NVIDIA/shading.shd")) == SHADER_NONE) return false;
	if ((outline = IRenderer::GetRendererInstance()->addShaderFromFile("/CelShading/NVIDIA/outline.shd")) == SHADER_NONE) return false;

	if ((celShade = IRenderer::GetRendererInstance()->addTexture("/CelShading/NVIDIA/celshade.tga", TEX_CLAMP | TEX_1D)) == TEXTURE_NONE) return false;

	model->uploadToVertexBuffer();
	edgeModel->uploadToVertexBuffer();

	return true;
}

bool drawFrame(){
	//glEnable(GL_CULL_FACE);

	//IRenderer::GetRendererInstance()->changeMask(ALL);

	D3DXFROMWINEMATRIX mvp, R, T;
	D3DXFROMWINEMATRIX projection;
    D3DXFROMWINEMatrixPerspectiveFovLH( &projection, 1.5f, IRenderer::GetRendererInstance()->GetAspect(), 0.1f, 1000.0f);

    D3DXFROMWINEMATRIX modelView = rotateZXY(-wx, -wy, -wz);
		D3DXFROMWINEMatrixRotationYawPitchRoll( &R, -wx, -wy, -wz);
		D3DXFROMWINEMatrixTranslation(&T, -position.x, -position.y, -position.z);

	modelView=R*T
	mvp=projection*modelView;


	//glLineWidth((float) lineWidth);

	//glEnable(GL_POLYGON_OFFSET_FILL);
	//glPolygonOffset(3, 3);

	IRenderer::GetRendererInstance()->setShader(shading);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", mvp);
	IRenderer::GetRendererInstance()->setTexture("CelShade", celShade);

	IRenderer::GetRendererInstance()->setShaderConstant3f("lightPos", position);

	model->draw();
	
	//glDisable(GL_POLYGON_OFFSET_FILL);

	if (drawOutline || drawEdge){
		IRenderer::GetRendererInstance()->setShader(outline);
	IRenderer::GetRendererInstance()->setShaderConstant4x4f("mvp", mvp);
		IRenderer::GetRendererInstance()->setMask(eCOLOR);

		IRenderer::GetRendererInstance()->setShaderConstant3f("camPos", position);
		IRenderer::GetRendererInstance()->setShaderConstant1f("outlineThreshold", drawOutline? 0.0f : -100000000.0f);
		IRenderer::GetRendererInstance()->setShaderConstant1f("edgeThreshold", drawEdge? edgeThreshold * 0.01f * 2 - 1 : -1.0f);

		edgeModel->draw();
	}

	return true;
}
};

MainApp app;
int init(const char* aTitle)
{
    app.init();
    app.load();
}

void render()
{
	IRenderer::GetRendererInstance()->Clear(true, true, D3DXFROMWINEVECTOR4(0.7f, 0.7f, 0.7f, 0.5f));
	IRenderer::GetRendererInstance()->BeginScene();
	app.drawFrame();
	IRenderer::GetRendererInstance()->EndScene();
	IRenderer::GetRendererInstance()->Present( );
}

int ApplicationLogic()
{
	IRenderer* r=IRenderer::GetRendererInstance("CelShading");
	IInput*    i=STX_Service::GetInputInstance();
	init("");
	while (!i->OnKeyPress (STX_KEY_ESCAPE) && !i->Quit())
	{
		i->Update();
		render();
	}
	return 0;
}

