/******************************************************************************
 * Spine Runtimes Software License
 * Version 2.1
 * 
 * Copyright (c) 2013, Esoteric Software
 * All rights reserved.
 * 
 * You are granted a perpetual, non-exclusive, non-sublicensable and
 * non-transferable license to install, execute and perform the Spine Runtimes
 * Software (the "Software") solely for internal use. Without the written
 * permission of Esoteric Software (typically granted by licensing Spine), you
 * may not (a) modify, translate, adapt or otherwise create derivative works,
 * improvements of the Software or develop new applications using the Software
 * or (b) remove, delete, alter or obscure any trademarks or any copyright,
 * trademark, patent or other intellectual property or proprietary rights
 * notices on or in the Software, including any copy thereof. Redistributions
 * in binary or source form must include this license and terms.
 * 
 * THIS SOFTWARE IS PROVIDED BY ESOTERIC SOFTWARE "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ESOTERIC SOFTARE BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/

#include <spine/spine-sfml.h>
#include <spine/extension.h>
/*
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
*/
#ifndef SPINE_MESH_VERTEX_COUNT_MAX
#define SPINE_MESH_VERTEX_COUNT_MAX 1000
#endif

#ifdef __cplusplus
extern "C" {
#endif
void _spAtlasPage_disposeTexture (spAtlasPage* self);
#ifdef __cplusplus
}
#endif

void _spAtlasPage_disposeTexture (spAtlasPage* self)
{
	//TextureID id=((spine::spineTexture*)self->rendererObject)->id;
	//IRenderer::GetRendererInstance()->deleteTexture(id);
}

namespace spine {

void _AtlasPage_disposeTexture (AtlasPage* self){
	delete (Texture*)self->rendererObject;
}

char* _Util_readFile (const char* path, int* length){
	return _readFile(path, length);
}

/**/

SkeletonDrawable::SkeletonDrawable (SkeletonData* skeletonData, AnimationStateData* stateData) :
				timeScale(1),
				worldVertices(0) {
	vertexArray.clear();
	vertexArray.resize(3*skeletonData->bonesCount * 4);
	Bone_setYDown(true);
	worldVertices = MALLOC(float, SPINE_MESH_VERTEX_COUNT_MAX);
	skeleton = Skeleton_create(skeletonData);

	ownsAnimationStateData = stateData == 0;
	if (ownsAnimationStateData) stateData = AnimationStateData_create(skeletonData);

	state = AnimationState_create(stateData);
}

SkeletonDrawable::~SkeletonDrawable () {
	vertexArray.clear();
	FREE(worldVertices);
    if (ownsAnimationStateData) AnimationStateData_dispose(state->data);
	AnimationState_dispose(state);
	Skeleton_dispose(skeleton);
}

void SkeletonDrawable::update (float deltaTime) {
	Skeleton_update(skeleton, deltaTime);
	AnimationState_update(state, deltaTime * timeScale);
	AnimationState_apply(state, skeleton);
	Skeleton_updateWorldTransform(skeleton);
}

void SkeletonDrawable::targetdraw(Primitives aprim, int nn, TextureID textureID, std::vector<PCTVertex>& avertexArray)
{
	IRenderer::GetRendererInstance()->InitTex(IRenderer::GetRendererInstance()->GettexShader(),IRenderer::GetRendererInstance()->GettexVF());	
	IRenderer::GetRendererInstance()->setShader(IRenderer::GetRendererInstance()->GettexShader());
	IRenderer::GetRendererInstance()->setVertexFormat(IRenderer::GetRendererInstance()->GettexVF());

	IRenderer::GetRendererInstance()->setup2DMode(0, (float) IRenderer::GetRendererInstance()->GetViewportWidth(), 0, (float) IRenderer::GetRendererInstance()->GetViewportHeight());
	IRenderer::GetRendererInstance()->setShaderConstant4f("scaleBias", IRenderer::GetRendererInstance()->GetscaleBias2D());
	IRenderer::GetRendererInstance()->setShaderConstant4f("colorRGBA", D3DXFROMWINEVECTOR4(1.0f,1.0f,1.0f,1.0f));
	//IRenderer::GetRendererInstance()->setShaderConstant1f("invertY", 1.0f);

	IRenderer::GetRendererInstance()->setTexture("Base", textureID);
	IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendSrcAlpha());
	IRenderer::GetRendererInstance()->setDepthState(IRenderer::GetRendererInstance()->GetnoDepthTest());
	//IRenderer::GetRendererInstance()->setBlendState(IRenderer::GetRendererInstance()->GetblendOneOne());
	//IRenderer::GetRendererInstance()->setRasterizerState(IRenderer::GetRendererInstance()->GetcullBack());
	IRenderer::GetRendererInstance()->DrawPrimitiveUP(aprim, nn, &avertexArray[0], &avertexArray[0], sizeof(PCTVertex));
}

void SkeletonDrawable::draw (/*RenderTarget& target, spineRenderStates states*/) {

	vertexArray.clear();
	
	//states.blendMode = BlendAlpha;
	spineTexture* statessspineTexture = 0;
	int nn=0;
	float ViewportWidth=IRenderer::GetRendererInstance()->GetViewportWidth();
	float ViewportHeight=IRenderer::GetRendererInstance()->GetViewportHeight();

	for (int i = 0; i < skeleton->slotsCount; ++i) {
		
		Slot* slot = skeleton->drawOrder[i];
		
		Attachment* attachment = slot->attachment;
		
		if (!attachment) continue;
		
		spineTexture* sspineTexture = 0;
		
		if (attachment->type == ATTACHMENT_REGION) {
			
			RegionAttachment* regionAttachment = (RegionAttachment*)attachment;
			
			sspineTexture = (spineTexture*)((AtlasRegion*)regionAttachment->rendererObject)->page->rendererObject;

			RegionAttachment_computeWorldVertices(regionAttachment, slot->bone, worldVertices);

			D3DXFROMWINEVECTOR2 size;
			size.x=1.0f;
			size.y=1.0f;
			size.x=((spineTexture*)((AtlasRegion*)regionAttachment->rendererObject)->page->rendererObject)->width;
			size.y=((spineTexture*)((AtlasRegion*)regionAttachment->rendererObject)->page->rendererObject)->height;
			
			nn=2;
			
			PCTVertex vertices[4];
			/*
			Uint8 r = static_cast<Uint8>(skeleton->r * slot->r * 255);
			Uint8 g = static_cast<Uint8>(skeleton->g * slot->g * 255);
			Uint8 b = static_cast<Uint8>(skeleton->b * slot->b * 255);
			Uint8 a = static_cast<Uint8>(skeleton->a * slot->a * 255);
			vertices[0].color.r = r;
			vertices[0].color.g = g;
			vertices[0].color.b = b;
			vertices[0].color.a = a;
			*/
			
			vertices[0].vertex.x = worldVertices[VERTEX_X1];
			
			vertices[0].vertex.y = worldVertices[VERTEX_Y1];
			
			vertices[0].texCoord.x = regionAttachment->uvs[VERTEX_X1];// * size.x;
			
			vertices[0].texCoord.y = 1.0f-regionAttachment->uvs[VERTEX_Y1];// * size.y;

			/*
			vertices[1].color.r = r;
			vertices[1].color.g = g;
			vertices[1].color.b = b;
			vertices[1].color.a = a;
			*/
			
			vertices[1].vertex.x = worldVertices[VERTEX_X2];
			
			vertices[1].vertex.y = worldVertices[VERTEX_Y2];
			
			vertices[1].texCoord.x = regionAttachment->uvs[VERTEX_X2];// * size.x;
			
			vertices[1].texCoord.y = 1.0f-regionAttachment->uvs[VERTEX_Y2];// * size.y;

			/*
			vertices[2].color.r = r;
			vertices[2].color.g = g;
			vertices[2].color.b = b;
			vertices[2].color.a = a;
			*/
			
			vertices[2].vertex.x = worldVertices[VERTEX_X3];
			
			vertices[2].vertex.y = worldVertices[VERTEX_Y3];
			
			vertices[2].texCoord.x = regionAttachment->uvs[VERTEX_X3];// * size.x;
			
			vertices[2].texCoord.y = 1.0f-regionAttachment->uvs[VERTEX_Y3];// * size.y;

			/*
			vertices[3].color.r = r;
			vertices[3].color.g = g;
			vertices[3].color.b = b;
			vertices[3].color.a = a;
			*/
			
			vertices[3].vertex.x = worldVertices[VERTEX_X4];
			
			vertices[3].vertex.y = worldVertices[VERTEX_Y4];
			
			vertices[3].texCoord.x = regionAttachment->uvs[VERTEX_X4];// * size.x;
			
			vertices[3].texCoord.y = 1.0f-regionAttachment->uvs[VERTEX_Y4];// * size.y;

			//vertexArray.resize(3*skeletonData->bonesCount * 4);
			vertexArray.push_back(vertices[0]);
			vertexArray.push_back(vertices[1]);
			vertexArray.push_back(vertices[2]);
			vertexArray.push_back(vertices[0]);
			vertexArray.push_back(vertices[2]);
			vertexArray.push_back(vertices[3]);
			
		} else if (attachment->type == ATTACHMENT_MESH) {
			
			MeshAttachment* mesh = (MeshAttachment*)attachment;
			
			if (mesh->verticesCount > SPINE_MESH_VERTEX_COUNT_MAX) continue;
			
			sspineTexture = (spineTexture*)((AtlasRegion*)mesh->rendererObject)->page->rendererObject;

			MeshAttachment_computeWorldVertices(mesh, slot, worldVertices);

			/*
			Uint8 r = static_cast<Uint8>(skeleton->r * slot->r * 255);
			Uint8 g = static_cast<Uint8>(skeleton->g * slot->g * 255);
			Uint8 b = static_cast<Uint8>(skeleton->b * slot->b * 255);
			Uint8 a = static_cast<Uint8>(skeleton->a * slot->a * 255);
			vertex.color.r = r;
			vertex.color.g = g;
			vertex.color.b = b;
			vertex.color.a = a;			
			*/
			
			nn=vertexArray.size();

			D3DXFROMWINEVECTOR2 size;
			size.x=1.0f;
			size.y=1.0f;
			size.x=((spineTexture*)((AtlasRegion*)mesh->rendererObject)->page->rendererObject)->width;
			size.y=((spineTexture*)((AtlasRegion*)mesh->rendererObject)->page->rendererObject)->height;
			
			for (int i = 0; i < mesh->trianglesCount; ++i) {
				
				int index = mesh->triangles[i] << 1;

				PCTVertex vertex;
				
				vertex.vertex.x = worldVertices[index];
				
				vertex.vertex.y = worldVertices[index + 1];
				
				vertex.texCoord.x = mesh->uvs[index];// * size.x;
				
				vertex.texCoord.y = 1.0f-mesh->uvs[index + 1];// * size.y;
	
				//vertexArray.resize(3*skeletonData->bonesCount * 4);
				vertexArray.push_back(vertex);
			}

		} else if (attachment->type == ATTACHMENT_SKINNED_MESH) {
			
			SkinnedMeshAttachment* mesh = (SkinnedMeshAttachment*)attachment;
			
			if (mesh->uvsCount > SPINE_MESH_VERTEX_COUNT_MAX) continue;
			
			sspineTexture = (spineTexture*)((AtlasRegion*)mesh->rendererObject)->page->rendererObject;

			SkinnedMeshAttachment_computeWorldVertices(mesh, slot, worldVertices);

			/*
			Uint8 r = static_cast<Uint8>(skeleton->r * slot->r * 255);
			Uint8 g = static_cast<Uint8>(skeleton->g * slot->g * 255);
			Uint8 b = static_cast<Uint8>(skeleton->b * slot->b * 255);
			Uint8 a = static_cast<Uint8>(skeleton->a * slot->a * 255);
			vertex.color.r = r;
			vertex.color.g = g;
			vertex.color.b = b;
			vertex.color.a = a;
			*/
			
			nn=vertexArray.size();

			D3DXFROMWINEVECTOR2 size;
			size.x=1.0f;
			size.y=1.0f;
			size.x=((spineTexture*)((AtlasRegion*)mesh->rendererObject)->page->rendererObject)->width;
			size.y=((spineTexture*)((AtlasRegion*)mesh->rendererObject)->page->rendererObject)->height;
			
			for (int i = 0; i < mesh->trianglesCount; ++i) {
				
				int index = mesh->triangles[i] << 1;
				
				PCTVertex vertex;
				
				vertex.vertex.x = worldVertices[index];
				
				vertex.vertex.y = worldVertices[index + 1];
				
				vertex.texCoord.x = mesh->uvs[index];// * size.x;
				
				vertex.texCoord.y = 1.0f-mesh->uvs[index + 1];// * size.y;
				
				//vertexArray.resize(3*skeletonData->bonesCount * 4);
				vertexArray.push_back(vertex);
				
			}
		}

		if (sspineTexture) {
			// SMFL doesn't handle batching for us, so we'll just force a single sspineTexture per skeleton.
			statessspineTexture = sspineTexture;

			//BlendStateID blend = slot->data->additiveBlending ? IRenderer::GetRendererInstance()->GetblendOneOne() : IRenderer::GetRendererInstance()->GetblendSrcAlpha();
			//if (states.blendMode != blend) {
				
				if(nn) targetdraw(PRIM_TRIANGLES, nn, sspineTexture->id, vertexArray);//, states);
				
				vertexArray.clear();
				
				//states.blendMode = blend;
			//}
		}
	}
	if(nn) targetdraw(PRIM_TRIANGLES, nn, statessspineTexture->id, vertexArray);//vertexArray, states);
	
}

} /* namespace spine */
