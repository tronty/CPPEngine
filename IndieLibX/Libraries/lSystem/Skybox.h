#ifndef _SKYBOX_H_
#define _SKYBOX_H_

#include "drawable.h"

class Skybox : public IDrawable
{
private:
	const Camera& m_camera;
	GLuint m_textureId;
	GLUquadric* m_pQuadric;
public:
	Skybox(const Camera& camera);
	void draw() const;
};

#endif//_SKYBOX_H_
