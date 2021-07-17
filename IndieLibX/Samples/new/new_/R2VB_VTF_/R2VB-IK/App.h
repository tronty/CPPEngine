#ifndef _APP_H_
#define _APP_H_

#include "../../Framework/Direct3D/D3D9App.h"
#include "../../Framework/Util/ATIModel.h"
#include "../../Framework/Util/Model.h"

struct Link{
	mat4 pivotMatrix;
	mat4 transformMatrix;

	TextureID tOPivotMatrix;
	TextureID tRotationDegMap[2], tPivotMatrix[2], tTransformMatrix;

	Link *parent;
	Model *pModel;
};

#define NUM_LINKS				4
#define LINK_LEN				9.0f

class Target : public ATIModel {
public:
	bool load(char *name, Renderer* renderer);

	void setup(Renderer *renderer);

protected:
	VertexFormatID VertexFormat;
	VertexBufferID VertexBuffer;
	IndexBufferID IndexBuffer;
	ShaderID Shader;
	TextureID DiffuseMap;
};

class App : public D3D9App {
public:
	char *getTitleString(){ return "R2VB Inverse Kinematics"; }
	char *getHomeDirectory(){ return "../DirectX 9.0/R2VB-IK"; }

	void initGUI();
	void onSize(const int w, const int h);
	bool onReset();

	void setup(char *args[], int nArguments);
	void resetCamera();

	void setupIKMatrix(vec3& pos, int times);

	void computeTransformMatrix(int i);

	bool createLinksVBO();
	bool load();
	bool unload();


	void computeCCD(int i, vec3& endpos);
	void combineTransformMatrix();
	void transformLinks();
	void drawLinks(float4x4& mvp);
	void drawTarget(float4x4& mvp);
	void drawFrame();
	void dumpTexture(TextureID ti);

private:
	vec3 endPos;
	Target target;
	Link link[NUM_LINKS];
	TextureID DiffuseMap;

	int numVertices, numTriangles;
	TextureID identityMatrix;
	TextureID combinedTransformMatrix;
	VertexFormatID VF;
	TextureID linkVertexTexture, matrixIndexTexture, R2VB_VBO;
	VertexBufferID R2VB_DummyVBO, texCoord_VBO;
	IndexBufferID IB;

	ShaderID quadShd, computeCCDShd, generatePivotMatrixShd, computeTransformMatrixShd, transformLinkShd, drawLinkShd;
	VertexFormatID QuadVF;
	VertexBufferID QuadVB;

	int currentPivotMatrix, outputPivotMatrix;
	int currentRoatationDegMap, outputRoatationDegMap;
};

#endif
