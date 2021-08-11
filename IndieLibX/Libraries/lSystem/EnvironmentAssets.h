#ifndef _SKYBOX_H_
#define _SKYBOX_H_

#include "drawable.h"

class Skybox : public IDrawable
{
private:
	const Camera& m_camera;
	GLuint m_textureId;
	GLuint m_groundTexture;
	GLUquadric* m_pQuadric;
	bool m_fShow;
public:
	Skybox(const Camera& camera);
	void draw() const;
	void toggleShow() { m_fShow = !m_fShow; }
};

#endif//_SKYBOX_H_
