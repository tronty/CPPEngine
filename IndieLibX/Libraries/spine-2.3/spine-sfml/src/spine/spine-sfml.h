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

#ifndef SPINE_SFML_H_
#define SPINE_SFML_H_

#define SPINE_SHORT_NAMES
#include <spine/spine.h>
//include <SFML/Graphics/Vertex.hpp>
//include <SFML/Graphics/VertexArray.hpp>
#include <Framework3/IRenderer.h>

namespace spine {

struct spineTexture {
	TextureID id;
#if 0
	float width;
	float height;
#else
	unsigned int width;
	unsigned int height;
#endif
};
struct spineRenderStates {
	spineTexture texture;
	BlendStateID blendMode;
};

	struct PCTVertex {
		D3DXFROMWINEVECTOR2 vertex;
		D3DXFROMWINEVECTOR2 texCoord;
	};

class SkeletonDrawable {//: public sf::Drawable {
public:
	Skeleton* skeleton;
	AnimationState* state;
	float timeScale;
	//sf::VertexArray* vertexArray;
	std::vector<PCTVertex> vertexArray;

	SkeletonDrawable (SkeletonData* skeleton, AnimationStateData* stateData = 0);
	~SkeletonDrawable ();

	void update (float deltaTime);

	virtual void draw (/*sf::RenderTarget& target, sf::RenderStates states*/);// const;
	virtual void targetdraw(Primitives g_prim, int nn, TextureID id, std::vector<PCTVertex>& vertexArray);// const;
private:
	bool ownsAnimationStateData;
	float* worldVertices;
};

} /* namespace spine */
#endif /* SPINE_SFML_H_ */
