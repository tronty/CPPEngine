
#ifndef _IND_Viewport_
#define _IND_Viewport_

class IND_Viewport
{
public:
	IND_Viewport()
	{
		X=0;
		Y=0;
		Width=0;
		Height=0;
	}
	~IND_Viewport(){}
	unsigned int X;
	unsigned int Y;
	unsigned int Width;
	unsigned int Height;
	IND_Viewport(	unsigned int aX,
			unsigned int aY,
			unsigned int aWidth,
			unsigned int aHeight)
	{
		X=aX;
		Y=aY;
		Width=aWidth;
		Height=aHeight;
	}
	IND_Viewport(const IND_Viewport& rhs)
	{
		X=rhs.X;
		Y=rhs.Y;
		Width=rhs.Width;
		Height=rhs.Height;
	}
};

#endif

