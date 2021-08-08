#ifndef _APP_H_
#define _APP_H_

#include "../../Framework/Direct3D/D3D9App.h"

#define D2R				(3.141590f/180.0f)
#define MAX_NUM_MODELS							65536

#define MAX_NUM_BONES							120		
#define MAX_NUM_ANIMATION_FRAMES				256

enum NPC_State{
	RUN = 0,
	DYING,
	DANCE,
	RESPAWN,
	KICK,
};

#define RUN_T0									1.0f
#define RUN_T1									21.0f

#define DIE_T0									22.0f
#define DIE_T1									30.0f
#define DIE_T2									37.0f

#define DANCE_T0								40.0f
#define DANCE_T1								113.0f

#define KICK_T0									114.0f
#define KICK_T1									171.0f

#define RESPAWN_T0								173.0f
#define RESPAWN_T1								198.0f


typedef struct {
	float			sT, eT;
	float			value[4];
}AniKey;

typedef struct {
	float			ID;
	float			parentID;
	DWORD			numPosKeys;
	DWORD			numRotKeys;
	DWORD			numSclKeys;
}Bone;

typedef struct {
	DWORD			numVertices;
	DWORD			numTriangles;
	DWORD			numBones;	
	DWORD			numFrames;
	float4x4		bonesMatrix[MAX_NUM_ANIMATION_FRAMES][MAX_NUM_BONES];

	VertexFormatID	vertexFormat;
	TextureID		R2VB_VBO;	
	VertexBufferID	R2VB_DummyVBO, R2VB_OnceDummyVBO, UVBuffer;
	TextureID		skinningVertex, vertexBoneIndex, vertexWeight, boneAnimation, boneMatrix, diffuse;

	VertexFormatID	vsVertexFormat;
	VertexBufferID	vsVertexBuffer;

	IndexBufferID	indexBuffer;	

	DWORD			vertexTextureWidth, vertexTextureHeight;		
}Model;

typedef struct {
	DWORD			numVertices;
	DWORD			numTriangles;

	TextureID		R2VB_VBO;	
	int				currentFrameTimeTexture;
	TextureID		FrameTime[3];
	TextureID		boneMatrix;

	VertexFormatID	vertexFormat;
	VertexBufferID	UVBuffer;

	IndexBufferID	indexBuffer;		

	DWORD			vertexTextureWidth, vertexTextureHeight;
}Batch;

typedef struct {
	float3			Scale;
	float4x4		Trans;
	float			Speed;
	float			Deg;
	bool			bBounce;
	bool			bInSafeZone;

	float			time;
	float			interpolate;

	bool			bDie, bBounceByGround;
	float			upSpeed;
	vec3			flyDir;

	NPC_State		currentState;	
}AI;

class App : public D3D9App {
public:
	char *getTitleString(){ return "R2VB skinning animation"; }
	char *getHomeDirectory(){ return "../DirectX 9.0/R2VB-Animation"; }
	void setup(char *args[], int nArguments);
	void resetCamera();

	void initGUI();

	bool loadModel(Model *m, Batch *b, char* name);
	bool loadBoneAnimationMap(Model *m, Batch *b, char* name);
	bool loadSkinningVertexMap(Model *m, char* name);
	bool load();

	void onSize(const int w, const int h);
	bool onReset();
	void resetModels();

	void vsCalculateAnimation(float t, float interp, float scale = 1.0f);
	void vsDrawModel(int mi);

	void psCalculateAnimation(float t, float interp);
	void psSkinning();
	void psDrawModel(int mi);

	void psBatchCalculateAnimation(int num);
	void psBatchSkinning(int num);
	void psBatchDrawModel(int num);

	void drawVertexShaderAnimation();
	void drawPixelShaderAnimation();
	void AILoop();
	void drawFrame();

private:
	Menu *menu, *animationMenu, *AIMenu;
	MenuItemID vsAnimation, psAnimation, sameAnimation, differentAnimation, noAI, respawn, bounce, lastAI;
	Slider *numModelSlider;

	VertexFormatID QuadVF, batch_QuadVF;
	VertexBufferID QuadVB, batch_QuadVB;

	Model model[2];
	AI modelAI[MAX_NUM_MODELS];
	Batch batch[2];
	DWORD maxBatchOnce;

	float4x4 MVP;
	float time;
	float interpolate;
	int	numModels;	
	float4 Quat;
	int currentModel;
	ShaderID animationSH, skinningSH, modelSH, batchAnimationSH, batchSkinningSH, batchSkinningNormalSH, batchModelSH, vsModelSH;	
};

#endif
