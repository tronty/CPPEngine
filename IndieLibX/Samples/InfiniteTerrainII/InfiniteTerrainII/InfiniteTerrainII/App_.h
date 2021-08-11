/***********      .---.         .-"-.      *******************\
* -------- *     /   ._.       / ´ ` \     * ---------------- *
* Author's *     \_  (__\      \_°v°_/     * humus@rogers.com *
*   note   *     //   \\       //   \\     * ICQ #47010716    *
* -------- *    ((     ))     ((     ))    * ---------------- *
*          ****--""---""-------""---""--****                  ********\
* This file is a part of the work done by Humus. You are free to use  *
* the code in any way you like, modified, unmodified or copy'n'pasted *
* into your own work. However, I expect you to respect these points:  *
*  @ If you use this file and its contents unmodified, or use a major *
*    part of this file, please credit the author and leave this note. *
*  @ For use in anything commercial, please request my approval.      *
*  @ Share your work and ideas too as much as you can.                *
\*********************************************************************/

#include "../Framework3/OpenGL/OpenGLApp.h"
#include "../Framework3/Math/Noise.h"
#include "../Framework3/Math/Frustum.h"

#define TERRAIN_SIZE 512
#define INDEX_COUNT ((TERRAIN_SIZE - 1) * (2 * TERRAIN_SIZE + 1) - 1)

#define NOISE_SIZE 128


#define R2VB_ENABLE  0x70ff0001
#define R2VB_DISABLE 0x70ff0000

#define R2VB_DMAP 0
#define R2VB_VTX0 1
#define R2VB_VTX1 2
#define R2VB_VTX2 3
#define R2VB_VTX3 4

#define R2VB_MAP_STREAM(stream, sampler) (0x71ff0000 | stream | (sampler << 4))
#define R2VB_UNMAP_STREAM(stream) (0x71ff0050 | stream)


class App : public OpenGLApp {
public:
	char *getTitle() const { return "Infinite Terrain II"; }

	void resetCamera();

	bool onKey(const uint key, const bool pressed);
	void onSize(const int w, const int h);

	bool init();

	void fillTerrainIB(uint *iDest, const bool zDir);
	bool load();

	void drawEnvironment(const float4x4 &mvp);
	void drawTerrain(const float4x4 &mvp);
	void drawFrame();

protected:
	float3 lightDir;
	float4 fogColor;

	TextureID heightRT, terrainRT;
	ShaderID genTerrain, tangentSpace;

	Image noiseImage;
	TextureID noise;

	VertexBufferID terrainVB;
	IndexBufferID  terrainIBs[2];
	VertexFormatID terrainVF, skyboxVF, quadVF;

	TextureID ground, env;
	ShaderID terrain, skyBox;

	SamplerStateID linear, nearestClamp, bilinearClamp, trilinearAniso;

	int lastX, lastZ;
	int nUpdates;
	bool useR2VB;
	bool showInfo;
};
