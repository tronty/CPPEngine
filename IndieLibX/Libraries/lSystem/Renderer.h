#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <list>
#include "Camera.h"

class IDrawable;
struct SDL_Surface;

class Renderer
{
private:
	int m_bpp;
	std::tuple<int, int> m_screenSize;
	SDL_Surface* m_pScreen; //Owned by SDL??
	std::list<std::unique_ptr<IDrawable>> m_drawableList;
	Camera m_camera;

public:
	Renderer(int screenX, int screenY, int bpp);
	bool init();
	bool draw();
	void flip();
	void addDrawable(std::unique_ptr<IDrawable> pDrawable);
	void removeDrawable(IDrawable* pDrawable);
	Camera& getCamera() throw() { return m_camera; }
    //void applyTransFormation(const Matrix4& matrix)
};

#endif//_RENDERER_H_